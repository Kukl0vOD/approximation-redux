#include "solution.h"
#include "constants.h"

namespace sol
{
	Component::Component(
		const std::string& name,
		double critical_temperature,
		double critical_pressure,
		double accentric_factor,
		double molar_mass)
		: name_(name)
		, critical_temperature_(critical_temperature)
		, critical_pressure_(critical_pressure)
		, accentric_factor_(accentric_factor)
		, molar_mass_(molar_mass)
	{
	}
	void Component::setName(const std::string& name)
	{
		name_ = name;
	}
	void Component::setCriticalTemperature(double critical_temperature)
	{
		critical_temperature_ = critical_temperature;
	}
	void Component::setCriticalPressure(double critical_pressure)
	{
		critical_pressure_ = critical_pressure;
	}
	void Component::setAccentricFactor(double accentric_factor)
	{
		accentric_factor_ = accentric_factor;
	}
	void Component::setMolarMass(double molar_mass)
	{
		molar_mass_ = molar_mass;
	}
	std::string Component::getName() const
	{
		return name_;
	}
	double Component::getCriticalTemperature() const
	{
		return critical_temperature_;
	}
	double Component::getCriticalPressure() const
	{
		return critical_pressure_;
	}
	double Component::getAccentricFactor() const
	{
		return accentric_factor_;
	}
	double Component::getMolarMass() const
	{
		return molar_mass_;
	}
}