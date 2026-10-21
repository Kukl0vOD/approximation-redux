#include "concentration_extractor.h"
#include "json_builder.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace
{
    constexpr double fraction_tolerance = 1e-9;
    constexpr double pressure_tolerance = 1e-9;

    json::Dict loadJsonObject(const std::filesystem::path& path)
    {
        std::ifstream input(path);
        if (!input.is_open())
        {
            throw std::runtime_error("Cannot open JSON file: " + path.string());
        }

        try
        {
            return json::Load(input).GetRoot().AsMap();
        }
        catch (const std::exception& error)
        {
            throw std::runtime_error(
                "Cannot parse JSON file '" + path.string() + "': " + error.what()
            );
        }
    }

    const json::Node& requiredNode(
        const json::Dict& object,
        const std::string& key,
        const std::filesystem::path& source
    )
    {
        const auto item = object.find(key);
        if (item == object.end())
        {
            throw std::runtime_error(
                "Missing key '" + key + "' in JSON file: " + source.string()
            );
        }
        return item->second;
    }

    std::vector<double> readNumberArray(
        const json::Dict& object,
        const std::string& key,
        const std::filesystem::path& source
    )
    {
        const auto& input = requiredNode(object, key, source).AsArray();
        std::vector<double> result;
        result.reserve(input.size());

        for (const auto& value : input)
        {
            result.push_back(value.AsDouble());
        }
        return result;
    }

    void requireSameSize(
        const std::vector<double>& values,
        std::size_t expected_size,
        const std::string& key,
        const std::filesystem::path& source
    )
    {
        if (values.size() != expected_size)
        {
            throw std::runtime_error(
                "Array '" + key + "' has an unexpected size in JSON file: " +
                source.string()
            );
        }
    }

    bool nearlyEqual(double lhs, double rhs, double tolerance)
    {
        const auto scale = (std::max)({ 1.0, std::abs(lhs), std::abs(rhs) });
        return std::abs(lhs - rhs) <= tolerance * scale;
    }

    std::size_t findPressureIndex(
        const std::vector<double>& pressures,
        double requested_pressure
    )
    {
        const auto item = std::find_if(
            pressures.begin(),
            pressures.end(),
            [requested_pressure](double pressure)
            {
                return nearlyEqual(pressure, requested_pressure, pressure_tolerance);
            }
        );

        if (item == pressures.end())
        {
            throw std::runtime_error(
                "Pressure " + std::to_string(requested_pressure) +
                " from K-values is absent in molar fractions data"
            );
        }
        return static_cast<std::size_t>(std::distance(pressures.begin(), item));
    }

    void addNumberArray(
        json::Builder& builder,
        const std::string& key,
        const std::vector<double>& values
    )
    {
        builder.Key(key).StartArray();
        for (const auto value : values)
        {
            builder.Value(value);
        }
        builder.EndArray();
    }

    std::string temperaturePrefix(double temperature_kelvin)
    {
        std::ostringstream output;
        output << std::setprecision(15) << temperature_kelvin;
        return output.str();
    }
}

namespace extractor
{
    ConcentrationExtractor::ConcentrationExtractor(
        const std::filesystem::path& solution_json,
        const std::filesystem::path& molar_fractions_json,
        const std::filesystem::path& kvalues_json
    )
    {
        readSolutionJson(solution_json);
        readMolarFractionsJson(molar_fractions_json);
        readKValuesJson(kvalues_json);
        calculateConcentrations();
    }

