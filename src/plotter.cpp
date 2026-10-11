#include "plotter.h"

#include "view/plot_file_name_formatter.h"
#include "view/plot_styles.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>

namespace
{
    struct Intersection
    {
        double pressure;
        double volume;
    };

    struct KValuesIntersection
    {
        double pressure;
        double k_value;
    };

    std::optional<Intersection> findIntersection(
        const std::vector<double>& pressures1,
        const std::vector<double>& volumes1,
        const std::vector<double>& pressures2,
        const std::vector<double>& volumes2)
    {
        const size_t points_count = (std::min)({ pressures1.size(), volumes1.size(), pressures2.size(), volumes2.size() });
        if (points_count == 0)
        {
            return std::nullopt;
        }

        constexpr double pressure_tolerance = 1e-9;
        auto difference = [&](size_t index)
        {
            return volumes1[index] - volumes2[index];
        };

        for (size_t index = 0; index < points_count; ++index)
        {
            if (std::abs(pressures1[index] - pressures2[index]) > pressure_tolerance)
            {
                return std::nullopt;
            }

            const double current_difference = difference(index);
            if (current_difference == 0.0)
            {
                return Intersection{ pressures1[index], volumes1[index] };
            }

            if (index == 0)
            {
                continue;
            }

            const double previous_difference = difference(index - 1);
            if ((previous_difference < 0.0 && current_difference > 0.0) ||
                (previous_difference > 0.0 && current_difference < 0.0))
            {
                const double interpolation = previous_difference / (previous_difference - current_difference);
                const double pressure = pressures1[index - 1] + interpolation * (pressures1[index] - pressures1[index - 1]);
                const double volume = volumes1[index - 1] + interpolation * (volumes1[index] - volumes1[index - 1]);
                return Intersection{ pressure, volume };
            }
        }

        return std::nullopt;
    }

    std::string intersectionLabel(double pressure)
    {
        std::ostringstream label;
        label.precision(4);
        label << "P = " << std::fixed << pressure << " bar";
        return label.str();
    }

    std::string scaleSuffix(PlotScale scale)
    {
        return scale == PlotScale::Logarithmic ? "_log" : "_linear";
    }

    std::optional<KValuesIntersection> findKValuesIntersection(
        const std::vector<double>& pressures,
        const std::unordered_map<std::string, std::vector<double>>& k_values)
    {
        if (pressures.empty() || k_values.empty())
        {
            return std::nullopt;
        }

        double best_relative_spread = std::numeric_limits<double>::infinity();
        std::optional<KValuesIntersection> best_intersection;

        for (size_t index = 0; index < pressures.size(); ++index)
        {
            double min_k = std::numeric_limits<double>::infinity();
            double max_k = -std::numeric_limits<double>::infinity();
            double sum_k = 0.0;
            size_t values_count = 0;

            for (const auto& [component, values] : k_values)
            {
                (void)component;
                if (index >= values.size() || !std::isfinite(values[index]) || values[index] <= 0.0)
                {
                    values_count = 0;
                    break;
                }

                min_k = (std::min)(min_k, values[index]);
                max_k = (std::max)(max_k, values[index]);
                sum_k += values[index];
                ++values_count;
            }

            if (values_count != k_values.size())
            {
                continue;
            }

            const double mean_k = sum_k / static_cast<double>(values_count);
            const double relative_spread = (max_k - min_k) / mean_k;
            if (relative_spread < best_relative_spread)
            {
                best_relative_spread = relative_spread;
                best_intersection = KValuesIntersection{ pressures[index], mean_k };
            }
        }

        return best_intersection;
    }

    std::string yLabelFor(SolType type)
    {
        switch (type)
        {
        case SolType::GAS:
            return "W_{G}, liter";
        case SolType::LIQUID:
            return "W_{L}, liter";
        default:
            return "V, liter";
        }
    }

    void addEndLabel(PlotSpec& spec, const std::string& label, const std::vector<double>& x, const std::vector<double>& y)
    {
        if (!x.empty() && !y.empty())
        {
            const double offset_x = label == "1" ? view::plot_config::first_end_label_offset_x : view::plot_config::other_end_label_offset_x;
            spec.labels.push_back({ label, x.back(), y.back(), offset_x, view::plot_config::default_label_offset_y });
        }
    }

