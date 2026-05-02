#pragma once

#include "eos.h"
#include "solution_properties.h"
#include "utilities.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace sol
{
	class ThermoCalculator
	{
	public:
		static double calculateMolarVolume(
			const eos::ICubicEOS& eos,
			const std::vector<Component>& components,
			const std::unordered_map<std::string, double>& concentrations,
			const Matrix<double>& bip,
			const State& state,
			Phase phase
		);

		static double calculateSpecificVolume(
			const eos::ICubicEOS& eos,
			const std::vector<Component>& components,
			const std::unordered_map<std::string, double>& concentrations,
			const Matrix<double>& bip,
			const State& state,
			Phase phase
		);
	};
}
