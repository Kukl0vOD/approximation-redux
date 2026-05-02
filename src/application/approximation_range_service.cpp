#include "application/approximation_range_service.h"

VolumeApproximationOutput ApproximationRangeService::calculateMolarVolume(
    PhasePair phase_pair,
    PressureRange range,
    std::array<double, 3> fixed_pressures
)
{
    ApproximationResult result;
    approx::Approximator approximator(
        phase_pair.gas,
        phase_pair.liquid,
        fixed_pressures[0],
        fixed_pressures[1],
        fixed_pressures[2]
    );

    for (double current_pressure = range.start; current_pressure < range.end; current_pressure += range.step)
    {
        auto gas_volume = approximator.approximateGasVolume(current_pressure, phase_pair.gas.getState().p_dim, phase_pair.gas.getState().v_dim);
        auto liquid_volume = approximator.approximateLiquidVolume(current_pressure, phase_pair.gas.getState().p_dim, phase_pair.gas.getState().v_dim);

        result.gas.pressures.push_back(current_pressure);
        result.gas.volumes.push_back(gas_volume);
        result.liquid.pressures.push_back(current_pressure);
        result.liquid.volumes.push_back(liquid_volume);
    }

    return
    {
        result,
        approximator.getConstants()
    };
}

ApproximationResult ApproximationRangeService::calculateSpecificVolume(
    PhasePair phase_pair,
    PressureRange range,
    std::array<double, 3> fixed_pressures
)
{
    ApproximationResult result;
    approx::Approximator approximator(
        phase_pair.gas,
        phase_pair.liquid,
        fixed_pressures[0],
        fixed_pressures[1],
        fixed_pressures[2],
        sol::VolumeType::SPECIFIC
    );

    for (double current_pressure = range.start; current_pressure < range.end; current_pressure += range.step)
    {
        auto gas_volume = approximator.approximateGasVolume(current_pressure, phase_pair.gas.getState().p_dim, phase_pair.gas.getState().sv_dim);
        auto liquid_volume = approximator.approximateLiquidVolume(current_pressure, phase_pair.liquid.getState().p_dim, phase_pair.liquid.getState().sv_dim);

        result.gas.pressures.push_back(current_pressure);
        result.gas.volumes.push_back(gas_volume);
        result.liquid.pressures.push_back(current_pressure);
        result.liquid.volumes.push_back(liquid_volume);
    }

    return result;
}

KValuesResult ApproximationRangeService::calculateKValues(
    PhasePair phase_pair,
    const std::vector<sol::Component>& components,
    PressureRange range,
    std::array<double, 3> fixed_pressures
)
{
    KValuesResult result;
    approx::Approximator approximator(
        phase_pair.gas,
        phase_pair.liquid,
        fixed_pressures[0],
        fixed_pressures[1],
        fixed_pressures[2],
        sol::VolumeType::MOLAR
    );

    for (double current_pressure = range.start; current_pressure < range.end; current_pressure += range.step)
    {
        result.pressures.push_back(current_pressure);
        auto kvalue = approximator.approximateKValue(current_pressure, fixed_pressures[0], phase_pair.gas.getState().p_dim);

        for (const auto& component : components)
        {
            result.kvalues[component.name].push_back(kvalue.at(component.name));
        }
    }

    return result;
}
