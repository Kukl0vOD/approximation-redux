#pragma once

#include "solution_properties.h"

#include <vector>
#include <cassert>
#include <unordered_map>

template <class T>
using Matrix = std::vector<std::vector<T>>;

namespace utilities
{
	class CubicPolynomial
	{
	public:
		using Roots = std::vector<double>;

		CubicPolynomial(double a, double b, double c, double d);

		void	set(double a, double b, double c, double d);
		
		double	getA() const;
		double	getB() const;
		double	getC() const;
		double	getD() const;

		bool	isCanonical() const;
		double	reduceToCanonical();

		double	calculateQDiscriminant();
		Roots	getRoots();
		
		double calculatePolynomialValue(double x) const;

	private:
		double	a_;
		double	b_;
		double	c_;
		double	d_;
	};

	inline double LagrangeInterpolation(const std::vector<double>& x_vec, const std::vector<double>& y_vec, double x)
	{
		assert(x_vec.size() == y_vec.size() && "Expected x values size = y values size");

		double y = 0.0;

		for (size_t i = 0; i < x_vec.size(); i++)
		{
			double basis = 1.0;
			for (size_t j = 0; j < x_vec.size(); j++)
			{
				if (i != j)
				{
					basis *= (x - x_vec[j]) / (x_vec[i] - x_vec[j]);
				}
			}
			basis *= y_vec[i];
			y += basis;
		}

		return y;
	}

	class UnitConverter
	{
	public:
		static double convert(double pressure, sol::PressureDimension input, sol::PressureDimension output);
		static double convert(double volume, sol::VolumeDimension input, sol::VolumeDimension output);
		static double convert(double volume, sol::SpecificVolumeDimension input, sol::SpecificVolumeDimension output);
		static double convert(double molar_mass, sol::MolarMassDimension input, sol::MolarMassDimension output);

	private:
		inline static const std::unordered_map<sol::PressureDimension, double> pressure_conversion_factor_
		{
			{sol::PressureDimension::PA, 1.0},
			{sol::PressureDimension::MPA, 0.000001},
			{sol::PressureDimension::BAR, 0.00001}
		};
		inline static const std::unordered_map<sol::VolumeDimension, double> volume_conversion_factor_
		{
			{sol::VolumeDimension::M3, 1.0},
			{sol::VolumeDimension::ML, 0.000001},
			{sol::VolumeDimension::LITER, 0.001}
		};
		inline static const std::unordered_map<sol::SpecificVolumeDimension, double> specific_volume_conversion_factor_
		{
			{sol::SpecificVolumeDimension::M3_PER_KG, 1.0},
			{sol::SpecificVolumeDimension::LITER_PER_GRAMM, 1.0},
			{sol::SpecificVolumeDimension::ML_PER_GRAMM, 0.001}
		};
		inline static const std::unordered_map<sol::MolarMassDimension, double> molar_mass_converion_factor_
		{
			{sol::MolarMassDimension::KG, 1.0},
			{sol::MolarMassDimension::GRAMM, 0.001}
		};
	};
}
