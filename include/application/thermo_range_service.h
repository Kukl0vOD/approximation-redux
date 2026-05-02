#pragma once

#include "application/requests.h"
#include "model/results.h"
#include "solution.h"

class ThermoRangeService
{
public:
    static CalculationResult calculateMolarVolumeRange(
        sol::Solution& solution,
        PressureRange range
    );

    static CalculationResult calculateSpecificVolumeRange(
        sol::Solution& solution,
        PressureRange range
    );
};
