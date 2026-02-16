#pragma once

#include "solution.h"
#include "json_parser.h"

#include <array>

enum class RequestType
{
    EXACT_MOLAR,
    EXACT_SPECIFIC,
    EXACT_KVALUE,

    WILSON_KVALUE,

    APPROXIMATION_MOLAR,
    APPROXIMATION_SPECIFIC,
    APPROXIMATION_KVALUE
};

struct CalculationResult
{
    std::vector<double> pressures;
    std::vector<double> volumes;
};

struct ApproximationResult
{
    CalculationResult gas;
    CalculationResult liquid;
};

struct KValuesResult
{
    std::vector<double> pressures;
    std::unordered_map<std::string, std::vector<double>> kvalues;
};

class Controller
{
public:
    Controller(const std::string json_filename,
               Correlation init_correlation = Correlation::GAO,
               EOSType init_eos_type = EOSType::PENG_ROBINSON,
               sol::Phase init_phase= sol::Phase::GAS,
               sol::State init_state = default_init_state);

    Controller(const Controller& other);
    Controller(Controller&& other) noexcept;

    //debug function
    //remove in future
    ParsedData                      getData() const;

    void                            setConcentrationState(size_t index);

    void                            setBip(Correlation correlation);
    void                            setEOS(EOSType eos);
    void                            setPressure(double pressure);
    void                            setPressureDimension(sol::PressureDimension new_dimension);
    void                            setVolumeDimension(sol::VolumeDimension new_dimension);
    void                            setSpecificVolumeDimension(sol::SpecificVolumeDimension new_dimension);
    void                            setMolarMassDimension(sol::MolarMassDimension new_dimension);

    CalculationResult               calculateVolumeInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc);
    CalculationResult               calculateSpecificVolumeInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc);
    KValuesResult                   calculateKValuesByDefinitionInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::pair<size_t, size_t> indecies);
    KValuesResult                   calculateKValuesByWilsonInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc);

    ApproximationResult             approximateVolumeInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies);
    ApproximationResult             approximateSpecificVolumeInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies);
    KValuesResult                   approximateKValuesInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies);

    static std::string              generateJsonName(const ConcentrationState& c_state, RequestType type);

private:
    void                            setConcentrationStateInternal(sol::Solution& solution, size_t index) const;

    size_t                          current_index_ = 0;
    ParsedData                      data_;
    std::unique_ptr<sol::Solution>  current_solution_;

    inline static const sol::State default_init_state
    {
        0.0,
        0.0,
        sol::PressureDimension::PA,
        sol::VolumeDimension::M3,
        sol::SpecificVolumeDimension::M3_PER_KG
    };
};
