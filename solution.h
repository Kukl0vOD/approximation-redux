#pragma once

#include "eos.h"

#include <string>
#include <memory>
#include <optional>
#include <functional>

namespace sol
{
	using ConcentrationCallback = std::function<std::unordered_map<std::string, double>(State)>;

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
		const std::unordered_map<std::string, double>& getConcentrations() const;
		const Matrix<double>&			getBIP() const;
		Phase							getPhase() const;
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
		void							setState(const State& state);
		void							setPhase(Phase phase);

		double							calculateVolume();
		double							calculateSpecificVolume();

	private:

		std::vector<Component>			components_;
		std::unordered_map<std::string, double>				concentations_;
		ConcentrationCallback			concentration_callback_;
		EOSType							eos_type_;
		std::unique_ptr<eos::ICubicEOS>	eos_;
		Matrix<double>					bip_;
		State							current_state_;
		Correlation						correlation_;
		Phase							phase_;
	};

	inline std::unordered_map<std::string, double> calculateKValue(Solution gas_solution, Solution liquid_solution)
	{
		auto gas_concentrations = gas_solution.getConcentrations();
		auto liquid_concentrations = liquid_solution.getConcentrations();

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

}