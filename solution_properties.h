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

	enum class PressureDimension
	{
		PA,
		MPA,
		BAR
	};

	enum class VolumeType
	{
		MOLAR,
		SPECIFIC
	};

	enum class VolumeDimension
	{
		M3,
		ML,
		LITER
	};

	enum class SpecificVolumeDimension
	{
		LITER_PER_GRAMM,
		ML_PER_GRAMM,
		M3_PER_KG
	};

	enum class MolarMassDimension
	{
		KG,
		GRAMM
	};

	struct Component
	{
		std::string				name;

		double					critical_temperature;
		double					critical_pressure;
		double					accentric_factor;
		double					molar_mass;

		PressureDimension		p_dim;
		MolarMassDimension		mm_dim;
	};

	struct State
	{
		double					pressure;
		double					temperature;

		PressureDimension		p_dim;
		VolumeDimension			v_dim;
		SpecificVolumeDimension	sv_dim;

		std::optional<double>	volume = std::nullopt;
		std::optional<double>	specific_volume = std::nullopt;
	};
}