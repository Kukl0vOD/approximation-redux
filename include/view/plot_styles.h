#pragma once

#include "view/plot_spec.h"

namespace view
{
    inline PlotSeries exactSeries(std::vector<double> x, std::vector<double> y)
    {
        return { "Exact", std::move(x), std::move(y), "black", 2.0, PlotLineStyle::Solid };
    }

    inline PlotSeries wilsonSeries(std::vector<double> x, std::vector<double> y)
    {
        return { "Wilson", std::move(x), std::move(y), "black", 4.0, PlotLineStyle::Dotted };
    }

    inline PlotSeries approximationSeries(std::vector<double> x, std::vector<double> y)
    {
        return { "Approximation", std::move(x), std::move(y), "black", 6.0, PlotLineStyle::Dashed };
    }

    inline PlotSeries componentSeries(std::string label, std::vector<double> x, std::vector<double> y)
    {
        return { std::move(label), std::move(x), std::move(y), "black", 4.0, PlotLineStyle::Solid };
    }
}
