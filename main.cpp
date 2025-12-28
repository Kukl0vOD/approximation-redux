#include "test.h"
#include "json_builder.h"
#include "controller.h"
#include <fstream>

int main()
{
	Controller controller("solution_data.json", Correlation::GAO, EOSType::PENG_ROBINSON, sol::Phase::GAS);
	controller.setPressureDimension(sol::PressureDimension::BAR);
	controller.setVolumeDimension(sol::VolumeDimension::LITER);
	auto data = controller.getData();
	
	for (size_t i = 0 ;i<data.names.size(); i++)
	{
		controller.setConcentrationState(i);
		controller.calculateVolumeInRange(Controller::generateJsonName(data.states.at(data.names[i]), RequestType::EXACT_MOLAR), 1.0, 10.0, 0.1);
	}

	test::startAllTests();

	return 0;
};