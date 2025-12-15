#pragma once

#include "eos.h"

#include <string>
#include <memory>
#include <optional>

namespace sol
{
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