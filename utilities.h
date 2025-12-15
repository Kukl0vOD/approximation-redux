#pragma once

#include <vector>

template <class T>
using Matrix = std::vector<std::vector<T>>;

namespace utilities
{
	class CubicPolynomial
	{
	public:
		using Roots = std::vector<double>;

		CubicPolynomial(double a, double b, double c, double d);

		void set(double a, double b, double c, double d);
		
		double getA() const;
		double getB() const;
		double getC() const;
		double getD() const;

		bool isCanonical() const;
		double reduceToCanonical();

		double calculateQDiscriminant();
		Roots getRoots();

	private:
		double a_;
		double b_;
		double c_;
		double d_;
	};
}
