#pragma once

#include "view/plot_spec.h"

namespace view
{
    inline PlotSeries exactSeries(std::vector<double> x, std::vector<double> y)
    {
        return { "Exact", std::move(x), std::move(y), "black", plot_config::exact_line_width, PlotLineStyle::Solid };
    }

    inline PlotSeries wilsonSeries(std::vector<double> x, std::vector<double> y)
    {
        return { "Wilson", std::move(x), std::move(y), "black", plot_config::wilson_line_width, PlotLineStyle::Dotted };
    }

    inline PlotSeries approximationSeries(std::vector<double> x, std::vector<double> y)
    {
        return { "Approximation", std::move(x), std::move(y), "black", plot_config::approximation_line_width, PlotLineStyle::Dashed };
    }

    inline PlotSeries componentSeries(std::string label, std::vector<double> x, std::vector<double> y)
    {
        return { std::move(label), std::move(x), std::move(y), "black", plot_config::component_line_width, PlotLineStyle::Solid };
    }
}
