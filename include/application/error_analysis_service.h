#pragma once

#include "model/results.h"

#include <vector>

struct ErrorAnalysisResult
{
    std::vector<double> pressures;
    std::vector<double> gas_molar_error;
    std::vector<double> liquid_molar_error;
    std::vector<double> gas_specific_error;
    std::vector<double> liquid_specific_error;
    double max_gas_molar_error = 0;
    double max_liquid_molar_error = 0;
    double max_gas_specific_error = 0;
    double max_liquid_specific_error = 0;
};

class ErrorAnalysisService
{
public:
    static ErrorAnalysisResult compareVolumeAndSpecificApproximation(
        const CalculationResult& gas_molar_exact,
        const CalculationResult& liquid_molar_exact,
        const ApproximationResult& molar_approximation,
        const CalculationResult& gas_specific_exact,
        const CalculationResult& liquid_specific_exact,
        const ApproximationResult& specific_approximation
    );
};
