#include "approximation.h"
#include "constants.h"

namespace approx
{
	Approximator::Approximator(sol::Solution gas_solution, sol::Solution liquid_solution, double pressure_1, double pressure_2, double pressure_3, VolumeType type)
		: gas_solution_(gas_solution)
		, liquid_solution_(liquid_solution)
		, temperature_(gas_solution.getState().temperature)
		, type_(type)
	{
		auto t = temperature_;
		auto R = constants::universal_gas_constant;

		assert((t - liquid_solution.getState().temperature)<=constants::machine_epsilon);

		double gas_volume_1 = 0.;
		double gas_volume_2 = 0.;
		double liquid_volume_1 =0.;
		double liquid_volume_2 = 0.;
		double liquid_volume_3 = 0.;

		switch (type)
		{
		case approx::VolumeType::MOLAR:
			gas_solution.setPressure(pressure_1);
			gas_volume_1 = gas_solution.calculateVolume();

			gas_solution.setPressure(pressure_2);
			gas_volume_2 = gas_solution.calculateVolume();

			liquid_solution.setPressure(pressure_1);
			liquid_volume_1 = liquid_solution.calculateVolume();

			liquid_solution.setPressure(pressure_2);
			liquid_volume_2 = liquid_solution.calculateVolume();

			liquid_solution.setPressure(pressure_3);
			liquid_volume_3 = liquid_solution.calculateVolume();
			break;
		case approx::VolumeType::SPECIFIC:
			gas_solution.setPressure(pressure_1);
			gas_volume_1 = gas_solution.calculateSpecificVolume();

			gas_solution.setPressure(pressure_2);
			gas_volume_2 = gas_solution.calculateSpecificVolume();

			liquid_solution.setPressure(pressure_1);
			liquid_volume_1 = liquid_solution.calculateSpecificVolume();

			liquid_solution.setPressure(pressure_2);
			liquid_volume_2 = liquid_solution.calculateSpecificVolume();

			liquid_solution.setPressure(pressure_3);
			liquid_volume_3 = liquid_solution.calculateSpecificVolume();
			break;
		default:
			break;
		}

		beta_ = pressure_1 * pressure_2 * (gas_volume_1 - gas_volume_2)
			/ (R * t * (pressure_2 - pressure_1));
		bg_ = (gas_volume_1 * pressure_1 - gas_volume_2 * pressure_2)
			/ (pressure_1 - pressure_2);
		pextra_ = (pressure_1 * pressure_2 * (liquid_volume_2 - liquid_volume_1) 
			+ pressure_1 * pressure_3 * (liquid_volume_1 - liquid_volume_3)
			+ pressure_2 * pressure_3 * (liquid_volume_3 - liquid_volume_2))
			/ ((liquid_volume_2 - liquid_volume_1) * (pressure_3 - pressure_1) 
			+ (liquid_volume_3 - liquid_volume_1) * (pressure_1 - pressure_2));
		bl_ = (liquid_volume_1 * pressure_1 - liquid_volume_2 * pressure_2)
			/ (pressure_1 - pressure_2) + (liquid_volume_1 - liquid_volume_2) * pextra_ 
			/ (pressure_1 - pressure_2);
		alpha_ = (liquid_volume_1 - bl_) * (pressure_1 + pextra_) / (R * t);
	}

	double Approximator::approximateGasVolume(double pressure)
	{
		auto R = constants::universal_gas_constant;
		auto volume = (beta_ * R * temperature_) / pressure + bg_;

		return volume;
	}

	double Approximator::approximateLiquidVolume(double pressure)
	{
		auto R = constants::universal_gas_constant;
		auto volume = (alpha_ * R * temperature_) / (pressure+pextra_) + bl_;

		return volume;
	}

	std::unordered_map<std::string_view, double> Approximator::approximateKValue(double pressure, double pressure_0)
	{
		auto R = constants::universal_gas_constant;

		gas_solution_.setPressure(pressure_0);
		liquid_solution_.setPressure(pressure_0);

		auto gas_concentrations = gas_solution_.getConcentrations();
		auto liquid_concentrations = liquid_solution_.getConcentrations();

		assert(gas_concentrations.size() == liquid_concentrations.size());

		std::unordered_map<std::string_view, double> k_values(gas_concentrations.size());

		for (const auto& [name, g_concentration] : gas_concentrations)
		{
			auto l_concentration_it = liquid_concentrations.find(name);

			if (l_concentration_it == liquid_concentrations.end())
			{
				throw std::logic_error("different solutions");
			}

			auto k0 = g_concentration / l_concentration_it->second;
			auto temp_k_value = k0 * pow((pressure + pextra_) / (pressure_0 + pextra_), alpha_)
				* pow(pressure_0 / pressure, beta_)
				* exp((bl_ - bg_) * (pressure - pressure_0) / (R * temperature_));

			k_values[name] = temp_k_value;
		}
		
		return k_values;
	}

}