#pragma once

#include <string>

namespace sol
{
	enum class Phase
	{
		LIQUID,
		GAS
	};

	//TODO: Component
	class Component
	{
	private:
		std::string name_;

		double critical_temperature_;
		double pressure_;
		double accentric_factor_;
		double molar_;
	};

	//TODO: Concentrations

	class Concentration
	{
	};

	//TODO: BIP

	class BIP
	{
		enum class Correlattion
		{
			GAO
		};


	};

	//TODO: Solution

	class Solution
	{

	};
}