    double dataRange(const PlotSpec& spec, bool x_axis)
    {
        bool initialized = false;
        double min_value = 0.0;
        double max_value = 0.0;

        for (const auto& series : spec.series)
        {
            const auto& values = x_axis ? series.x : series.y;
            for (const auto value : values)
            {
                if (!initialized)
                {
                    min_value = value;
                    max_value = value;
                    initialized = true;
                }
                else
                {
                    min_value = (std::min)(min_value, value);
                    max_value = (std::max)(max_value, value);
                }
            }
        }

        const double range = max_value - min_value;
        return initialized && range > 0.0 ? range : 1.0;
    }

    void addMiddleLabel(PlotSpec& spec, const std::string& label, const std::vector<double>& x, const std::vector<double>& y)
    {
        const size_t points_count = (std::min)(x.size(), y.size());
        if (points_count > 0)
        {
            const size_t label_index = points_count / 2;
            const double label_x = x[label_index];
            const double label_y = y[label_index];
            const double x_range = dataRange(spec, true);
            const double y_range = dataRange(spec, false);
            double offset_y = view::plot_config::middle_label_offset_y;

            for (const auto& existing_label : spec.labels)
            {
                const double normalized_x_distance = std::abs(label_x - existing_label.x) / x_range;
                const double normalized_y_distance = std::abs(label_y - existing_label.y) / y_range;
                if (normalized_x_distance < view::plot_config::label_overlap_x_threshold &&
                    normalized_y_distance < view::plot_config::label_overlap_y_threshold)
                {
                    offset_y = (std::max)(offset_y, existing_label.offset_y + view::plot_config::label_overlap_offset_step_y);
                }
            }

            spec.labels.push_back({ label, label_x, label_y, view::plot_config::middle_label_offset_x, offset_y });
        }
    }
}


Plotter::Plotter(const Controller& controller)
	: controller_(std::make_unique<Controller>(controller))
{
}

