#include "model/k_value_calculator.h"

#include "utilities.h"

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace sol
{
	std::unordered_map<std::string, double> calculateKValuesByDefinition(
		const std::unordered_map<std::string, double>& gas_concentrations,
		const std::unordered_map<std::string, double>& liquid_concentrations
	)
	{
		assert(gas_concentrations.size() == liquid_concentrations.size());

		std::unordered_map<std::string, double> k_values(gas_concentrations.size());

		for (const auto& [name, g_concentration] : gas_concentrations)
		{
			auto l_concentration_it = liquid_concentrations.find(name);

			if (l_concentration_it == liquid_concentrations.end())
			{
				throw std::logic_error("different solutions");
			}

			k_values[name] = g_concentration / l_concentration_it->second;
		}

		return k_values;
	}

	std::unordered_map<std::string, double> calculateKValuesByWilson(
		const std::vector<Component>& components,
		const State& state
	)
	{
		std::unordered_map<std::string, double> kvalues;

		for (const auto& component : components)
		{
			auto pc = utilities::UnitConverter::convert(component.critical_pressure, component.p_dim, PressureDimension::PA);
			auto tc = component.critical_temperature;
			auto w = component.accentric_factor;
			auto p = utilities::UnitConverter::convert(state.pressure, state.p_dim, PressureDimension::PA);
			auto t = state.temperature;
			auto kvalue = pc / p * exp(5.31 * (1 + w) * (1 - tc / t));

			kvalues[component.name] = kvalue;
		}

		return kvalues;
	}
}
