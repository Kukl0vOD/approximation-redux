#include "application/phase_pair_factory.h"

#include <stdexcept>

void PhasePairFactory::applyConcentrationState(
    sol::Solution& solution,
    const ParsedData& data,
    size_t index
)
{
    auto current_name = data.names.at(index);
    auto current_state = data.states.at(current_name);
    auto current_callback = data.callbacks.at(current_name);
    auto solution_state = solution.getState();

    solution_state.temperature = current_state.temperature;
    solution.setPhase(current_state.phase);
    solution.setCallback(current_callback);
    solution.setState(solution_state);
}

PhasePair PhasePairFactory::create(
    const sol::Solution& prototype,
    const ParsedData& data,
    PhasePairSelection selection
)
{
    sol::Solution gas_solution(prototype);
    sol::Solution liquid_solution(prototype);

    applyConcentrationState(gas_solution, data, selection.gas_index);
    applyConcentrationState(liquid_solution, data, selection.liquid_index);

    if (gas_solution.getState().temperature != liquid_solution.getState().temperature
        || gas_solution.getPhase() == liquid_solution.getPhase())
    {
        throw std::logic_error("Different temperatures or same phases");
    }

    return
    {
        gas_solution,
        liquid_solution
    };
}