void Plotter::plotComparativeMolarVolumes(double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
{
    auto data = controller_->getData();
    auto gas_name = data.names[indecies.first];
    auto liquid_name = data.names[indecies.second];
    auto gas_state = data.states.at(gas_name);
    auto liquid_state = data.states.at(liquid_name);

    controller_->setConcentrationState(indecies.first);
    auto gas_calculation = controller_->calculateVolumeInRange(Controller::generateJsonName(gas_state, RequestType::EXACT_MOLAR), pressure_start, pressure_end, pressure_inc);

    controller_->setConcentrationState(indecies.second);
    auto liquid_calculation = controller_->calculateVolumeInRange(Controller::generateJsonName(liquid_state, RequestType::EXACT_MOLAR), pressure_start, pressure_end, pressure_inc);

    auto approximation = controller_->approximateVolumeInRange(Controller::generateJsonName(gas_state, RequestType::APPROXIMATION_MOLAR), pressure_start, pressure_end, pressure_inc, fixed_pressures, indecies);

    plotTwoVectors(generatePNGName("molar_gas", gas_state.temperature), gas_calculation.pressures, gas_calculation.volumes, approximation.gas.pressures, approximation.gas.volumes, SolType::GAS);
    plotTwoVectors(generatePNGName("molar_liquid", liquid_state.temperature), liquid_calculation.pressures, liquid_calculation.volumes, approximation.liquid.pressures, approximation.liquid.volumes, SolType::LIQUID);
}

void Plotter::plotComparativeSpecificVolumes(double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
{
    auto data = controller_->getData();
    auto gas_name = data.names[indecies.first];
    auto liquid_name = data.names[indecies.second];
    auto gas_state = data.states.at(gas_name);
    auto liquid_state = data.states.at(liquid_name);

    controller_->setConcentrationState(indecies.first);
    auto gas_calculation = controller_->calculateSpecificVolumeInRange(Controller::generateJsonName(gas_state, RequestType::EXACT_SPECIFIC), pressure_start, pressure_end, pressure_inc);

    controller_->setConcentrationState(indecies.second);
    auto liquid_calculation = controller_->calculateSpecificVolumeInRange(Controller::generateJsonName(liquid_state, RequestType::EXACT_SPECIFIC), pressure_start, pressure_end, pressure_inc);

    auto approximation = controller_->approximateSpecificVolumeInRange(Controller::generateJsonName(gas_state, RequestType::APPROXIMATION_SPECIFIC), pressure_start, pressure_end, pressure_inc, fixed_pressures, indecies);

    plotTwoVectors(generatePNGName("specific_gas", gas_state.temperature), gas_calculation.pressures, gas_calculation.volumes, approximation.gas.pressures, approximation.gas.volumes, SolType::GAS);
    plotTwoVectors(generatePNGName("specific_liquid", liquid_state.temperature), liquid_calculation.pressures, liquid_calculation.volumes, approximation.liquid.pressures, approximation.liquid.volumes, SolType::LIQUID);
}

void Plotter::plotMolarVolumeIntersection(double pressure_start, double pressure_end, double pressure_inc, std::pair<size_t, size_t> indecies, PlotScale scale)
{
    auto data = controller_->getData();
    auto gas_name = data.names[indecies.first];
    auto liquid_name = data.names[indecies.second];
    auto gas_state = data.states.at(gas_name);
    auto liquid_state = data.states.at(liquid_name);

    controller_->setConcentrationState(indecies.first);
    auto gas_calculation = controller_->calculateVolumeInRange(Controller::generateJsonName(gas_state, RequestType::EXACT_MOLAR), pressure_start, pressure_end, pressure_inc);

    controller_->setConcentrationState(indecies.second);
    auto liquid_calculation = controller_->calculateVolumeInRange(Controller::generateJsonName(liquid_state, RequestType::EXACT_MOLAR), pressure_start, pressure_end, pressure_inc);

    plotVolumeIntersection(generatePNGName("molar_intersection" + scaleSuffix(scale), gas_state.temperature), gas_calculation.pressures, gas_calculation.volumes, liquid_calculation.pressures, liquid_calculation.volumes, SolType::DEFAULT, scale);
}

void Plotter::plotSpecificVolumeIntersection(double pressure_start, double pressure_end, double pressure_inc, std::pair<size_t, size_t> indecies, PlotScale scale)
{
    auto data = controller_->getData();
    auto gas_name = data.names[indecies.first];
    auto liquid_name = data.names[indecies.second];
    auto gas_state = data.states.at(gas_name);
    auto liquid_state = data.states.at(liquid_name);

    controller_->setConcentrationState(indecies.first);
    auto gas_calculation = controller_->calculateSpecificVolumeInRange(Controller::generateJsonName(gas_state, RequestType::EXACT_SPECIFIC), pressure_start, pressure_end, pressure_inc);

    controller_->setConcentrationState(indecies.second);
    auto liquid_calculation = controller_->calculateSpecificVolumeInRange(Controller::generateJsonName(liquid_state, RequestType::EXACT_SPECIFIC), pressure_start, pressure_end, pressure_inc);

    plotVolumeIntersection(generatePNGName("specific_intersection" + scaleSuffix(scale), gas_state.temperature), gas_calculation.pressures, gas_calculation.volumes, liquid_calculation.pressures, liquid_calculation.volumes, SolType::DEFAULT, scale);
}

void Plotter::plotComparativeKValues(double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
{
    auto data = controller_->getData();
    auto gas_name = data.names[indecies.first];
    auto gas_state = data.states.at(gas_name);

    controller_->setConcentrationState(indecies.first);
    auto def_kvalues = controller_->calculateKValuesByDefinitionInRange(Controller::generateJsonName(gas_state, RequestType::EXACT_KVALUE), pressure_start, pressure_end, pressure_inc, indecies);
    auto wilson_kvalues = controller_->calculateKValuesByWilsonInRange(Controller::generateJsonName(gas_state, RequestType::WILSON_KVALUE), pressure_start, pressure_end, pressure_inc);
    auto app_kvalues = controller_->approximateKValuesInRange(Controller::generateJsonName(gas_state, RequestType::APPROXIMATION_KVALUE), pressure_start, pressure_end, pressure_inc, fixed_pressures, indecies);

    for (const auto& component : controller_->getData().components)
    {
        plotThreeVectors(generatePNGName(component.name, gas_state.temperature), generatePNGName(component.name, gas_state.temperature), def_kvalues.pressures, def_kvalues.kvalues.at(component.name), wilson_kvalues.pressures, wilson_kvalues.kvalues.at(component.name), app_kvalues.pressures, app_kvalues.kvalues.at(component.name));
    }
}

void Plotter::plotDefinitionKValuesIntersection(double pressure_start, double pressure_end, double pressure_inc, std::pair<size_t, size_t> indecies)
{
    const auto data = controller_->getData();
    const auto& gas_name = data.names[indecies.first];
    const auto gas_state = data.states.at(gas_name);

    controller_->setConcentrationState(indecies.first);
    const auto k_values = controller_->calculateKValuesByDefinitionInRange(
        Controller::generateJsonName(gas_state, RequestType::EXACT_KVALUE),
        pressure_start,
        pressure_end,
        pressure_inc,
        indecies
    );

    plotAllVectors(
        generatePNGName("kvalues_by_definition_intersection", gas_state.temperature),
        k_values.pressures,
        k_values.kvalues
    );
}

void Plotter::plotTwoVectors(const std::string& filename,  const std::vector<double>& x1, const std::vector<double>& y1, const std::vector<double>& x2, const std::vector<double>& y2, SolType type)
{
    PlotSpec spec;
    spec.output_filename = filename;
    spec.x_label = "P, bar";
    spec.y_label = yLabelFor(type);
    spec.show_legend = false;
    spec.series.push_back(view::exactSeries(x1, y1));
    spec.series.push_back(view::approximationSeries(x2, y2));
    addMiddleLabel(spec, "1", x1, y1);
    addMiddleLabel(spec, "2", x2, y2);
    renderer_.render(spec);
}

void Plotter::plotVolumeIntersection(const std::string& filename, const std::vector<double>& x1, const std::vector<double>& y1, const std::vector<double>& x2, const std::vector<double>& y2, SolType type, PlotScale scale)
{
    PlotSpec spec;
    spec.output_filename = filename;
    spec.x_label = "P, bar";
    spec.y_label = yLabelFor(type);
    spec.log_x = scale == PlotScale::Logarithmic;
    spec.log_y = scale == PlotScale::Logarithmic;
    spec.show_legend = false;
    spec.series.push_back(view::exactSeries(x1, y1));
    spec.series.push_back(view::exactSeries(x2, y2));
    addMiddleLabel(spec, "1", x1, y1);
    addMiddleLabel(spec, "2", x2, y2);

    if (const auto intersection = findIntersection(x1, y1, x2, y2))
    {
        spec.points.push_back({ intersection->pressure, intersection->volume });
        spec.labels.push_back({
            intersectionLabel(intersection->pressure),
            intersection->pressure,
            intersection->volume,
            view::plot_config::intersection_label_offset_x,
            view::plot_config::intersection_label_offset_y
        });
    }

    renderer_.render(spec);
}

void Plotter::plotThreeVectors(const std::string& title, const std::string& filename, const std::vector<double>& x1, const std::vector<double>& y1, const std::vector<double>& x2, const std::vector<double>& y2, const std::vector<double>& x3, const std::vector<double>& y3)
{
    PlotSpec spec;
    spec.title = title;
    spec.output_filename = filename;
    spec.x_label = "P, bar";
    spec.y_label = "KValue";
    spec.base_font_size = view::plot_config::k_value_base_font_size;
    spec.show_legend = false;
    spec.show_title = false;
    spec.series.push_back(view::exactSeries(x1, y1));
    spec.series.push_back(view::wilsonSeries(x2, y2));
    spec.series.push_back(view::approximationSeries(x3, y3));
    addMiddleLabel(spec, "1", x1, y1);
    addMiddleLabel(spec, "2", x2, y2);
    addMiddleLabel(spec, "3", x3, y3);
    renderer_.render(spec);
}

void Plotter::plotAllVectors(const std::string& title, const std::vector<double>& pressures, const std::unordered_map<std::string, std::vector<double>>& k_values)
{
    std::string filename = title;
    if (!filename.ends_with(".png")) {
        filename += ".png";
    }

    PlotSpec spec;
    spec.title = title;
    spec.output_filename = filename;
    spec.x_label = "P";
    spec.y_label = "KValue";
    spec.log_x = true;
    spec.log_y = true;
    spec.show_legend = false;

    for (const auto& kvalue : k_values)
    {
        if (pressures.size() != kvalue.second.size()) {
            std::cerr << "Ошибка: размеры векторов не совпадают для " << kvalue.first
                << " (" << pressures.size() << " vs " << kvalue.second.size() << ")\n";
            continue;
        }

        spec.series.push_back(view::componentSeries(kvalue.first, pressures, kvalue.second));
    }

    if (const auto intersection = findKValuesIntersection(pressures, k_values))
    {
        spec.points.push_back({ intersection->pressure, intersection->k_value });
        spec.labels.push_back({
            intersectionLabel(intersection->pressure),
            intersection->pressure,
            intersection->k_value,
            view::plot_config::intersection_label_offset_x,
            view::plot_config::intersection_label_offset_y
        });
    }

    renderer_.render(spec);
}

std::string Plotter::generatePNGName(const std::string& name, double temperature)
{
    return PlotFileNameFormatter::pngName(name, temperature);
}
