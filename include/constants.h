#pragma once

namespace constants
{
	inline constexpr double universal_gas_constant = 8.31446261815324;
	inline constexpr double machine_epsilon = 1e-8;
	inline constexpr double pi = 3.14159265358979323846;

	namespace peng_robinson
	{
		inline constexpr double m_base = 0.37464;
		inline constexpr double m_linear_factor = 1.54226;
		inline constexpr double m_quadratic_factor = 0.2699;
		inline constexpr double a_factor = 0.45724;
		inline constexpr double b_factor = 0.07780;
	}

	namespace wilson
	{
		inline constexpr double exponent_factor = 5.31;
	}

	namespace application
	{
		inline constexpr double percentage_multiplier = 100.0;
		inline constexpr double molar_volume_epsilon = 1.0;
		inline constexpr double specific_volume_epsilon = 0.01;
	}
}
