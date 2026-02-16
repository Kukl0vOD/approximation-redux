#include "controller.h"
#include "json_builder.h"
#include "approximation.h"

#include <sstream>

Controller::Controller(const std::string json_filename, Correlation correlation, EOSType eos_type, sol::Phase phase, sol::State init_state)
{
    std::ifstream json_in(json_filename);
    if(!json_in.is_open())
    {
        throw std::runtime_error("Can't open data file");
    }

    json::Parser parser(json_in);
    data_  = parser.parseAll();
    current_solution_ = std::make_unique<sol::Solution>(
        data_.components,
        data_.callbacks.at(*data_.names.begin()),
        correlation,
        eos_type,
        init_state,
        phase
        );

    setConcentrationState(0);
}

Controller::Controller(const Controller& other)
    : current_index_(other.current_index_)
    , data_(other.data_)
{
    if (other.current_solution_)
    {
        current_solution_ = std::make_unique<sol::Solution>(*other.current_solution_);
    }
}

Controller::Controller(Controller&& other) noexcept
    : current_index_(std::move(other.current_index_))
    , data_(std::move(other.data_))
    , current_solution_(std::move(other.current_solution_))
{
}

ParsedData Controller::getData() const
{
    return data_;
}

void Controller::setConcentrationState(size_t index)
{
    auto current_name = data_.names[index];
    auto current_state = data_.states.at(current_name);
    auto current_callback = data_.callbacks.at(current_name);
    auto solution_state = current_solution_->getState();

    solution_state.temperature=current_state.temperature;
    current_solution_->setPhase(current_state.phase);
    current_solution_->setCallback(current_callback);
    current_solution_->setState(solution_state);

    current_index_ = index;
}

void Controller::setBip(Correlation correlation)
{
    current_solution_->setBip(correlation);
}

void Controller::setEOS(EOSType eos)
{
    current_solution_->setEOS(eos::EOSFactory::createEOS(eos));
}

void Controller::setPressure(double pressure)
{
    current_solution_->setPressure(pressure);
}

void Controller::setPressureDimension(sol::PressureDimension new_dimension)
{
    current_solution_->setPressureDimension(new_dimension);
}

void Controller::setVolumeDimension(sol::VolumeDimension new_dimension)
{
    current_solution_->setVolumeDimension(new_dimension);
}

void Controller::setSpecificVolumeDimension(sol::SpecificVolumeDimension new_dimension)
{
    current_solution_->setSpecificVolumeDimension(new_dimension);
}

void Controller::setMolarMassDimension(sol::MolarMassDimension new_dimension)
{
    current_solution_->setMolarMassDimension(new_dimension);
}

CalculationResult Controller::calculateVolumeInRange(const std::string &json_output_filename, double pressure_start, double pressure_end, double pressure_inc)
{
    std::ofstream json_out(json_output_filename);

    std::vector<double> rtn_pressures;
    std::vector<double> rtn_volumes;

    json::Array pressures;
    json::Array volumes;

    for(double current_pressure=pressure_start; current_pressure<pressure_end;current_pressure+=pressure_inc)
    {
        setPressure(current_pressure);
        rtn_pressures.push_back(current_pressure);
        pressures.push_back(current_pressure);
        rtn_volumes.push_back(current_solution_->calculateVolume());
        volumes.push_back(current_solution_->calculateVolume());
    }

    auto output_doc = json::Document(json::Builder{}
            .StartDict()
                .Key("P")
                    .StartArray()
                        .Value(pressures)
                    .EndArray()
                .Key("V")
                    .StartArray()
                        .Value(volumes)
                    .EndArray()
            .EndDict().Build());

    json::Print(output_doc,json_out);

    return
    {
        rtn_pressures,
        rtn_volumes
    };
}

