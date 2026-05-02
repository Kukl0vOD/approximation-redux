#include "application/error_analysis_service.h"

#include <algorithm>
#include <cmath>

namespace
{
    double relativeError(double exact, double approximate)
    {
        return std::abs(exact - approximate) / exact * 100;
    }

    std::vector<double> calculateErrors(
        const std::vector<double>& exact,
        const std::vector<double>& approximate
    )
    {
        std::vector<double> errors;
        errors.reserve(exact.size());

        for (size_t i = 0; i < exact.size(); ++i)
        {
            errors.push_back(relativeError(exact[i], approximate[i]));
        }

        return errors;
    }

    double maxValue(const std::vector<double>& values)
    {
        return *std::max_element(values.begin(), values.end());
    }
}

ErrorAnalysisResult ErrorAnalysisService::compareVolumeAndSpecificApproximation(
    const CalculationResult& gas_molar_exact,
    const CalculationResult& liquid_molar_exact,
    const ApproximationResult& molar_approximation,
    const CalculationResult& gas_specific_exact,
    const CalculationResult& liquid_specific_exact,
    const ApproximationResult& specific_approximation
)
{
    ErrorAnalysisResult result;
    result.pressures = gas_molar_exact.pressures;
    result.gas_molar_error = calculateErrors(gas_molar_exact.volumes, molar_approximation.gas.volumes);
    result.liquid_molar_error = calculateErrors(liquid_molar_exact.volumes, molar_approximation.liquid.volumes);
    result.gas_specific_error = calculateErrors(gas_specific_exact.volumes, specific_approximation.gas.volumes);
    result.liquid_specific_error = calculateErrors(liquid_specific_exact.volumes, specific_approximation.liquid.volumes);

    result.max_gas_molar_error = maxValue(result.gas_molar_error);
    result.max_liquid_molar_error = maxValue(result.liquid_molar_error);
    result.max_gas_specific_error = maxValue(result.gas_specific_error);
    result.max_liquid_specific_error = maxValue(result.liquid_specific_error);

    return result;
}
