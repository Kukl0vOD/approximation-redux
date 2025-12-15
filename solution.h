#pragma once

#include "eos.h"

#include <string>
#include <memory>
#include <optional>

namespace sol
{
	enum class Phase
	{
		LIQUID,
		GAS,
		TWO_PHASED
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

	class Solution
	{
		Solution(
			const std::vector<Component>& components,
			const std::vector<double>& concentrations,
			Correlation correlation,
			std::unique_ptr<eos::ICubicEOS> eos,
			const State& current_state
			);

	private:
		std::vector<Component>			components_;
		std::vector<double>				concentations_;
		Matrix<double>					bip_;

		std::unique_ptr<eos::ICubicEOS>	eos_;

		State							current_state_;
	};
}