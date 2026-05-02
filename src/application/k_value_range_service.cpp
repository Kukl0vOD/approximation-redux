#include "application/k_value_range_service.h"

KValuesResult KValueRangeService::calculateByDefinition(
    sol::Solution& gas_solution,
    sol::Solution& liquid_solution,
    const std::vector<sol::Component>& components,
    PressureRange range
)
{
    KValuesResult result;

    for (double current_pressure = range.start; current_pressure < range.end; current_pressure += range.step)
    {
        gas_solution.setPressure(current_pressure);
        liquid_solution.setPressure(current_pressure);
        result.pressures.push_back(current_pressure);

        auto kvalue = sol::calculateKValue(gas_solution, liquid_solution);
        for (const auto& component : components)
        {
            result.kvalues[component.name].push_back(kvalue.at(component.name));
        }
    }

    return result;
}

KValuesResult KValueRangeService::calculateByWilson(
    sol::Solution& solution,
    PressureRange range
)
{
    KValuesResult result;

    for (double current_pressure = range.start; current_pressure < range.end; current_pressure += range.step)
    {
        solution.setPressure(current_pressure);
        result.pressures.push_back(current_pressure);

        auto kvalue = sol::calcualteKValueByWilson(solution.getComponents(), solution.getState());

        for (const auto& component : solution.getComponents())
        {
            result.kvalues[component.name].push_back(kvalue.at(component.name));
        }
    }

    return result;
}
