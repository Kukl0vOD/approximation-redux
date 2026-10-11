#pragma once

#include <cstddef>
#include <iostream>

#include "plotter.h"

namespace scenario
{
inline void printSolutionNames(const Controller& controller)
{
	std::size_t index = 0;
	for (const auto& name : controller.getData().names)
	{
		std::cout << index << '\t' << name << '\n';
		index++;
	}
}

inline void plotComparativeMolarVolumes(Plotter& plotter)
{
	plotter.plotComparativeMolarVolumes(1.0, 4.97, 0.01, { 1.2,3.0,4.8 }, { 0, 1 });
	plotter.plotComparativeMolarVolumes(1.0, 13.8, 0.01, { 1.67,7.74,13.8 }, { 2, 3 });
	plotter.plotComparativeMolarVolumes(1.0, 26.0, 0.01, { 1.1,12.0,26.8 }, { 4, 5 });
	plotter.plotComparativeMolarVolumes(4.1, 38.6, 0.01, { 5.92,22.26,38.6 }, { 6, 7 });
}

inline void plotComparativeSpecificVolumes(Plotter& plotter)
{
	plotter.plotComparativeSpecificVolumes(1.0, 4.97, 0.01, { 1.2,3.0,4.8 }, { 0, 1 });
	plotter.plotComparativeSpecificVolumes(1.0, 13.8, 0.01, { 1.67,7.74,13.8 }, { 2, 3 });
	plotter.plotComparativeSpecificVolumes(1.0, 26.0, 0.01, { 1.1,12.0,26.8 }, { 4, 5 });
	plotter.plotComparativeSpecificVolumes(4.1, 38.6, 0.01, { 5.92,22.26,38.6 }, { 6, 7 });
}

inline void plotComparativeKValues(Plotter& plotter)
{
	plotter.plotComparativeKValues(1.0, 4.97, 0.01, { 1.2,3.0,4.8 }, { 0, 1 });
	plotter.plotComparativeKValues(1.0, 13.8, 0.01, { 1.67,7.74,13.8 }, { 2, 3 });
	plotter.plotComparativeKValues(1.0, 26.0, 0.01, { 1.1,12.0,26.8 }, { 4, 5 });
	plotter.plotComparativeKValues(4.1, 38.6, 0.01, { 5.92,22.26,38.6 }, { 6, 7 });
}

inline void plotMolarVolumeIntersections(Plotter& plotter)
{
	for (const auto scale : { PlotScale::Linear, PlotScale::Logarithmic })
	{
		plotter.plotMolarVolumeIntersection(1.0, 15, 0.01, { 0, 1 }, scale);
		plotter.plotMolarVolumeIntersection(1.0, 40, 0.01, { 2, 3 }, scale);
		plotter.plotMolarVolumeIntersection(1.0, 90, 0.01, { 4, 5 }, scale);
		plotter.plotMolarVolumeIntersection(4.1, 120., 0.01, { 6, 7 }, scale);
	}
}

inline void plotSpecificVolumeIntersections(Plotter& plotter)
{
	for (const auto scale : { PlotScale::Linear, PlotScale::Logarithmic })
	{
		plotter.plotSpecificVolumeIntersection(1.0, 15, 0.01, { 0, 1 }, scale);
		plotter.plotSpecificVolumeIntersection(1.0, 40, 0.01, { 2, 3 }, scale);
		plotter.plotSpecificVolumeIntersection(1.0, 150, 0.01, { 4, 5 }, scale);
		plotter.plotSpecificVolumeIntersection(4.1, 450, 0.01, { 6, 7 }, scale);
	}
}

inline void compareVolumeApproximationErrors(Controller& controller)
{
	controller.compareVolumeAndSpecificApprox("rel_error_273.15.json", 1.0, 4.97, 0.01, { 1.2,3.0,4.8 }, { 0, 1 });
	controller.compareVolumeAndSpecificApprox("rel_error_373.15.json", 1.0, 13.8, 0.01, { 1.67,7.74,13.8 }, { 2, 3 });
	controller.compareVolumeAndSpecificApprox("rel_error_473.15.json", 1.0, 26.0, 0.01, { 1.1,12.0,26.8 }, { 4, 5 });
	controller.compareVolumeAndSpecificApprox("rel_error_573.15.json", 4.1, 38.6, 0.01, { 5.92,22.26,38.6 }, { 6, 7 });
}

inline int run()
{
	Controller controller("solution_data.json");
	controller.setPressureDimension(sol::PressureDimension::BAR);
	controller.setVolumeDimension(sol::VolumeDimension::LITER);

	printSolutionNames(controller);

	Plotter plotter(controller);
	plotComparativeMolarVolumes(plotter);
	plotComparativeSpecificVolumes(plotter);
	plotComparativeKValues(plotter);
	plotMolarVolumeIntersections(plotter);
	plotSpecificVolumeIntersections(plotter);
	compareVolumeApproximationErrors(controller);

	return 0;
}
}
