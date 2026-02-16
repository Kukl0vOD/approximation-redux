#include <vector>
#include <cmath>
#include <iostream>

#include "plotter.h"

int main()
{
	Controller controller("solution_data.json");
	controller.setPressureDimension(sol::PressureDimension::BAR);
	controller.setVolumeDimension(sol::VolumeDimension::LITER);

	size_t index = 0;

	for (const auto& name : controller.getData().names)
	{
		std::cout << index << '\t' << name << '\n';
		index++;
	}

	Plotter plt(controller);

	//plt.plotComparativeMolarVolumes(1.0, 4.97, 0.01, { 1.2,3.0,4.8 }, { 0, 1 });
	//plt.plotComparativeMolarVolumes(1.0, 13.8, 0.01, { 1.67,7.74,13.8 }, { 2, 3 });
	//plt.plotComparativeMolarVolumes(1.0, 26.0, 0.01, { 1.1,12.0,26.8 }, { 4, 5 });
	//plt.plotComparativeMolarVolumes(4.1, 38.6, 0.01, { 5.92,22.26,38.6 }, { 6, 7 });

	plt.plotComparativeKValues(1.0, 4.97, 0.01, { 1.2,3.0,4.8 }, { 0, 1 });
	plt.plotComparativeKValues(1.0, 13.8, 0.01, { 1.67,7.74,13.8 }, { 2, 3 });
	plt.plotComparativeKValues(1.0, 26.0, 0.01, { 1.1,12.0,26.8 }, { 4, 5 });
	plt.plotComparativeKValues(4.1, 38.6, 0.01, { 5.92,22.26,38.6 }, { 6, 7 });

	return 0;
}