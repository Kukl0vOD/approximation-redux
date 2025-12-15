#pragma once

#include <string>
#include <optional>

namespace sol
{
	enum class Phase
	{
		LIQUID,
		GAS,
	};

	enum PressureDimension
	{
		BAR
	};

	enum VolumeDimension
	{
		LITER
	};

	enum SpecificVolumeDimension
	{
		LITER_PER_GRAMM
	};

	struct Component
	{
		std::string	name;

		double		critical_temperature;
		double		critical_pressure;
		double		accentric_factor;
		double		molar_mass;
	};

	struct State
	{
		Phase							phase;
		PressureDimension				p_dim;
		double							pressure;
		double							temperature;
		VolumeDimension					v_dim;
		std::optional<double>			volume = std::nullopt;
		SpecificVolumeDimension			sv_dim;
		std::optional<double>			specific_volume = std::nullopt;
	};
}