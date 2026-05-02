#include "approximation.h"
#include "constants.h"



namespace approx
{
	Approximator::Approximator(sol::Solution gas_solution, sol::Solution liquid_solution, double pressure_1, double pressure_2, double pressure_3, sol::VolumeType type)
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
		double liquid_volume_1 = 0.;
		double liquid_volume_2 = 0.;
		double liquid_volume_3 = 0.;

		auto weight_g1 = 0.;
		auto weight_g2 = 0.;

		auto weight_l1 = 0.;
		auto weight_l2 = 0.;
		auto weight_l3 = 0.;

		switch (type)
		{
		case sol::VolumeType::MOLAR:
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

			gas_volume_1 = utilities::UnitConverter::convert(gas_volume_1, gas_solution.getState().v_dim, sol::VolumeDimension::M3);
			gas_volume_2 = utilities::UnitConverter::convert(gas_volume_2, gas_solution.getState().v_dim, sol::VolumeDimension::M3);
			liquid_volume_1 = utilities::UnitConverter::convert(liquid_volume_1, liquid_solution.getState().v_dim, sol::VolumeDimension::M3);
			liquid_volume_2 = utilities::UnitConverter::convert(liquid_volume_2, liquid_solution.getState().v_dim, sol::VolumeDimension::M3);
			liquid_volume_3 = utilities::UnitConverter::convert(liquid_volume_3, liquid_solution.getState().v_dim, sol::VolumeDimension::M3);
			break;
		case sol::VolumeType::SPECIFIC:
			gas_solution.setPressure(pressure_1);

			for (const auto& component : gas_solution.getComponents())
			{
				auto molar_mass = utilities::UnitConverter::convert(component.molar_mass, component.mm_dim, sol::MolarMassDimension::KG);
				weight_g1 += gas_solution.getConcentrations().at(component.name) * molar_mass;
			}

			gas_volume_1 = gas_solution.calculateSpecificVolume();

			gas_solution.setPressure(pressure_2);

			for (const auto& component : gas_solution.getComponents())
			{
				auto molar_mass = utilities::UnitConverter::convert(component.molar_mass, component.mm_dim, sol::MolarMassDimension::KG);
				weight_g2 += gas_solution.getConcentrations().at(component.name) * molar_mass;
			}

			gas_volume_2 = gas_solution.calculateSpecificVolume();

			liquid_solution.setPressure(pressure_1);

			for (const auto& component : liquid_solution.getComponents())
			{
				auto molar_mass = utilities::UnitConverter::convert(component.molar_mass, component.mm_dim, sol::MolarMassDimension::KG);
				weight_l1 += liquid_solution.getConcentrations().at(component.name) * molar_mass;
			}

			liquid_volume_1 = liquid_solution.calculateSpecificVolume();

			liquid_solution.setPressure(pressure_2);

			for (const auto& component : liquid_solution.getComponents())
			{
				auto molar_mass = utilities::UnitConverter::convert(component.molar_mass, component.mm_dim, sol::MolarMassDimension::KG);
				weight_l2 += liquid_solution.getConcentrations().at(component.name) * molar_mass;
			}
			liquid_volume_2 = liquid_solution.calculateSpecificVolume();

			liquid_solution.setPressure(pressure_3);

			for (const auto& component : liquid_solution.getComponents())
			{
				auto molar_mass = utilities::UnitConverter::convert(component.molar_mass, component.mm_dim, sol::MolarMassDimension::KG);
				weight_l3 += liquid_solution.getConcentrations().at(component.name) * molar_mass;
			}

			liquid_volume_3 = liquid_solution.calculateSpecificVolume();

			gas_volume_1 = utilities::UnitConverter::convert(gas_volume_1, gas_solution.getState().sv_dim, sol::SpecificVolumeDimension::M3_PER_KG);
			gas_volume_2 = utilities::UnitConverter::convert(gas_volume_2, gas_solution.getState().sv_dim, sol::SpecificVolumeDimension::M3_PER_KG);
			liquid_volume_1 = utilities::UnitConverter::convert(liquid_volume_1, liquid_solution.getState().sv_dim, sol::SpecificVolumeDimension::M3_PER_KG);
			liquid_volume_2 = utilities::UnitConverter::convert(liquid_volume_2, liquid_solution.getState().sv_dim, sol::SpecificVolumeDimension::M3_PER_KG);
			liquid_volume_3 = utilities::UnitConverter::convert(liquid_volume_3, liquid_solution.getState().sv_dim, sol::SpecificVolumeDimension::M3_PER_KG);
			break;
		default:
			break;
		}

		pressure_1=utilities::UnitConverter::convert(pressure_1, gas_solution.getState().p_dim, sol::PressureDimension::PA);
		pressure_2=utilities::UnitConverter::convert(pressure_2, gas_solution.getState().p_dim, sol::PressureDimension::PA);
		pressure_3=utilities::UnitConverter::convert(pressure_3, gas_solution.getState().p_dim, sol::PressureDimension::PA);

		double Rg1 = R / weight_g1;
		double Rg2 = R / weight_g2;
		double Rl1 = R / weight_l1;
		double Rl2 = R / weight_l2;
		double Rl3 = R / weight_l3;

