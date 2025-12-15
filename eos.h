#pragma once

#include "utilities.h"
#include "solution_properties.h"

enum class Correlation
{
	GAO
};

namespace eos
{
	class ICubicEOS
	{
	public:
		virtual ~ICubicEOS() = 0;
		virtual double calculateClearComponentA(const sol::Component& component, const double temperature) const = 0;
		virtual double calculateClearComponentB(const sol::Component& component) const = 0;
		virtual double calcualteSolutionA(const std::vector<sol::Component>& components, const std::vector<double>& concentration, Matrix<double> bip, double temperature) const = 0;
		virtual double calculateSolutionB(const std::vector<sol::Component>& components, const std::vector<double>& concentration) const = 0;

		virtual Matrix<double> calculateBIP(const std::vector<sol::Component>& components, Correlation correlation, sol::Phase phase) = 0;

		virtual std::vector<double> calculateZFactor(const sol::Component& component) const = 0;
		virtual std::vector<double> calculateSpecificZFactor(const sol::Component& component) const = 0;
		virtual std::vector<double> calculateZFactor(const sol::Component& component, sol::State current_state) const = 0;
		virtual std::vector<double> calculateSpecificZFactor(const sol::Component& component, sol::State current_state) const = 0;
		virtual std::vector<double> calculateZFactor(const std::vector<sol::Component>& components, const std::vector<double>& concentration, Matrix<double> bip, sol::State current_state) const = 0;
		virtual std::vector<double> calculateSpecificZFactor(const std::vector<sol::Component>& components, const std::vector<double>& concentration, Matrix<double> bip, sol::State current_state) const = 0;
	};

	class PengRobinson : public ICubicEOS
	{
		~PengRobinson() override = default;
		double calculateClearComponentA(const sol::Component& component, const double temperature) const override;
		double calculateClearComponentB(const sol::Component& component) const override;
		double calcualteSolutionA(const std::vector<sol::Component>& components, const std::vector<double>& concentration, Matrix<double> bip, double temperature) const override;
		double calculateSolutionB(const std::vector<sol::Component>& components, const std::vector<double>& concentration) const override;

		Matrix<double> calculateBIP(const std::vector<sol::Component>& components, Correlation correlation, sol::Phase phase) override;

		virtual std::vector<double> calculateZFactor(const sol::Component& component) const override;
		virtual std::vector<double> calculateSpecificZFactor(const sol::Component& component) const override;
		virtual std::vector<double> calculateZFactor(const sol::Component& component, sol::State current_state) const override;
		virtual std::vector<double> calculateSpecificZFactor(const sol::Component& component, sol::State current_state) const override;
		std::vector<double> calculateZFactor(const std::vector<sol::Component>& components, const std::vector<double>& concentration, Matrix<double> bip, sol::State current_state) const override;
		std::vector<double> calculateSpecificZFactor(const std::vector<sol::Component>& components, const std::vector<double>& concentration, Matrix<double> bip, sol::State current_state) const override;
	};
}
