#pragma once

#include "solution.h"

#include <cassert>
#include <iostream>

namespace test
{
	namespace component
	{
		void initTest()
		{
			sol::Component test_component("test1", 273.15, 1.0, 0.4884, 23.15);

			assert(test_component.getName() == "test1");
			assert(test_component.getCriticalTemperature() == 273.15);
			assert(test_component.getCriticalPressure() == 1.0);
			assert(test_component.getAccentricFactor() == 0.4884);
			assert(test_component.getMolarMass() == 23.15);
		}

		void setTest()
		{
			sol::Component test_component("test2", 0.0, 0.0, 0.0, 0.0);

			assert(test_component.getName() == "test2");
			assert(test_component.getCriticalTemperature() == 0.0);
			assert(test_component.getCriticalPressure() == 0.0);
			assert(test_component.getAccentricFactor() == 0.0);
			assert(test_component.getMolarMass() == 0.0);

			test_component.setName("test3");
			test_component.setCriticalTemperature(273.15);
			test_component.setCriticalPressure(1.0);
			test_component.setAccentricFactor(0.4884);
			test_component.setMolarMass(23.15);

			assert(test_component.getName() == "test3");
			assert(test_component.getCriticalTemperature() == 273.15);
			assert(test_component.getCriticalPressure() == 1.0);
			assert(test_component.getAccentricFactor() == 0.4884);
			assert(test_component.getMolarMass() == 23.15);
		}

        void test()
        {
            initTest();
            setTest();

            std::cout << "All Component tests passed!" << std::endl;
        }
	}

	void startAllTests()
	{
		component::test();

		std::cout << "All tests passed!";
	}
}