		switch (type)
		{
		case sol::VolumeType::MOLAR:
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
			break;
		case sol::VolumeType::SPECIFIC:
			beta_ = pressure_1 * pressure_2 * (gas_volume_1 - gas_volume_2)
				/ (t * (R*pressure_2 - R*pressure_1));
			bg_ = gas_volume_1 - (beta_ * R * t) / pressure_1;
			pextra_ = (pressure_1 * pressure_2 * (liquid_volume_2 - liquid_volume_1)
				+ pressure_1 * pressure_3 * (liquid_volume_1 - liquid_volume_3)
				+ pressure_2 * pressure_3 * (liquid_volume_3 - liquid_volume_2))
				/ ((liquid_volume_2 - liquid_volume_1) * (pressure_3 - pressure_1)
					+ (liquid_volume_3 - liquid_volume_1) * (pressure_1 - pressure_2));
			bl_ = (liquid_volume_1 * pressure_1 - liquid_volume_2 * pressure_2)
				/ (pressure_1 - pressure_2) + (liquid_volume_1 - liquid_volume_2) * pextra_
				/ (pressure_1 - pressure_2);
			alpha_ = (liquid_volume_1 - bl_) * (pressure_1 + pextra_) / (R * t);
			break;
		default:
			break;
		}
	}

	double Approximator::approximateGasVolume(double pressure, sol::PressureDimension from_p_dim, std::variant<sol::VolumeDimension, sol::SpecificVolumeDimension> res_v_dim)
	{
		auto R = constants::universal_gas_constant;
		pressure = utilities::UnitConverter::convert(pressure, from_p_dim, sol::PressureDimension::PA);

		auto volume = (beta_ * R * temperature_) / pressure + bg_;
		sol::VolumeDimension v_dim;
		sol::SpecificVolumeDimension sv_dim;
		switch (type_)
		{
		case sol::VolumeType::MOLAR:
			if (!std::holds_alternative<sol::VolumeDimension>(res_v_dim))
			{
				throw std::runtime_error("Type error");
			}
			v_dim = std::get<sol::VolumeDimension>(res_v_dim);
			volume = utilities::UnitConverter::convert(volume, sol::VolumeDimension::M3, v_dim);
			break;
		case sol::VolumeType::SPECIFIC:
			if (!std::holds_alternative<sol::SpecificVolumeDimension>(res_v_dim))
			{
				throw std::runtime_error("Type error");
			}
			sv_dim = std::get<sol::SpecificVolumeDimension>(res_v_dim);
			volume = utilities::UnitConverter::convert(volume, sol::SpecificVolumeDimension::M3_PER_KG, sv_dim);
			break;
		default:
			break;
		}

		return volume;
	}

	double Approximator::approximateLiquidVolume(double pressure, sol::PressureDimension from_p_dim, std::variant<sol::VolumeDimension, sol::SpecificVolumeDimension> res_v_dim)
	{
		auto R = constants::universal_gas_constant;
		pressure = utilities::UnitConverter::convert(pressure, from_p_dim, sol::PressureDimension::PA);

		auto volume = (alpha_ * R * temperature_) / (pressure + pextra_) + bl_;
		sol::VolumeDimension v_dim;
		sol::SpecificVolumeDimension sv_dim;
		switch (type_)
		{
		case sol::VolumeType::MOLAR:
			if (!std::holds_alternative<sol::VolumeDimension>(res_v_dim))
			{
				throw std::runtime_error("Type error");
			}
			v_dim = std::get<sol::VolumeDimension>(res_v_dim);
			volume = utilities::UnitConverter::convert(volume, sol::VolumeDimension::M3, v_dim);
			break;
		case sol::VolumeType::SPECIFIC:
			if (!std::holds_alternative<sol::SpecificVolumeDimension>(res_v_dim))
			{
				throw std::runtime_error("Type error");
			}
			sv_dim = std::get<sol::SpecificVolumeDimension>(res_v_dim);
			volume = utilities::UnitConverter::convert(volume, sol::SpecificVolumeDimension::M3_PER_KG, sv_dim);
			break;
		default:
			break;
		}

		return volume;
	}

	std::unordered_map<std::string, double> Approximator::approximateKValue(double pressure, double pressure_0, sol::PressureDimension from_p_dim)
	{
		auto R = constants::universal_gas_constant;

		//gas_solution_.setPressureDimension(from_p_dim);
		//liquid_solution_.setPressureDimension(from_p_dim);
		//pressure = utilities::UnitConverter::convert(pressure, from_p_dim, sol::PressureDimension::PA);
		//pressure_0 = utilities::UnitConverter::convert(pressure_0, from_p_dim, sol::PressureDimension::PA);

		gas_solution_.setPressure(pressure_0);
		liquid_solution_.setPressure(pressure_0);

		auto gas_concentrations = gas_solution_.getConcentrations();
		auto liquid_concentrations = liquid_solution_.getConcentrations();

		assert(gas_concentrations.size() == liquid_concentrations.size());

		std::unordered_map<std::string, double> k_values(gas_concentrations.size());

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

	Constants Approximator::getConstants() const
	{
		return
		{
			.alpha = alpha_,
			.beta = beta_,
			.bg = bg_,
			.bl = bl_,
			.pextra = pextra_
		};
	}

}