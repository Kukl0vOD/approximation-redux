#include "application/thermo_range_service.h"

#include <cmath>

namespace
{
    CalculationResult calculateRange(
        sol::Solution& solution,
        PressureRange range,
        double epsilon,
        double (sol::Solution::*calculate)()
    )
    {
        CalculationResult result;

        for (double current_pressure = range.start; current_pressure < range.end; current_pressure += range.step)
        {
            solution.setPressure(current_pressure);
            double current_volume = (solution.*calculate)();

            if (result.pressures.empty())
            {
                result.pressures.push_back(current_pressure);
                result.volumes.push_back(current_volume);
                continue;
            }

            double prev_volume = result.volumes.back();
            double volume_diff = std::abs(current_volume - prev_volume);

            if (volume_diff < epsilon)
            {
                result.pressures.push_back(current_pressure);
                result.volumes.push_back(current_volume);
            }
        }

        return result;
    }
}

CalculationResult ThermoRangeService::calculateMolarVolumeRange(
    sol::Solution& solution,
    PressureRange range
)
{
    return calculateRange(solution, range, 1.0, &sol::Solution::calculateVolume);
}

CalculationResult ThermoRangeService::calculateSpecificVolumeRange(
    sol::Solution& solution,
    PressureRange range
)
{
    return calculateRange(solution, range, 0.01, &sol::Solution::calculateSpecificVolume);
}
