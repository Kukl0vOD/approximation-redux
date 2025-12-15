#pragma once

#include "constants.h"
#include "solution.h"
#include "utilities.h"

#include <cassert>
#include <iostream>

namespace test
{
	//CubicPolynomial test
	void testIsCanonicalInit()
	{
		utilities::CubicPolynomial test(1.0, 0.0, 3.0, 2.0);
		assert(test.isCanonical());
	}
	void testIsCanonicalAfterReduce()
	{
		utilities::CubicPolynomial test(1.0, 2.0, 3.0, 2.0);
		test.reduceToCanonical();
		assert(test.isCanonical());
	}
	void testQDiscriminant()
	{
		utilities::CubicPolynomial test(1.0, 0.0, 3.0, 2.0);
		assert(fabs(test.calculateQDiscriminant() - 2) <= constants::machine_epsilon);
	}
	void testRootCalculation()
	{
		utilities::CubicPolynomial test(1.0, -1.0, 1.0, -1.0);
		auto roots = test.getRoots();

		assert(roots.size() == 1);
		assert(fabs(roots[0] - 1) <= constants::machine_epsilon);
	}
	void testCubicPolynomial()
	{
		testIsCanonicalInit();
		testIsCanonicalAfterReduce();
		testQDiscriminant();
		testRootCalculation();
	}

	//Interpolation test
	void testLagrangeInterpolation()
	{
		std::vector<double> x_vec = { 0,1 };
		std::vector<double> y_vec = { 0,1 };

		assert(fabs(utilities::LagrangeInterpolation(x_vec, y_vec, 0.5) - 0.5) <= constants::machine_epsilon);
	}

	void startAllTests()
	{
		testCubicPolynomial();
		testLagrangeInterpolation();

		std::cout << "All tests passed!";
	}
}