CalculationResult Controller::calculateSpecificVolumeInRange(const std::string &json_output_filename, double pressure_start, double pressure_end, double pressure_inc)
{
    std::ofstream json_out(json_output_filename);

    std::vector<double> rtn_pressures;
    std::vector<double> rtn_volumes;

    json::Array pressures;
    json::Array volumes;

    for(double current_pressure=pressure_start; current_pressure<pressure_end;current_pressure+=pressure_inc)
    {
        setPressure(current_pressure);
        rtn_pressures.push_back(current_pressure);
        pressures.push_back(current_pressure);
        rtn_volumes.push_back(current_solution_->calculateSpecificVolume());
        volumes.push_back(current_solution_->calculateSpecificVolume());
    }

    auto output_doc = json::Document(json::Builder{}
            .StartDict()
                .Key("P")
                    .StartArray()
                        .Value(pressures)
                    .EndArray()
                .Key("SV")
                    .StartArray()
                        .Value(volumes)
                    .EndArray()
            .EndDict().Build());

    json::Print(output_doc,json_out);

    return
    {
        rtn_pressures,
        rtn_volumes
    };
}

KValuesResult Controller::calculateKValuesByDefinitionInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::pair<size_t, size_t> indecies)
{
    std::ofstream json_out(json_output_filename);

    sol::Solution gas_solution(*current_solution_);
    sol::Solution liquid_solution(*current_solution_);

    setConcentrationStateInternal(gas_solution, indecies.first);
    setConcentrationStateInternal(liquid_solution, indecies.second);

    if (gas_solution.getState().temperature != liquid_solution.getState().temperature
        || gas_solution.getPhase() == liquid_solution.getPhase())
    {
        throw std::logic_error("Different temperatures or same phases");
    }

    json::Array pressures;
    std::vector<double> rtn_pressures;
    std::unordered_map<std::string, std::vector<double>> kvalues;

    for (double current_pressure = pressure_start; current_pressure < pressure_end; current_pressure += pressure_inc)
    {
        gas_solution.setPressure(current_pressure);
        liquid_solution.setPressure(current_pressure);
        rtn_pressures.push_back(current_pressure);
        pressures.push_back(current_pressure);

        auto kvalue = sol::calculateKValue(gas_solution, liquid_solution);
        for (const auto& component : current_solution_->getComponents())
        {
            kvalues[component.name].push_back(kvalue.at(component.name));
        }
    }

    // Создаем Builder
    json::Builder builder;
    builder.StartDict()
        .Key("P")
        .StartArray();

    // Добавляем каждое значение давления отдельно
    for (const auto& pressure : pressures) {
        builder.Value(pressure);
    }

    builder.EndArray();

    // Добавляем K-значения для каждого компонента
    for (const auto& component : current_solution_->getComponents())
    {
        builder.Key(component.name).StartArray();
        for (const auto& kvalue : kvalues.at(component.name))
        {
            builder.Value(kvalue);
        }
        builder.EndArray();
    }

    auto output_doc = json::Document(builder.EndDict().Build());

    json::Print(output_doc, json_out);

    return
    {
        rtn_pressures,
        kvalues
    };
}

KValuesResult Controller::calculateKValuesByWilsonInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc)
{
    std::ofstream json_out(json_output_filename);

    json::Array pressures;
    std::vector<double> rtn_pressures;
    std::unordered_map<std::string, std::vector<double>> kvalues;

    for (double current_pressure = pressure_start; current_pressure < pressure_end; current_pressure+=pressure_inc)
    {
        setPressure(current_pressure);
        rtn_pressures.push_back(current_pressure);
        pressures.push_back(current_pressure);

        auto kvalue = sol::calcualteKValueByWilson(current_solution_->getComponents(), current_solution_->getState());

        for (const auto& component : current_solution_->getComponents())
        {
            kvalues[component.name].push_back(kvalue.at(component.name));
        }
    }

    // Создаем Builder
    json::Builder builder;
    builder.StartDict()
        .Key("P")
        .StartArray();

    // Добавляем каждое значение давления отдельно
    for (const auto& pressure : pressures) {
        builder.Value(pressure);
    }

    builder.EndArray();

    // Добавляем K-значения для каждого компонента
    for (const auto& component : current_solution_->getComponents())
    {
        builder.Key(component.name).StartArray();
        for (const auto& kvalue : kvalues.at(component.name))
        {
            builder.Value(kvalue);
        }
        builder.EndArray();
    }

    auto output_doc = json::Document(builder.EndDict().Build());

    json::Print(output_doc, json_out);

    return
    {
        rtn_pressures,
        kvalues
    };
}

