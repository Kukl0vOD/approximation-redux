#pragma once

#include <vector>
#include <cassert>

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
}
