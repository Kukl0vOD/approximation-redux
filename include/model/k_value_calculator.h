#pragma once

#include "solution_properties.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace sol
{
	std::unordered_map<std::string, double> calculateKValuesByDefinition(
		const std::unordered_map<std::string, double>& gas_concentrations,
		const std::unordered_map<std::string, double>& liquid_concentrations
	);

	std::unordered_map<std::string, double> calculateKValuesByWilson(
		const std::vector<Component>& components,
		const State& state
	);
}
