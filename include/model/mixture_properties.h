#pragma once

#include "solution_properties.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace sol
{
	double calculateAverageMolarMass(
		const std::vector<Component>& components,
		const std::unordered_map<std::string, double>& concentrations
	);
}
