#include "view/matplot_renderer.h"

#include "application/resource_paths.h"

#include <matplot/matplot.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <stdexcept>

namespace
{
    class ScopedCurrentPath
    {
    public:
        explicit ScopedCurrentPath(const std::filesystem::path& path)
            : original_path_(std::filesystem::current_path())
        {
            std::filesystem::current_path(path);
        }

        ~ScopedCurrentPath()
        {
            std::filesystem::current_path(original_path_);
        }

    private:
        std::filesystem::path original_path_;
    };

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
            return std::pow(view::plot_config::log_scale_base, std::log10(value) + offset * view::plot_config::label_offset_scale * (log_max - log_min));
        }

        return value + offset * view::plot_config::label_offset_scale * (max_value - min_value);
    }
}

void MatplotRenderer::render(const PlotSpec& spec) const
{
    if (spec.series.empty())
    {
        throw std::logic_error("Plot spec has no series");
    }

    const auto output_path = app_paths::plotPath(spec.output_filename);
    std::filesystem::create_directories(output_path.parent_path());
    const ScopedCurrentPath scoped_path(output_path.parent_path());

    auto figure = matplot::figure(true);
    figure->size(spec.width, spec.height);
    figure->font(spec.font);
    figure->font_size(static_cast<float>(spec.base_font_size));
    figure->color("white");

    auto axes = figure->current_axes();
    axes->position({
        view::plot_config::axes_left,
        view::plot_config::axes_bottom,
        view::plot_config::axes_width,
        view::plot_config::axes_height
    });
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
    axes->x_axis().tick_length(view::plot_config::axis_tick_length);
    axes->y_axis().tick_length(view::plot_config::axis_tick_length);

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

    axes->run_command(
        "set border " + std::to_string(view::plot_config::gnuplot_full_border_mask) +
        " front lw " + std::to_string(view::plot_config::gnuplot_border_line_width) +
        " lc rgb 'black'");
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

    for (const auto& point : spec.points)
    {
        auto marker = axes->plot(std::vector<double>{ point.x }, std::vector<double>{ point.y }, "o");
        marker->marker_size(static_cast<float>(point.marker_size));
        marker->marker_face(true);
        marker->color(point.color);
        marker->marker_color(point.color);
    }

    auto [x_min, x_max] = dataBounds(spec, true);
    auto [y_min, y_max] = dataBounds(spec, false);

    for (const auto& label : spec.labels)
    {
        const auto x = applyOffset(label.x, label.offset_x, x_min, x_max, spec.log_x);
        const auto y = applyOffset(label.y, label.offset_y, y_min, y_max, spec.log_y);
        auto text = axes->text(x, y, label.text);
        text->font(spec.font);
        text->font_size(static_cast<float>(spec.label_font_size));
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

    if (!figure->save(output_path.filename().string()))
    {
        throw std::runtime_error("Can't save plot: " + output_path.string());
    }
}
