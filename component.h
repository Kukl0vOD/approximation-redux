#pragma once

#include <string>

namespace sol
{
	struct Component
	{
		std::string						name;

		double							critical_temperature;
		double							critical_pressure;
		double							accentric_factor;
		double							molar_mass;
	};
}