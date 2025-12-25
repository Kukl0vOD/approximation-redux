#include "utilities.h"
#include "constants.h"

#include <cassert>

namespace utilities
{
	CubicPolynomial::CubicPolynomial(double a, double b, double c, double d)
		: a_(a)
		, b_(b)
		, c_(c)
		, d_(d)
	{
	}
	void CubicPolynomial::set(double a, double b, double c, double d)
	{
		a_ = a;
		b_ = b;
		c_ = c;
		d_ = d;
	}
	double CubicPolynomial::getA() const
	{
		return a_;
	}
	double CubicPolynomial::getB() const
	{
		return b_;
	}
	double CubicPolynomial::getC() const
	{
		return c_;
	}
	double CubicPolynomial::getD() const
	{
		return d_;
	}
	bool CubicPolynomial::isCanonical() const
	{
		return fabs(a_ - 1.0) <= constants::machine_epsilon && fabs(b_) <= constants::machine_epsilon;
	}
	double CubicPolynomial::reduceToCanonical()
	{
		double p = c_ / a_ - (b_ * b_) / (3.0 * a_ * a_);
		double q = d_ / a_ - (b_ * c_) / (3.0 * a_ * a_) + (2.0 * b_ * b_ * b_) / (27.0 * a_ * a_ * a_);
		double sub = b_ / (3.0 * a_);

		set(1.0, 0.0, p, q);

		return sub;
	}
	double CubicPolynomial::calculateQDiscriminant()
	{
		assert(isCanonical() && "Polynomial must be in canonical form");

		return pow(c_ / 3.0, 3.0) + pow(d_ / 2.0, 2.0);
	}
	CubicPolynomial::Roots CubicPolynomial::getRoots()
	{
		double sub = 0.0;
		
		if (!isCanonical())
		{
			sub = reduceToCanonical();
		}

		double discriminant = calculateQDiscriminant();

		if (discriminant < 0)
		{
			double ro = sqrt(-c_ * c_ * c_ / 27.0);
			double phi = acos(-d_ / (2.0 * ro));
			double z1 = 2.0 * pow(ro, 1.0 / 3.0) * cos(phi / 3.0) - sub;
			double z2 = 2.0 * pow(ro, 1.0 / 3.0) * cos(phi / 3.0 + 2.0 * constants::pi / 3.0) - sub;
			double z3 = 2.0 * pow(ro, 1.0 / 3.0) * cos(phi / 3.0 + 4.0 * constants::pi / 3.0) - sub;

			return { z1,z2,z3 };
		}
		else
		{
			if ((fabs(-(d_ / 2.0) + sqrt(discriminant))) >= constants::machine_epsilon && (fabs(-(d_ / 2.0) - sqrt(discriminant))) >= constants::machine_epsilon)
			{
				double sign1 = (-(d_ / 2.0) + sqrt(discriminant)) / (abs(-(d_ / 2.0) + sqrt(discriminant)));
				double sign2 = (-(d_ / 2.0) - sqrt(discriminant)) / (abs(-(d_ / 2.0) - sqrt(discriminant)));

				return { sign1 * pow(abs(-(d_ / 2.0) + sqrt(discriminant)), 1.0 / 3.0) + sign2 * pow(abs(-(d_ / 2.0) - sqrt(discriminant)), 1.0 / 3.0) - sub };
			}
			else
			{
				return { pow(abs(-(d_ / 2.0) + sqrt(discriminant)), 1.0 / 3.0) + pow(abs(-(d_ / 2.0) - sqrt(discriminant)), 1.0 / 3.0) - sub };
			}
		}
	}
	double CubicPolynomial::calculatePolynomialValue(double x) const
	{
		return a_ * pow(x, 3.0) + b_ * pow(x, 2.0) + c_ * x + d_;
	}
	double UnitConverter::convert(double pressure, sol::PressureDimension input, sol::PressureDimension output)
	{
		return pressure * pressure_conversion_factor_.at(output) / pressure_conversion_factor_.at(input);
	}
	double UnitConverter::convert(double volume, sol::VolumeDimension input, sol::VolumeDimension output)
	{
		return volume * volume_conversion_factor_.at(output) / volume_conversion_factor_.at(input);
	}
	double UnitConverter::convert(double volume, sol::SpecificVolumeDimension input, sol::SpecificVolumeDimension output)
	{
		return volume * specific_volume_conversion_factor_.at(output) / specific_volume_conversion_factor_.at(input);
	}
	double UnitConverter::convert(double molar_mass, sol::MolarMassDimension input, sol::MolarMassDimension output)
	{
		return molar_mass * molar_mass_converion_factor_.at(output) / molar_mass_converion_factor_.at(output);
	}
}