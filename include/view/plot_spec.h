#pragma once

#include "view/plot_config.h"

#include <string>
#include <vector>

enum class PlotLineStyle
{
    Solid,
    Dashed,
    Dotted
};

struct PlotSeries
{
    std::string label;
    std::vector<double> x;
    std::vector<double> y;
    std::string color = "black";
    double line_width = view::plot_config::default_series_line_width;
    PlotLineStyle line_style = PlotLineStyle::Solid;
};

struct PlotTextLabel
{
    std::string text;
    double x = 0.0;
    double y = 0.0;
    double offset_x = view::plot_config::default_label_offset_x;
    double offset_y = view::plot_config::default_label_offset_y;
};

struct PlotPoint
{
    double x = 0.0;
    double y = 0.0;
    std::string color = "black";
    double marker_size = view::plot_config::default_point_marker_size;
};

struct PlotSpec
{
    std::string title;
    std::string output_filename;
    std::string x_label;
    std::string y_label;
    bool log_x = false;
    bool log_y = false;
    bool show_legend = false;
    bool show_title = false;
    int width = view::plot_config::default_width;
    int height = view::plot_config::default_height;
    std::string font = "Arial";
    double base_font_size = view::plot_config::default_base_font_size;
    double axis_font_size = view::plot_config::default_axis_font_size;
    double legend_font_size = view::plot_config::default_legend_font_size;
    double label_font_size = view::plot_config::default_label_font_size;
    std::vector<PlotSeries> series;
    std::vector<PlotPoint> points;
    std::vector<PlotTextLabel> labels;
};
