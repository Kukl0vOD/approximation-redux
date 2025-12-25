#pragma once

#include "eos.h"

#include <string>
#include <memory>
#include <optional>
#include <functional>

namespace sol
{
	using ConcentrationCallback = std::function<std::vector<double>(State)>;

	class Solution
	{
	public:
		Solution(
			const std::vector<Component>& components,
			ConcentrationCallback callback,
			Correlation correlation,
			EOSType eos_type,
			const State& current_state,
			Phase phase
			);
		Solution(const Solution& other);
		
		const std::vector<Component>&	getComponents() const;
		const std::vector<double>&		getConcentrations() const;
		const Matrix<double>&			getBIP() const;
		const State&					getState();

		void							setComponents(const std::vector<Component> components);
		void							setCallback(const ConcentrationCallback& callback);
		void							setBip(Correlation correlation);
		void							setEOS(std::unique_ptr<eos::ICubicEOS> eos);
		void							setPressure(double pressure);
		void							setPressureDimension(PressureDimension new_dimension);
		void							setVolumeDimension(VolumeDimension new_dimension);
		void							setSpecificVolumeDimension(SpecificVolumeDimension new_dimension);
		void							setMolarMassDimension(MolarMassDimension new_dimension);

		double							calculateVolume();
		double							calculateSpecificVolume();

	private:
		void							setState(const State& state);

		std::vector<Component>			components_;
		std::vector<double>				concentations_;
		ConcentrationCallback			concentration_callback_;
		EOSType							eos_type_;
		std::unique_ptr<eos::ICubicEOS>	eos_;
		Matrix<double>					bip_;
		State							current_state_;
		Correlation						correlation_;
		Phase							phase_;
	};

	inline std::vector<double> calculateKValue(Solution gas_solution, Solution liquid_solution)
	{
		auto gas_concentrations = gas_solution.getConcentrations();
		auto liquid_concentrations = liquid_solution.getConcentrations();

		assert(gas_concentrations.size() == liquid_concentrations.size());

		std::vector<double> k_values(gas_concentrations.size());

		for (size_t i = 0; i < gas_concentrations.size(); i++)
		{
			auto k_value = gas_concentrations[i] / liquid_concentrations[i];
			k_values.push_back(k_value);
		}

		return k_values;
	}

}