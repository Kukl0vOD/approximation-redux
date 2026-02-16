#pragma once

#include "utilities.h"
#include "solution_properties.h"

#include <stdexcept>
#include <memory>

enum class EOSType
{
	PENG_ROBINSON
};

enum class Correlation
{
	GAO
};

namespace eos
{
	class ICubicEOS
	{
	public:
		virtual								~ICubicEOS() = 0;
		virtual double						calculateClearComponentA(const sol::Component& component, const double temperature) const = 0;
		virtual double						calculateClearComponentB(const sol::Component& component) const = 0;
		virtual double						calculateSolutionA(const std::vector<sol::Component>& components, const std::unordered_map<std::string, double>&, Matrix<double> bip, double temperature) const = 0;
		virtual double						calculateSolutionB(const std::vector<sol::Component>& components, const std::unordered_map<std::string, double>&) const = 0;

		virtual Matrix<double>				calculateBIP(const std::vector<sol::Component>& components, Correlation correlation, sol::Phase phase) = 0;

		virtual std::vector<double>			calculateZFactor(const sol::Component& component) const = 0;
		virtual std::vector<double>			calculateZFactor(const sol::Component& component, sol::State current_state) const = 0;
		virtual std::vector<double>			calculateZFactor(const std::vector<sol::Component>& components, const std::unordered_map<std::string, double>& concentration, Matrix<double> bip, sol::State current_state) const = 0;
	};

	class PengRobinson : public ICubicEOS
	{
	public:
											~PengRobinson() override = default;
		double								calculateClearComponentA(const sol::Component& component, const double temperature) const override;
		double								calculateClearComponentB(const sol::Component& component) const override;
		double								calculateSolutionA(const std::vector<sol::Component>& components, const std::unordered_map<std::string, double>&, Matrix<double> bip, double temperature) const override;
		double								calculateSolutionB(const std::vector<sol::Component>& components, const std::unordered_map<std::string, double>&) const override;

		Matrix<double>						calculateBIP(const std::vector<sol::Component>& components, Correlation correlation, sol::Phase phase) override;

		std::vector<double>					calculateZFactor(const sol::Component& component) const override;
		std::vector<double>					calculateZFactor(const sol::Component& component, sol::State current_state) const override;
		std::vector<double>					calculateZFactor(const std::vector<sol::Component>& components, const std::unordered_map<std::string, double>& concentration, Matrix<double> bip, sol::State current_state) const override;
	};

	class EOSFactory
	{
	public:
		static std::unique_ptr<ICubicEOS>	createEOS(EOSType type);
	};
}
