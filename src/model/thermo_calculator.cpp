#include "model/thermo_calculator.h"

#include "constants.h"
#include "model/mixture_properties.h"
#include "model/z_factor_selector.h"

namespace sol
{
	double ThermoCalculator::calculateMolarVolume(
		const eos::ICubicEOS& eos,
		const std::vector<Component>& components,
		const std::unordered_map<std::string, double>& concentrations,
		const Matrix<double>& bip,
		const State& state,
		Phase phase
	)
	{
		auto R = constants::universal_gas_constant;
		auto t = state.temperature;
		auto p = utilities::UnitConverter::convert(state.pressure, state.p_dim, PressureDimension::PA);
		auto roots = eos.calculateZFactor(components, concentrations, bip, state);
		auto z_factor = selectZFactor(roots, phase);

		return utilities::UnitConverter::convert((z_factor * R * t) / p, VolumeDimension::M3, state.v_dim);
	}

	double ThermoCalculator::calculateSpecificVolume(
		const eos::ICubicEOS& eos,
		const std::vector<Component>& components,
		const std::unordered_map<std::string, double>& concentrations,
		const Matrix<double>& bip,
		const State& state,
		Phase phase
	)
	{
		double weight = calculateAverageMolarMass(components, concentrations);

		auto R = constants::universal_gas_constant / weight;
		auto t = state.temperature;
		auto p = utilities::UnitConverter::convert(state.pressure, state.p_dim, PressureDimension::PA);
		auto roots = eos.calculateZFactor(components, concentrations, bip, state, R);
		auto z_factor = selectZFactor(roots, phase);

		return utilities::UnitConverter::convert((z_factor * R * t) / p, SpecificVolumeDimension::M3_PER_KG, state.sv_dim);
	}
}
