#pragma once

#include "constants.h"
#include "solution.h"
#include "utilities.h"
#include "json_parser.h"

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
	void testValueCalculation()
	{
		utilities::CubicPolynomial test(1.0, -1.0, 1.0, -1.0);
		auto value = test.calculatePolynomialValue(1.0);
		assert(value <= constants::machine_epsilon);
	}
	void testCubicPolynomial()
	{
		testIsCanonicalInit();
		testIsCanonicalAfterReduce();
		testQDiscriminant();
		testRootCalculation();
		testValueCalculation();
	}

	//Interpolation test
	void testLagrangeInterpolation()
	{
		std::vector<double> x_vec = { 0,1 };
		std::vector<double> y_vec = { 0,1 };

		assert(fabs(utilities::LagrangeInterpolation(x_vec, y_vec, 0.5) - 0.5) <= constants::machine_epsilon);
	}
	
	//UnitConverter test
	void testUnitConverter()
	{
		auto pressure = utilities::UnitConverter::convert(1.0, sol::PressureDimension::MPA, sol::PressureDimension::BAR);
		assert((pressure - 10.0) <= constants::machine_epsilon);

		auto volume = utilities::UnitConverter::convert(1.0, sol::VolumeDimension::M3, sol::VolumeDimension::LITER);
		assert((volume - 1000.0) <= constants::machine_epsilon);
	}

	//Z-factor test
	void testClearComponentZFactor()
	{
		eos::PengRobinson preos;
		sol::Component decane
		{
			"decane",
			617.7,
			2.103 * 1000000.0,
			0.4884,
			0.14229,
			sol::PressureDimension::PA,
			sol::MolarMassDimension::KG
		};
		sol::State state
		{
		
			0.5 * 1000000.0,
			590.0,
			sol::PressureDimension::PA,
			sol::VolumeDimension::M3,
			sol::SpecificVolumeDimension::LITER_PER_GRAMM
		};

		auto roots = preos.calculateZFactor(decane, state);

		double R = constants::universal_gas_constant;
		double p = state.pressure;
		double t = state.temperature;
		double a = preos.calculateClearComponentA(decane, t);
		double b = preos.calculateClearComponentB(decane);
		double A = a * p / pow(R * t, 2);
		double B = b * p / (R * t);

		utilities::CubicPolynomial equation(1.0, -(1.0 - B), (A - 3.0 * B * B - 2.0 * B), -(A * B - B * B - B * B * B));
		
		for (const auto& root : roots)
		{
			auto equation_value = equation.calculatePolynomialValue(root);
			assert(fabs(equation_value) <= constants::machine_epsilon);
		}
	}

	void startAllTests()
	{
		testCubicPolynomial();
		testLagrangeInterpolation();
		testUnitConverter();
		testClearComponentZFactor();

		std::cout << "All tests passed!";
	}
}