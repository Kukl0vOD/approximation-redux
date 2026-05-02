#include "controller.h"
#include "json_builder.h"
#include "approximation.h"
#include "application/approximation_range_service.h"
#include "application/error_analysis_service.h"
#include "application/json_result_writer.h"
#include "application/k_value_range_service.h"
#include "application/phase_pair_factory.h"
#include "application/thermo_range_service.h"

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
    PhasePairFactory::applyConcentrationState(*current_solution_, data_, index);
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
    auto result = ThermoRangeService::calculateMolarVolumeRange(*current_solution_, { pressure_start, pressure_end, pressure_inc });
    JsonResultWriter::writeCalculationResult(result, json_output_filename, "V");
    return result;
}

CalculationResult Controller::calculateSpecificVolumeInRange(const std::string &json_output_filename, double pressure_start, double pressure_end, double pressure_inc)
{
    auto result = ThermoRangeService::calculateSpecificVolumeRange(*current_solution_, { pressure_start, pressure_end, pressure_inc });
    JsonResultWriter::writeCalculationResult(result, json_output_filename, "SV");
    return result;
}

KValuesResult Controller::calculateKValuesByDefinitionInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::pair<size_t, size_t> indecies)
{
    auto pair = PhasePairFactory::create(*current_solution_, data_, { indecies.first, indecies.second });
    auto result = KValueRangeService::calculateByDefinition(pair.gas, pair.liquid, current_solution_->getComponents(), { pressure_start, pressure_end, pressure_inc });
    JsonResultWriter::writeKValuesResult(result, current_solution_->getComponents(), json_output_filename);
    return result;
}

KValuesResult Controller::calculateKValuesByWilsonInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc)
{
    auto result = KValueRangeService::calculateByWilson(*current_solution_, { pressure_start, pressure_end, pressure_inc });
    JsonResultWriter::writeKValuesResult(result, current_solution_->getComponents(), json_output_filename);
    return result;
}

void Controller::setConcentrationStateInternal(sol::Solution& solution, size_t index) const
{
    PhasePairFactory::applyConcentrationState(solution, data_, index);
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
    auto pair = PhasePairFactory::create(*current_solution_, data_, { indecies.first, indecies.second });
    auto result = ApproximationRangeService::calculateKValues(pair, current_solution_->getComponents(), { pressure_start, pressure_end, pressure_inc }, fixed_pressures);
    JsonResultWriter::writeKValuesResult(result, current_solution_->getComponents(), json_output_filename);
    return result;
}

void Controller::compareVolumeAndSpecificApprox(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
{
    auto data = getData();
    auto gas_name = data.names[indecies.first];
    auto liquid_name = data.names[indecies.second];
    auto gas_state = data.states.at(gas_name);
    auto liquid_state = data.states.at(liquid_name);

    setConcentrationState(indecies.first);
    auto gas_molar = calculateVolumeInRange(Controller::generateJsonName(gas_state, RequestType::EXACT_MOLAR), pressure_start, pressure_end, pressure_inc);

    setConcentrationState(indecies.second);
    auto liquid_molar = calculateVolumeInRange(Controller::generateJsonName(liquid_state, RequestType::EXACT_MOLAR), pressure_start, pressure_end, pressure_inc);

    auto molar_approximation = approximateVolumeInRange(Controller::generateJsonName(gas_state, RequestType::APPROXIMATION_MOLAR), pressure_start, pressure_end, pressure_inc, fixed_pressures, indecies);

    setConcentrationState(indecies.first);
    auto gas_specific = calculateSpecificVolumeInRange(Controller::generateJsonName(gas_state, RequestType::EXACT_MOLAR), pressure_start, pressure_end, pressure_inc);

    setConcentrationState(indecies.second);
    auto liquid_specific = calculateSpecificVolumeInRange(Controller::generateJsonName(liquid_state, RequestType::EXACT_MOLAR), pressure_start, pressure_end, pressure_inc);

    auto specific_approximation = approximateSpecificVolumeInRange(Controller::generateJsonName(gas_state, RequestType::APPROXIMATION_SPECIFIC), pressure_start, pressure_end, pressure_inc, fixed_pressures, indecies);

    auto result = ErrorAnalysisService::compareVolumeAndSpecificApproximation(
        gas_molar,
        liquid_molar,
        molar_approximation,
        gas_specific,
        liquid_specific,
        specific_approximation
    );

    JsonResultWriter::writeErrorAnalysisResult(result, json_output_filename);
}

ApproximationResult Controller::approximateSpecificVolumeInRange(const std::string &json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
{
    auto pair = PhasePairFactory::create(*current_solution_, data_, { indecies.first, indecies.second });
    auto result = ApproximationRangeService::calculateSpecificVolume(pair, { pressure_start, pressure_end, pressure_inc }, fixed_pressures);
    JsonResultWriter::writeSpecificApproximationResult(result, json_output_filename);
    return result;
}

ApproximationResult Controller::approximateVolumeInRange(const std::string &json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
{
    auto pair = PhasePairFactory::create(*current_solution_, data_, { indecies.first, indecies.second });
    auto output = ApproximationRangeService::calculateMolarVolume(pair, { pressure_start, pressure_end, pressure_inc }, fixed_pressures);
    JsonResultWriter::writeMolarApproximationResult(output.result, output.constants, json_output_filename);
    return output.result;
}
