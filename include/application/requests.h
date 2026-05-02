#pragma once

#include <array>
#include <cstddef>

struct PressureRange
{
    double start;
    double end;
    double step;
};

struct PhasePairSelection
{
    size_t gas_index;
    size_t liquid_index;
};

struct ApproximationRequest
{
    PressureRange range;
    std::array<double, 3> fixed_pressures;
    PhasePairSelection phases;
};
