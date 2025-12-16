#include "eos.h"
#include "constants.h"

#include <algorithm>

namespace eos
{
	double PengRobinson::calculateClearComponentA(const sol::Component& component, const double temperature) const
	{
		double w = component.accentric_factor;
		double t_c = component.critical_temperature;
		double p_c = component.critical_pressure;
		double R = constants::universal_gas_constant;

		double m = 0.37464 + 1.54226 * w - 0.2699 * w * w;
		double alpha = (1.0 + m * (1.0 - sqrt(temperature / t_c))) * (1.0 + m * (1.0 - sqrt(temperature / t_c)));
		double a = 0.45724 * R * R * t_c * t_c * alpha / p_c;

		return a;
	}
	double PengRobinson::calculateClearComponentB(const sol::Component& component) const
	{
		double t_c = component.critical_temperature;
		double p_c = component.critical_pressure;
		double R = constants::universal_gas_constant;

		double b = 0.07780 * R * t_c / p_c;

		return b;
	}
	double PengRobinson::calcualteSolutionA(const std::vector<sol::Component>& components, const std::vector<double>& concentration, Matrix<double> bip, double temperature) const
	{
		std::vector<double> a_clear;
		a_clear.reserve(components.size());

		for (const auto& component : components)
		{
			a_clear.push_back(calculateClearComponentA(component, temperature));
		}

		double a_solution = 0;

		for (size_t i = 0; i < components.size(); i++)
		{
			for (size_t j = 0; j < components.size(); j++)
			{
				a_solution += (1 - bip[i][j]) * concentration[i] * concentration[j] * sqrt(a_clear[i] * a_clear[j]);
			}
		}

		return a_solution;
	}
	double PengRobinson::calculateSolutionB(const std::vector<sol::Component>& components, const std::vector<double>& concentration) const
	{
		double b_solution = 0;

		for (size_t i = 0; i < components.size(); i++)
		{
			b_solution += concentration[i] * calculateClearComponentB(components[i]);
		}

		return b_solution;
	}
	Matrix<double> PengRobinson::calculateBIP(const std::vector<sol::Component>& components, Correlation correlation, sol::Phase phase)
	{
		Matrix<double> bip;
		bip.reserve(components.size());

		for (auto& row : bip)
		{
			row.reserve(components.size());
		}

		switch (correlation)
		{
		case Correlation::GAO:

			for (size_t i = 0; i < components.size(); i++)
			{
				for (size_t j = 0; j < components.size(); j++)
				{
					auto roots_i = calculateZFactor(components[i]);
					auto roots_j = calculateZFactor(components[j]);
					double zi_factor = 0.0;
					double zj_factor = 0.0;

					switch (phase)
					{
					case sol::Phase::LIQUID:
						zi_factor = *std::min_element(roots_i.begin(), roots_i.end());
						zj_factor = *std::min_element(roots_j.begin(), roots_j.end());
						break;
					case sol::Phase::GAS:
						zi_factor = *std::max_element(roots_i.begin(), roots_i.end());
						zj_factor = *std::max_element(roots_j.begin(), roots_j.end());
						break;
					default:
						break;
					}

					double t_ci = components[i].critical_temperature;
					double t_cj = components[j].critical_temperature;

					double kij = 1 - pow((2 * sqrt(t_ci * t_cj) / (t_ci + t_cj)), (zi_factor + zj_factor) / 2);

					bip[i].push_back(kij);
				}
			}

			return bip;
		default:
			return bip;
		}
	}
	std::vector<double> PengRobinson::calculateZFactor(const sol::Component& component) const
	{
		double p_c = component.critical_pressure;
		double t_c = component.critical_temperature;
		double R = constants::universal_gas_constant;

		double a = calculateClearComponentA(component, t_c);
		double b = calculateClearComponentB(component);

		double A = a * p_c / pow(R * t_c, 2);
		double B = b * p_c / (R * t_c);

		utilities::CubicPolynomial equation(1.0, -(1.0 - B), (A - 3.0 * B * B - 2.0 * B), -(A * B - B * B - B * B * B));

		return equation.getRoots();
	}
	std::vector<double> PengRobinson::calculateZFactor(const sol::Component& component, sol::State current_state) const
	{
		double p = current_state.pressure;
		double t = current_state.temperature;
		double R = constants::universal_gas_constant;

		double a = calculateClearComponentA(component, t);
		double b = calculateClearComponentB(component);

		double A = a * p / pow(R * t, 2);
		double B = b * p / (R * t);

		utilities::CubicPolynomial equation(1.0, -(1.0 - B), (A - 3.0 * B * B - 2.0 * B), -(A * B - B * B - B * B * B));

		return equation.getRoots();
	}
	std::vector<double> PengRobinson::calculateZFactor(const std::vector<sol::Component>& components, const std::vector<double>& concentration, Matrix<double> bip, sol::State current_state) const
	{
		double p = current_state.pressure;
		double t = current_state.temperature;
		double R = constants::universal_gas_constant;

		double a = calcualteSolutionA(components,concentration, bip, t);
		double b = calculateSolutionB(components, concentration);

		double A = a * p / pow(R * t, 2);
		double B = b * p / (R * t);

		utilities::CubicPolynomial equation(1.0, -(1.0 - B), (A - 3.0 * B * B - 2.0 * B), -(A * B - B * B - B * B * B));

		return equation.getRoots();
	}
}