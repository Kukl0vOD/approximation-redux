#pragma once

#include "application/requests.h"
#include "model/results.h"
#include "solution.h"

class KValueRangeService
{
public:
    static KValuesResult calculateByDefinition(
        sol::Solution& gas_solution,
        sol::Solution& liquid_solution,
        const std::vector<sol::Component>& components,
        PressureRange range
    );

    static KValuesResult calculateByWilson(
        sol::Solution& solution,
        PressureRange range
    );
};
