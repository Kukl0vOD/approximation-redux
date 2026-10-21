#pragma once

#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "application/resource_paths.h"
#include "concentration_extractor.h"
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

inline void plotDefinitionKValuesIntersections(Plotter& plotter)
{
	plotter.plotDefinitionKValuesIntersection(1.0, 15, 0.01, { 0, 1 });
	plotter.plotDefinitionKValuesIntersection(1.0, 40, 0.01, { 2, 3 });
	plotter.plotDefinitionKValuesIntersection(1.0, 90, 0.01, { 4, 5 });
	plotter.plotDefinitionKValuesIntersection(4.1, 450, 0.01, { 6, 7 });
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

inline void plotSixComponentComparativeMolarVolumes(Plotter& plotter)
{
	plotter.plotComparativeMolarVolumes(1.01325, 66.37, 0.01, { 1.01325, 33.691184210526316, 66.36911842105263 }, { 0, 1 });
	plotter.plotComparativeMolarVolumes(11.572381578947368, 96.05, 0.01, { 11.572381578947368, 53.80890789473685, 96.04543421052631 }, { 2, 3 });
	plotter.plotComparativeMolarVolumes(16.85194736842105, 90.77, 0.01, { 16.85194736842105, 53.80890789473685, 90.76586842105264 }, { 4, 5 });
}

inline void plotSixComponentComparativeSpecificVolumes(Plotter& plotter)
{
	plotter.plotComparativeSpecificVolumes(1.01325, 66.37, 0.01, { 1.01325, 33.691184210526316, 66.36911842105263 }, { 0, 1 });
	plotter.plotComparativeSpecificVolumes(11.572381578947368, 96.05, 0.01, { 11.572381578947368, 53.80890789473685, 96.04543421052631 }, { 2, 3 });
	plotter.plotComparativeSpecificVolumes(16.85194736842105, 90.77, 0.01, { 16.85194736842105, 53.80890789473685, 90.76586842105264 }, { 4, 5 });
}

inline void plotSixComponentComparativeKValues(Plotter& plotter)
{
	plotter.plotComparativeKValues(1.01325, 66.37, 0.01, { 1.01325, 33.691184210526316, 66.36911842105263 }, { 0, 1 });
	plotter.plotComparativeKValues(11.572381578947368, 96.05, 0.01, { 11.572381578947368, 53.80890789473685, 96.04543421052631 }, { 2, 3 });
	plotter.plotComparativeKValues(16.85194736842105, 90.77, 0.01, { 16.85194736842105, 53.80890789473685, 90.76586842105264 }, { 4, 5 });
}

inline void plotSixComponentDefinitionKValuesIntersections(Plotter& plotter)
{
	plotter.plotDefinitionKValuesIntersection(1.01325, 66.37, 0.01, { 0, 1 });
	plotter.plotDefinitionKValuesIntersection(11.572381578947368, 96.05, 0.01, { 2, 3 });
	plotter.plotDefinitionKValuesIntersection(16.85194736842105, 90.77, 0.01, { 4, 5 });
}

inline void plotSixComponentMolarVolumeIntersections(Plotter& plotter)
{
	for (const auto scale : { PlotScale::Linear, PlotScale::Logarithmic })
	{
		plotter.plotMolarVolumeIntersection(1.01325, 66.37, 0.01, { 0, 1 }, scale);
		plotter.plotMolarVolumeIntersection(11.572381578947368, 96.05, 0.01, { 2, 3 }, scale);
		plotter.plotMolarVolumeIntersection(16.85194736842105, 90.77, 0.01, { 4, 5 }, scale);
	}
}

inline void plotSixComponentSpecificVolumeIntersections(Plotter& plotter)
{
	for (const auto scale : { PlotScale::Linear, PlotScale::Logarithmic })
	{
		plotter.plotSpecificVolumeIntersection(1.01325, 66.37, 0.01, { 0, 1 }, scale);
		plotter.plotSpecificVolumeIntersection(11.572381578947368, 96.05, 0.01, { 2, 3 }, scale);
		plotter.plotSpecificVolumeIntersection(16.85194736842105, 90.77, 0.01, { 4, 5 }, scale);
	}
}

inline void compareSixComponentVolumeApproximationErrors(Controller& controller)
{
	controller.compareVolumeAndSpecificApprox("rel_error_273.15.json", 1.01325, 66.37, 0.01, { 1.01325, 33.691184210526316, 66.36911842105263 }, { 0, 1 });
	controller.compareVolumeAndSpecificApprox("rel_error_763.15.json", 11.572381578947368, 96.05, 0.01, { 11.572381578947368, 53.80890789473685, 96.04543421052631 }, { 2, 3 });
	controller.compareVolumeAndSpecificApprox("rel_error_773.15.json", 16.85194736842105, 90.77, 0.01, { 16.85194736842105, 53.80890789473685, 90.76586842105264 }, { 4, 5 });
}

inline int runMainCalculations()
{
	Controller controller("solution_data.json");
	controller.setPressureDimension(sol::PressureDimension::BAR);
	controller.setVolumeDimension(sol::VolumeDimension::LITER);

	printSolutionNames(controller);

	Plotter plotter(controller);
	plotComparativeMolarVolumes(plotter);
	plotComparativeSpecificVolumes(plotter);
	plotComparativeKValues(plotter);
	plotDefinitionKValuesIntersections(plotter);
	plotMolarVolumeIntersections(plotter);
	plotSpecificVolumeIntersections(plotter);
	compareVolumeApproximationErrors(controller);

	return 0;
}

inline int runSixComponentCalculations()
{
	Controller controller("6_component_data/solution_data.json");
	controller.setPressureDimension(sol::PressureDimension::BAR);
	controller.setVolumeDimension(sol::VolumeDimension::LITER);
	controller.setOutputSubdirectory("6_component");

	printSolutionNames(controller);

	Plotter plotter(controller, "6_component");
	plotSixComponentComparativeMolarVolumes(plotter);
	plotSixComponentComparativeSpecificVolumes(plotter);
	plotSixComponentComparativeKValues(plotter);
	plotSixComponentDefinitionKValuesIntersections(plotter);
	plotSixComponentMolarVolumeIntersections(plotter);
	plotSixComponentSpecificVolumeIntersections(plotter);
	compareSixComponentVolumeApproximationErrors(controller);

	return 0;
}

inline int runConcentrationExtractor()
{
	std::filesystem::path input_directory =
		app_paths::inputDir() / "6_component_data";
	std::filesystem::path output_directory =
		app_paths::outputDir() / "6_component_concentrations";

	for (int temperature : { 490, 500 })
	{
		std::string suffix = std::to_string(temperature) + "_celsius.json";
		extractor::ConcentrationExtractor concentration_extractor(
			input_directory / "solution.json",
			input_directory / ("molar_fractions_" + suffix),
			input_directory / ("kvalues_" + suffix)
		);

		double temperature_kelvin = temperature + 273.15;
		std::ostringstream output_name;
		output_name << "concentrations_" << std::setprecision(15)
			<< temperature_kelvin << ".json";
		std::filesystem::path output_file = output_directory / output_name.str();
		concentration_extractor.exportToJson(output_file);

		std::cout << "Concentrations exported to " << output_file << '\n';
	}

	return 0;
}

inline int run()
{
	const int main_result = runMainCalculations();
	if (main_result != 0)
	{
		return main_result;
	}

	return runSixComponentCalculations();
}
}
