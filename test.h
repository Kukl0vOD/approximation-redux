#pragma once

#include "constants.h"
#include "solution.h"
#include "utilities.h"

#include <cassert>
#include <iostream>

namespace test
{
	//CubicPolynomial Test
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

	void startAllTests()
	{
		testCubicPolynomial();

		std::cout << "All tests passed!";
	}
}