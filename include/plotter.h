#pragma once

#include "controller.h"
#include "view/matplot_renderer.h"

enum class SolType
{
	GAS,
	LIQUID,
	DEFAULT
};

enum class PlotScale
{
	Linear,
	Logarithmic
};

class Plotter
{
public:
	Plotter(const Controller& controller);

	void							plotComparativeMolarVolumes(double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies);
	void							plotComparativeSpecificVolumes(double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies);
	void							plotMolarVolumeIntersection(double pressure_start, double pressure_end, double pressure_inc, std::pair<size_t, size_t> indecies, PlotScale scale);
	void							plotSpecificVolumeIntersection(double pressure_start, double pressure_end, double pressure_inc, std::pair<size_t, size_t> indecies, PlotScale scale);
	void							plotComparativeKValues(double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies);
	void							plotTwoVectors(const std::string& filename, const std::vector<double>& x1, const std::vector<double>& y1, const std::vector<double>& x2, const std::vector<double>& y2, SolType type);
	void							plotVolumeIntersection(const std::string& filename, const std::vector<double>& x1, const std::vector<double>& y1, const std::vector<double>& x2, const std::vector<double>& y2, SolType type, PlotScale scale);
	void							plotThreeVectors(const std::string& title, const std::string& filename, const std::vector<double>& x1, const std::vector<double>& y1, const std::vector<double>& x2, const std::vector<double>& y2, const std::vector<double>& x3, const std::vector<double>& y3);
	void							plotAllVectors(const std::string& title, const std::vector<double>& pressures, const std::unordered_map<std::string, std::vector<double>> k_values);
	static std::string				generatePNGName(const std::string& name, double temperature);

private:
	std::unique_ptr<Controller>		controller_;
	MatplotRenderer					renderer_;
};
