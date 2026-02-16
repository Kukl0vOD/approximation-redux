#pragma once

#include "solution.h"
#include <variant>

namespace approx
{
	class Approximator
	{
	public:
		Approximator(
			sol::Solution gas_solution,
			sol::Solution liquid_solution,
			double pressure_1,
			double pressure_2,
			double pressure_3,
			sol::VolumeType type = sol::VolumeType::MOLAR
		);
		double										approximateGasVolume(double pressure, sol::PressureDimension from_p_dim, std::variant<sol::VolumeDimension, sol::SpecificVolumeDimension> res_v_dim);
		double										approximateLiquidVolume(double pressure, sol::PressureDimension from_p_dim, std::variant<sol::VolumeDimension, sol::SpecificVolumeDimension> res_v_dim);
		std::unordered_map<std::string, double>		approximateKValue(double pressure, double pressure_0, sol::PressureDimension from_p_dim);

	private:
		sol::Solution								gas_solution_;
		sol::Solution								liquid_solution_;

		double										temperature_;
		double										alpha_;
		double										beta_;
		double										bg_;
		double										bl_;
		double										pextra_;

		sol::VolumeType								type_;
	};
}