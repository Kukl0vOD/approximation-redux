#pragma once

#include "utility.h"
#include "component.h"

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
		virtual double calculateClearComponentA(const sol::Component& component, const double temperature) = 0;
		virtual double calculateClearComponentB(const sol::Component& component) = 0;
		virtual double calcualteSolutionA(const std::vector<sol::Component>& components, const std::vector<double>& concentration, Matrix<double> bip) = 0;
		virtual double calculateSolutionB(const std::vector<sol::Component>& components, const std::vector<double>& concentration) = 0;

		virtual Matrix<double> calculateBIP(Correlation correlation) = 0;
		virtual std::vector<double> calculateZFactor() = 0;
		virtual std::vector<double> calculateSpecificZFactor() = 0;
	};

	class PengRobinson : public ICubicEOS
	{

	};
}