void Controller::setConcentrationStateInternal(sol::Solution& solution, size_t index) const
{
    auto current_name = data_.names[index];
    auto current_state = data_.states.at(current_name);
    auto current_callback = data_.callbacks.at(current_name);
    auto solution_state = solution.getState();

    solution_state.temperature=current_state.temperature;
    solution.setPhase(current_state.phase);
    solution.setCallback(current_callback);
    solution.setState(solution_state);
}

std::string Controller::generateJsonName(const ConcentrationState& c_state, RequestType type)
{
    std::stringstream ss;

    switch (type)
    {
    case RequestType::EXACT_MOLAR:
        ss << "exact_molar_";
        break;
    case RequestType::EXACT_SPECIFIC:
        ss << "exact_specific_";
        break;
    case RequestType::EXACT_KVALUE:
        ss << "exact_kvalue_";
        break;
    case RequestType::WILSON_KVALUE:
        ss << "wilson_kvalue_";
        break;
    case RequestType::APPROXIMATION_MOLAR:
        ss << "app_molar_";
        break;
    case RequestType::APPROXIMATION_SPECIFIC:
        ss << "app_specific_";
        break;
    case RequestType::APPROXIMATION_KVALUE:
        ss << "app_kvalue_";
        break;
    default:
        break;

    }

    ss << c_state.temperature;

    if (type == RequestType::EXACT_MOLAR || type == RequestType::EXACT_SPECIFIC)
    {
        switch (c_state.phase)
        {
        case sol::Phase::GAS:
            ss << "_gas";
            break;
        case sol::Phase::LIQUID:
            ss << "_liquid";
            break;
        default:
            break;
        }
    }

    ss << ".json";

    return ss.str();
}

KValuesResult Controller::approximateKValuesInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
{
    std::ofstream json_out(json_output_filename);

    sol::Solution gas_solution(*current_solution_);
    sol::Solution liquid_solution(*current_solution_);

    setConcentrationStateInternal(gas_solution, indecies.first);
    setConcentrationStateInternal(liquid_solution, indecies.second);

    if (gas_solution.getState().temperature != liquid_solution.getState().temperature
        || gas_solution.getPhase() == liquid_solution.getPhase())
    {
        throw std::logic_error("Different temperatures or same phases");
    }

    json::Array pressures;
    std::vector<double> rtn_pressures;
    std::unordered_map<std::string, std::vector<double>> kvalues;

    approx::Approximator approximator(gas_solution, liquid_solution, fixed_pressures[0], fixed_pressures[1], fixed_pressures[2]);

    for (double current_pressure = pressure_start; current_pressure < pressure_end; current_pressure += pressure_inc)
    {
        pressures.push_back(current_pressure);
        rtn_pressures.push_back(current_pressure);
        auto kvalue = approximator.approximateKValue(current_pressure, fixed_pressures[0],current_solution_->getState().p_dim);

        for (const auto& component : current_solution_->getComponents())
        {
            kvalues[component.name].push_back(kvalue.at(component.name));
        }
    }

    json::Builder builder;
    builder.StartDict()
        .Key("P")
        .StartArray();

    for (const auto& pressure : pressures) {
        builder.Value(pressure);
    }

    builder.EndArray();

    for (const auto& component : current_solution_->getComponents())
    {
        builder.Key(component.name).StartArray();
        for (const auto& kvalue : kvalues.at(component.name))
        {
            builder.Value(kvalue);
        }
        builder.EndArray();
    }

    auto output_doc = json::Document(builder.EndDict().Build());

    json::Print(output_doc, json_out);

    return
    {
        rtn_pressures,
        kvalues
    };
}

