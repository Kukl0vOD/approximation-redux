#pragma once

#include "json.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace extractor
{
    class ConcentrationExtractor
    {
    public:
        ConcentrationExtractor(
            const std::filesystem::path& solution_json,
            const std::filesystem::path& molar_fractions_json,
            const std::filesystem::path& kvalues_json
        );

        void exportToJson(const std::filesystem::path& output_json) const;

    private:
        void readSolutionJson(const std::filesystem::path& solution_json);
        void readMolarFractionsJson(const std::filesystem::path& molar_fractions_json);
        void readKValuesJson(const std::filesystem::path& kvalues_json);
        void calculateConcentrations();

        std::string pressure_dimension_;
        double temperature_kelvin_ = 0.0;
        json::Array solution_;
        std::vector<std::string> component_names_;
        std::vector<double> full_concentrations_;
        std::vector<double> fraction_pressures_;
        std::vector<double> liquid_molar_fractions_;
        std::vector<double> gas_molar_fractions_;
        std::vector<double> pressures_;
        std::unordered_map<std::string, std::vector<double>> kvalues_;
        std::unordered_map<std::string, std::vector<double>> liquid_concentrations_;
        std::unordered_map<std::string, std::vector<double>> gas_concentrations_;
    };
}
