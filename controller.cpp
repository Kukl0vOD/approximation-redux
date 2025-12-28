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

void Controller::calculateVolumeInRange(const std::string &json_output_filename, double pressure_start, double pressure_end, double pressure_inc)
{
    std::ofstream json_out(json_output_filename);
    json::Array pressures;
    json::Array volumes;

    for(double current_pressure=pressure_start; current_pressure<pressure_end;current_pressure+=pressure_inc)
    {
        setPressure(current_pressure);
        pressures.push_back(current_pressure);
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
}

void Controller::calculateSpecificVolumeInRange(const std::string &json_output_filename, double pressure_start, double pressure_end, double pressure_inc)
{
    std::ofstream json_out(json_output_filename);
    json::Array pressures;
    json::Array volumes;

    for(double current_pressure=pressure_start; current_pressure<pressure_end;current_pressure+=pressure_inc)
    {
        setPressure(current_pressure);
        pressures.push_back(current_pressure);
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
}

void Controller::calculateKValuesByDefinitionInRange(const std::string &json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::pair<size_t, size_t> indecies)
{
    std::ofstream json_out(json_output_filename);

    sol::Solution gas_solution(*current_solution_);
    sol::Solution liquid_solution(*current_solution_);

    setConcentrationStateInternal(gas_solution, indecies.first);
    setConcentrationStateInternal(liquid_solution, indecies.second);

    if(gas_solution.getState().temperature!=liquid_solution.getState().temperature
        || gas_solution.getPhase()==liquid_solution.getPhase())
    {
        throw std::logic_error("Different temperatures or same phases");
    }

    json::Array pressures;
    std::unordered_map<std::string, std::vector<double>> kvalues;
    
    for(double current_pressure=pressure_start; current_pressure<pressure_end;current_pressure+=pressure_inc)
    {
        gas_solution.setPressure(current_pressure);
        liquid_solution.setPressure(current_pressure);
        pressures.push_back(current_pressure);

        auto kvalue = sol::calculateKValue(gas_solution, liquid_solution);
        for (const auto& component : current_solution_->getComponents())
        {
            kvalues[component.name].push_back(kvalue.at(component.name));
        }
    }

    auto builded_part = json::Builder{}
        .StartDict()
            .Key("P")
                .StartArray()
                    .Value(pressures)
                .EndArray();

    for (const auto& component : current_solution_->getComponents())
    {
        json::Array json_kvalue;
        for (const auto& kvalue : kvalues.at(component.name))
        {
            json_kvalue.push_back(kvalue);
        }

        builded_part = builded_part
            .Key(component.name)
                .StartArray()
                    .Value(json_kvalue)
                .EndArray();
    }

    auto output_doc = json::Document(builded_part.EndDict().Build());

    json::Print(output_doc,json_out);
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

void Controller::approximateKValuesInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
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
    std::unordered_map<std::string, std::vector<double>> kvalues;

    approx::Approximator approximator(gas_solution, liquid_solution, fixed_pressures[0], fixed_pressures[1], fixed_pressures[2]);

    for (double current_pressure = pressure_start; current_pressure < pressure_end; current_pressure += pressure_inc)
    {
        pressures.push_back(current_pressure);
        auto kvalue = approximator.approximateKValue(current_pressure, fixed_pressures[0]);

        for (const auto& component : current_solution_->getComponents())
        {
            kvalues[component.name].push_back(kvalue.at(component.name));
        }
    }

    auto builded_part = json::Builder{}
        .StartDict()
            .Key("P")
                .StartArray()
                    .Value(pressures)
                .EndArray();

    for (const auto& component : current_solution_->getComponents())
    {
        json::Array json_kvalue;
        for (const auto& kvalue : kvalues.at(component.name))
        {
            json_kvalue.push_back(kvalue);
        }

        builded_part = builded_part
            .Key(component.name)
                .StartArray()
                    .Value(json_kvalue)
                .EndArray();
    }

    auto output_doc = json::Document(builded_part.EndDict().Build());

    json::Print(output_doc, json_out);
}

void Controller::approximateSpecificVolumeInRange(const std::string &json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
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
    json::Array gas_volumes;
    json::Array liquid_volumes;

    approx::Approximator approximator(gas_solution, liquid_solution, fixed_pressures[0], fixed_pressures[1], fixed_pressures[2], sol::VolumeType::SPECIFIC);

    for (double current_pressure = pressure_start; current_pressure < pressure_end; current_pressure += pressure_inc)
    {
        auto gas_volume = approximator.approximateGasVolume(current_pressure);
        auto liquid_volume = approximator.approximateLiquidVolume(current_pressure);

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
}

void Controller::approximateVolumeInRange(const std::string &json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
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
    json::Array gas_volumes;
    json::Array liquid_volumes;

    approx::Approximator approximator(gas_solution, liquid_solution, fixed_pressures[0], fixed_pressures[1], fixed_pressures[2]);

    for (double current_pressure = pressure_start; current_pressure < pressure_end; current_pressure += pressure_inc)
    {
        auto gas_volume = approximator.approximateGasVolume(current_pressure);
        auto liquid_volume = approximator.approximateLiquidVolume(current_pressure);

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
}
