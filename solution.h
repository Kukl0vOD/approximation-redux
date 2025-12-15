#pragma once

#include "eos.h"

#include <string>
#include <memory>
#include <optional>
#include <functional>

namespace sol
{
	using ConcentrationCallback = std::function<std::vector<double>(State)>;

	class Solution
	{
		Solution(
			const std::vector<Component>& components,
			ConcentrationCallback callback,
			Correlation correlation,
			std::unique_ptr<eos::ICubicEOS> eos,
			const State& current_state
			);

		std::vector<Component>			getComponents() const;
		std::vector<double>				getConcentrations() const;
		Matrix<double>					getBIP() const;
		State							getState();

		void							setComponents(const std::vector<Component> components);
		void							setCallback(const ConcentrationCallback& callback);
		void							setBip(Correlation correlation);
		void							setEOS(std::unique_ptr<eos::ICubicEOS> eos);
		void							setState(const State& state);

		double							calculateVolume();
		double							calculateSpecificVolume();

	private:
		std::vector<Component>			components_;
		std::vector<double>				concentations_;
		ConcentrationCallback			concentration_callback_;
		Matrix<double>					bip_;

		std::unique_ptr<eos::ICubicEOS>	eos_;

		State							current_state_;
	};

}