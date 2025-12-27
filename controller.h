#pragma once

#include "solution.h"
#include "json_parser.h"

#include <array>

class Controller
{
public:
    Controller(const std::string json_filename,
               Correlation init_correlation = Correlation::GAO,
               EOSType init_eos_type = EOSType::PENG_ROBINSON,
               sol::Phase init_phase= sol::Phase::GAS,
               sol::State init_state = default_init_state);

    void setConcentrationState(size_t index);

    void setBip(Correlation correlation);
    void setEOS(EOSType eos);
    void setPressure(double pressure);
    void setPressureDimension(sol::PressureDimension new_dimension);
    void setVolumeDimension(sol::VolumeDimension new_dimension);
    void setSpecificVolumeDimension(sol::SpecificVolumeDimension new_dimension);
    void setMolarMassDimension(sol::MolarMassDimension new_dimension);

    void calculateVolumeInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc);
    void calculateSpecificVolumeInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc);
    void calculateKValuesByDefinitionInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::pair<size_t, size_t> indecies);

    void approximateVolumeInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies);
    void approximateSpecificVolumeInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies);
    void approximateKValuesInRange(const std::string& json_output_filename, double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies);

private:
    void setConcentrationState(sol::Solution& solution, size_t index) const;

    ParsedData data_;
    std::unique_ptr<sol::Solution> current_solution_;

    inline static const sol::State default_init_state
    {
        0.0,
        0.0,
        sol::PressureDimension::PA,
        sol::VolumeDimension::M3,
        sol::SpecificVolumeDimension::M3_PER_KG
    };
};
