#pragma once

#include <string>

namespace sol
{
	enum class Phase
	{
		LIQUID,
		GAS
	};

	class Component
	{
	public:
		Component(
			const std::string& name,
			double critical_temperature,
			double critical_pressure,
			double accentric_factor,
			double molar_mass
		);

		void		setName(const std::string& name);
		void		setCriticalTemperature(double critical_temperature);
		void		setCriticalPressure(double critical_pressure);
		void		setAccentricFactor(double accentric_factor);
		void		setMolarMass(double molar_mass);

		std::string getName() const;
		double		getCriticalTemperature() const;
		double		getCriticalPressure() const;
		double		getAccentricFactor() const;
		double		getMolarMass() const;
	private:
		std::string name_;

		double		critical_temperature_;
		double		critical_pressure_;
		double		accentric_factor_;
		double		molar_mass_;
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