    void ConcentrationExtractor::readSolutionJson(
        const std::filesystem::path& solution_json
    )
    {
        const auto solution_data = loadJsonObject(solution_json);

        pressure_dimension_ = requiredNode(
            solution_data,
            "Pressure_dimension",
            solution_json
        ).AsString();
        solution_ = requiredNode(solution_data, "Solution", solution_json).AsArray();
        full_concentrations_.reserve(solution_.size());
        component_names_.reserve(solution_.size());

        for (const auto& component_node : solution_)
        {
            const auto& component = component_node.AsMap();
            const auto name = requiredNode(component, "name", solution_json).AsString();
            const auto concentration = requiredNode(
                component,
                "concentration",
                solution_json
            ).AsDouble();

            if (concentration < 0.0)
            {
                throw std::runtime_error("Solution contains a negative concentration: " + name);
            }
            component_names_.push_back(name);
            full_concentrations_.push_back(concentration);
        }

        const auto concentration_sum = std::accumulate(
            full_concentrations_.begin(),
            full_concentrations_.end(),
            0.0
        );
        if (!nearlyEqual(concentration_sum, 1.0, fraction_tolerance))
        {
            throw std::runtime_error("Solution component concentrations do not sum to one");
        }
    }

    void ConcentrationExtractor::readMolarFractionsJson(
        const std::filesystem::path& molar_fractions_json
    )
    {
        const auto molar_fraction_data = loadJsonObject(molar_fractions_json);

        double temperature_celsius = requiredNode(
            molar_fraction_data,
            "temperature_celsius",
            molar_fractions_json
        ).AsDouble();
        temperature_kelvin_ = requiredNode(
            molar_fraction_data,
            "temperature_kelvin",
            molar_fractions_json
        ).AsDouble();
        if (!nearlyEqual(
                temperature_kelvin_,
                temperature_celsius + 273.15,
                fraction_tolerance
            ))
        {
            throw std::runtime_error(
                "Celsius and Kelvin temperatures differ in molar fractions data"
            );
        }

        const auto molar_pressure_dimension = requiredNode(
            molar_fraction_data,
            "pressure_dimension",
            molar_fractions_json
        ).AsString();
        if (pressure_dimension_ != molar_pressure_dimension)
        {
            throw std::runtime_error("Solution and molar fractions use different pressure dimensions");
        }

        fraction_pressures_ = readNumberArray(
            molar_fraction_data,
            "P",
            molar_fractions_json
        );
        liquid_molar_fractions_ = readNumberArray(
            molar_fraction_data,
            "oil",
            molar_fractions_json
        );
        gas_molar_fractions_ = readNumberArray(
            molar_fraction_data,
            "gas",
            molar_fractions_json
        );
        requireSameSize(
            liquid_molar_fractions_,
            fraction_pressures_.size(),
            "oil",
            molar_fractions_json
        );
        requireSameSize(
            gas_molar_fractions_,
            fraction_pressures_.size(),
            "gas",
            molar_fractions_json
        );
    }

    void ConcentrationExtractor::readKValuesJson(
        const std::filesystem::path& kvalues_json
    )
    {
        const auto kvalue_data = loadJsonObject(kvalues_json);

        const auto kvalue_temperature = requiredNode(
            kvalue_data,
            "temperature_kelvin",
            kvalues_json
        ).AsDouble();
        if (!nearlyEqual(temperature_kelvin_, kvalue_temperature, fraction_tolerance))
        {
            throw std::runtime_error("Molar fractions and K-values temperatures differ");
        }

        const auto kvalue_pressure_dimension = requiredNode(
            kvalue_data,
            "pressure_dimension",
            kvalues_json
        ).AsString();
        if (pressure_dimension_ != kvalue_pressure_dimension)
        {
            throw std::runtime_error("Solution and K-values use different pressure dimensions");
        }

        pressures_ = readNumberArray(kvalue_data, "P", kvalues_json);

        for (const auto& component_name : component_names_)
        {
            auto values = readNumberArray(kvalue_data, component_name, kvalues_json);
            requireSameSize(values, pressures_.size(), component_name, kvalues_json);
            kvalues_.emplace(component_name, std::move(values));
        }
    }

