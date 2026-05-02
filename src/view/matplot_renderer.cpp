#include "view/matplot_renderer.h"

#include <matplot/matplot.h>

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace
{
    std::string lineStyle(PlotLineStyle style)
    {
        switch (style)
        {
        case PlotLineStyle::Solid:
            return "-";
        case PlotLineStyle::Dashed:
            return "--";
        case PlotLineStyle::Dotted:
            return ":";
        default:
            return "-";
        }
    }

    std::string colorPrefix(const std::string& color)
    {
        if (color.empty())
        {
            return "";
        }
        if (color == "black")
        {
            return "k";
        }
        return "";
    }

    std::pair<double, double> dataBounds(const PlotSpec& spec, bool x_axis)
    {
        bool initialized = false;
        double min_value = 0.0;
        double max_value = 0.0;

        for (const auto& series : spec.series)
        {
            const auto& values = x_axis ? series.x : series.y;
            for (const auto value : values)
            {
                if (!initialized)
                {
                    min_value = value;
                    max_value = value;
                    initialized = true;
                }
                else
                {
                    min_value = (std::min)(min_value, value);
                    max_value = (std::max)(max_value, value);
                }
            }
        }

        return { min_value, max_value };
    }

    double applyOffset(double value, double offset, double min_value, double max_value, bool log_scale)
    {
        if (log_scale && min_value > 0.0 && max_value > 0.0)
        {
            const double log_min = std::log10(min_value);
            const double log_max = std::log10(max_value);
            return std::pow(10.0, std::log10(value) + offset * 0.0125 * (log_max - log_min));
        }

        return value + offset * 0.0125 * (max_value - min_value);
    }
}

void MatplotRenderer::render(const PlotSpec& spec) const
{
    if (spec.series.empty())
    {
        throw std::logic_error("Plot spec has no series");
    }

    auto figure = matplot::figure(true);
    figure->size(spec.width, spec.height);
    figure->font(spec.font);
    figure->font_size(static_cast<float>(spec.base_font_size));
    figure->color("white");

    auto axes = figure->current_axes();
    axes->position({ 0.09f, 0.09f, 0.88f, 0.88f });
    axes->font(spec.font);
    axes->font_size(static_cast<float>(spec.axis_font_size));
    axes->xlabel(spec.x_label);
    axes->ylabel(spec.y_label);
    axes->grid(false);
    axes->hold(true);
    axes->box(true);
    axes->box_full(true);
    axes->color("white");
    axes->x_axis().color("black");
    axes->y_axis().color("black");
    axes->x_axis().label_font_size(static_cast<float>(spec.axis_font_size));
    axes->y_axis().label_font_size(static_cast<float>(spec.axis_font_size));
    axes->x_axis().tick_length(0.75f);
    axes->y_axis().tick_length(0.75f);

    if (spec.show_title && !spec.title.empty())
    {
        axes->title(spec.title);
    }

    if (spec.log_x)
    {
        axes->x_axis().scale(matplot::axis_type::axis_scale::log);
    }
    if (spec.log_y)
    {
        axes->y_axis().scale(matplot::axis_type::axis_scale::log);
    }

    axes->run_command("set border 31 front lw 1 lc rgb 'black'");
    axes->run_command("set tics in mirror");
    axes->run_command("set xtics font '" + spec.font + "," + std::to_string(static_cast<int>(spec.axis_font_size)) + "'");
    axes->run_command("set ytics font '" + spec.font + "," + std::to_string(static_cast<int>(spec.axis_font_size)) + "'");
    axes->run_command("unset grid");

    if (spec.log_x)
    {
        axes->run_command("set format x '10^{%L}'");
    }
    if (spec.log_y)
    {
        axes->run_command("set format y '10^{%L}'");
    }

    std::vector<std::string> legend_labels;
    legend_labels.reserve(spec.series.size());

    for (const auto& series : spec.series)
    {
        auto line = axes->plot(series.x, series.y);
        line->line_style(colorPrefix(series.color) + lineStyle(series.line_style));
        line->line_width(static_cast<float>(series.line_width));
        if (!series.color.empty())
        {
            line->color(series.color);
        }
        legend_labels.push_back(series.label);
    }

    auto [x_min, x_max] = dataBounds(spec, true);
    auto [y_min, y_max] = dataBounds(spec, false);

    for (const auto& label : spec.labels)
    {
        const auto x = applyOffset(label.x, label.offset_x, x_min, x_max, spec.log_x);
        const auto y = applyOffset(label.y, label.offset_y, y_min, y_max, spec.log_y);
        auto text = axes->text(x, y, label.text);
        text->font(spec.font);
        text->font_size(static_cast<float>(spec.axis_font_size));
        text->color("black");
    }

    if (spec.show_legend)
    {
        axes->legend(legend_labels);
        if (axes->legend())
        {
            axes->legend()->font_size(static_cast<float>(spec.legend_font_size));
        }
        axes->run_command("set key font '" + spec.font + "," + std::to_string(static_cast<int>(spec.legend_font_size)) + "'");
    }
    else
    {
        matplot::legend(axes, false);
        if (axes->legend())
        {
            axes->legend()->visible(false);
        }
        axes->run_command("unset key");
    }

    if (!figure->save(spec.output_filename))
    {
        throw std::runtime_error("Can't save plot: " + spec.output_filename);
    }
}