ApproximationResult Controller::approximateSpecificVolumeInRange(const std::string &json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
{
    std::ofstream json_out(json_output_filename);

    sol::Solution gas_solution(*current_solution_);
    sol::Solution liquid_solution(*current_solution_);

    setConcentrationStateInternal(gas_solution, indecies.first);
    setConcentrationStateInternal(liquid_solution, indecies.second);

    if (gas_solution.getState().temperature != liquid_solution.getState().temperature
        || gas_solution.getPhase() == liquid_solution.getPhase())
    {
        throw std::logic_error("Different temperatures or same phases");
    }

    std::vector<double> rtn_pressures;
    std::vector<double> rtn_gas_volumes;
    std::vector<double> rtn_liquid_volumes;

    json::Array pressures;
    json::Array gas_volumes;
    json::Array liquid_volumes;

    approx::Approximator approximator(gas_solution, liquid_solution, fixed_pressures[0], fixed_pressures[1], fixed_pressures[2], sol::VolumeType::SPECIFIC);

    for (double current_pressure = pressure_start; current_pressure < pressure_end; current_pressure += pressure_inc)
    {
        auto gas_volume = approximator.approximateGasVolume(current_pressure, gas_solution.getState().p_dim, gas_solution.getState().v_dim);
        auto liquid_volume = approximator.approximateLiquidVolume(current_pressure, gas_solution.getState().p_dim, gas_solution.getState().v_dim);

        rtn_pressures.push_back(current_pressure);
        rtn_gas_volumes.push_back(gas_volume);
        rtn_liquid_volumes.push_back(liquid_volume);

        pressures.push_back(current_pressure);
        gas_volumes.push_back(gas_volume);
        liquid_volumes.push_back(liquid_volume);
    }

    auto output_doc = json::Document(json::Builder{}
        .StartDict()
            .Key("P")
                .StartArray()
                    .Value(pressures)
                .EndArray()
            .Key("SV_Gas")
                .StartArray()
                    .Value(gas_volumes)
                .EndArray()
            .Key("SV_Liquid")
                .StartArray()
                    .Value(liquid_volumes)
                .EndArray()
            .EndDict().Build());

    json::Print(output_doc, json_out);

    return
    {
        {
            rtn_pressures,
            rtn_gas_volumes
        },
        {
            rtn_pressures,
            rtn_liquid_volumes
        }
    };
}

ApproximationResult Controller::approximateVolumeInRange(const std::string &json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
{
    std::ofstream json_out(json_output_filename);

    sol::Solution gas_solution(*current_solution_);
    sol::Solution liquid_solution(*current_solution_);

    setConcentrationStateInternal(gas_solution, indecies.first);
    setConcentrationStateInternal(liquid_solution, indecies.second);

    if (gas_solution.getState().temperature != liquid_solution.getState().temperature
        || gas_solution.getPhase() == liquid_solution.getPhase())
    {
        throw std::logic_error("Different temperatures or same phases");
    }

    std::vector<double> rtn_pressures;
    std::vector<double> rtn_gas_volumes;
    std::vector<double> rtn_liquid_volumes;

    json::Array pressures;
    json::Array gas_volumes;
    json::Array liquid_volumes;

    approx::Approximator approximator(gas_solution, liquid_solution, fixed_pressures[0], fixed_pressures[1], fixed_pressures[2]);

    for (double current_pressure = pressure_start; current_pressure < pressure_end; current_pressure += pressure_inc)
    {
        auto gas_volume = approximator.approximateGasVolume(current_pressure, gas_solution.getState().p_dim, gas_solution.getState().v_dim);
        auto liquid_volume = approximator.approximateLiquidVolume(current_pressure, gas_solution.getState().p_dim, gas_solution.getState().v_dim);

        rtn_pressures.push_back(current_pressure);
        rtn_gas_volumes.push_back(gas_volume);
        rtn_liquid_volumes.push_back(liquid_volume);

        pressures.push_back(current_pressure);
        gas_volumes.push_back(gas_volume);
        liquid_volumes.push_back(liquid_volume);
    }

    auto output_doc = json::Document(json::Builder{}
        .StartDict()
            .Key("P")
                .StartArray()
                    .Value(pressures)
                .EndArray()
            .Key("V_Gas")
                .StartArray()
                    .Value(gas_volumes)
                .EndArray()
            .Key("V_Liquid")
                .StartArray()
                    .Value(liquid_volumes)
                .EndArray()
        .EndDict().Build());

    json::Print(output_doc, json_out);

    return
    {
        {
            rtn_pressures,
            rtn_gas_volumes
        },
        {
            rtn_pressures,
            rtn_liquid_volumes
        }
    };
}
