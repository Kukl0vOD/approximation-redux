#pragma once

#include "application/requests.h"
#include "json_parser.h"
#include "solution.h"

struct PhasePair
{
    sol::Solution gas;
    sol::Solution liquid;
};

class PhasePairFactory
{
public:
    static void applyConcentrationState(
        sol::Solution& solution,
        const ParsedData& data,
        size_t index
    );

    static PhasePair create(
        const sol::Solution& prototype,
        const ParsedData& data,
        PhasePairSelection selection
    );
};
