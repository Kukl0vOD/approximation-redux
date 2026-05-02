#pragma once

#include "application/phase_pair_factory.h"
#include "application/requests.h"
#include "approximation.h"
#include "model/results.h"

struct VolumeApproximationOutput
{
    ApproximationResult result;
    approx::Constants constants;
};

class ApproximationRangeService
{
public:
    static VolumeApproximationOutput calculateMolarVolume(
        PhasePair phase_pair,
        PressureRange range,
        std::array<double, 3> fixed_pressures
    );

    static ApproximationResult calculateSpecificVolume(
        PhasePair phase_pair,
        PressureRange range,
        std::array<double, 3> fixed_pressures
    );

    static KValuesResult calculateKValues(
        PhasePair phase_pair,
        const std::vector<sol::Component>& components,
        PressureRange range,
        std::array<double, 3> fixed_pressures
    );
};