    void ConcentrationExtractor::calculateConcentrations()
    {
        for (const auto& component_name : component_names_)
        {
            liquid_concentrations_[component_name].reserve(pressures_.size());
            gas_concentrations_[component_name].reserve(pressures_.size());
        }

        for (std::size_t pressure_index = 0; pressure_index < pressures_.size(); ++pressure_index)
        {
            const auto fraction_index = findPressureIndex(
                fraction_pressures_,
                pressures_[pressure_index]
            );
            const auto liquid_fraction = liquid_molar_fractions_[fraction_index];
            const auto gas_fraction = gas_molar_fractions_[fraction_index];

            if (liquid_fraction < 0.0 || gas_fraction < 0.0 ||
                !nearlyEqual(liquid_fraction + gas_fraction, 1.0, fraction_tolerance))
            {
                throw std::runtime_error(
                    "Invalid molar fractions at pressure " +
                    std::to_string(pressures_[pressure_index])
                );
            }

            double liquid_concentration_sum = 0.0;
            double gas_concentration_sum = 0.0;

            for (std::size_t component_index = 0;
                 component_index < component_names_.size();
                 ++component_index)
            {
                const auto& component_name = component_names_[component_index];
                const auto kvalue = kvalues_.at(component_name)[pressure_index];
                const auto denominator = liquid_fraction + gas_fraction * kvalue;

                if (kvalue < 0.0 || denominator <= std::numeric_limits<double>::epsilon())
                {
                    throw std::runtime_error(
                        "Invalid K-value for component '" + component_name + "'"
                    );
                }

                const auto liquid_concentration =
                    full_concentrations_[component_index] / denominator;
                const auto gas_concentration = liquid_concentration * kvalue;

                liquid_concentrations_.at(component_name).push_back(liquid_concentration);
                gas_concentrations_.at(component_name).push_back(gas_concentration);
                liquid_concentration_sum += liquid_concentration;
                gas_concentration_sum += gas_concentration;
            }

            if (!nearlyEqual(liquid_concentration_sum, 1.0, fraction_tolerance) ||
                !nearlyEqual(gas_concentration_sum, 1.0, fraction_tolerance))
            {
                throw std::runtime_error(
                    "Calculated phase concentrations do not sum to one at pressure " +
                    std::to_string(pressures_[pressure_index])
                );
            }
        }
    }

    void ConcentrationExtractor::exportToJson(
        const std::filesystem::path& output_json
    ) const
    {
        std::string prefix = temperaturePrefix(temperature_kelvin_);
        std::string gas_name = prefix + "_gas";
        std::string oil_name = prefix + "_oil";

        json::Builder builder;
        builder.StartDict()
            .Key("Pressure_dimension").Value(pressure_dimension_)
            .Key("Solution").StartArray();

        for (const auto& component : solution_)
        {
            builder.Value(component);
        }

        builder.EndArray()
            .Key("Concentrations").StartDict()
                .Key("concentration_names").StartArray()
                    .Value(gas_name)
                    .Value(oil_name)
                .EndArray()
                .Key("concentration_state").StartDict()
                    .Key(gas_name).StartDict()
                        .Key("temperature").Value(temperature_kelvin_)
                        .Key("phase").Value(std::string("gas"))
                    .EndDict()
                    .Key(oil_name).StartDict()
                        .Key("temperature").Value(temperature_kelvin_)
                        .Key("phase").Value(std::string("oil"))
                    .EndDict()
                .EndDict()
                .Key(gas_name).StartDict();

        addNumberArray(builder, "P", pressures_);
        for (const auto& component_name : component_names_)
        {
            addNumberArray(
                builder,
                component_name,
                gas_concentrations_.at(component_name)
            );
        }

        builder.EndDict()
            .Key(oil_name).StartDict();

        addNumberArray(builder, "P", pressures_);
        for (const auto& component_name : component_names_)
        {
            addNumberArray(
                builder,
                component_name,
                liquid_concentrations_.at(component_name)
            );
        }

        const json::Document output_document(
            builder.EndDict().EndDict().EndDict().Build()
        );

        if (!output_json.parent_path().empty())
        {
            std::filesystem::create_directories(output_json.parent_path());
        }
        std::ofstream output(output_json);
        if (!output.is_open())
        {
            throw std::runtime_error("Cannot open output JSON file: " + output_json.string());
        }

        output << std::setprecision(std::numeric_limits<double>::max_digits10);
        json::Print(output_document, output);
        if (!output)
        {
            throw std::runtime_error("Cannot write output JSON file: " + output_json.string());
        }
    }
}
