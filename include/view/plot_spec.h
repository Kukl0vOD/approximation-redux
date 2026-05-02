#pragma once

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
    double line_width = 2.0;
    PlotLineStyle line_style = PlotLineStyle::Solid;
};

struct PlotTextLabel
{
    std::string text;
    double x = 0.0;
    double y = 0.0;
    double offset_x = -1.0;
    double offset_y = 1.0;
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
    int width = 1280;
    int height = 960;
    std::string font = "Arial";
    double base_font_size = 14.0;
    double axis_font_size = 24.0;
    double legend_font_size = 14.0;
    std::vector<PlotSeries> series;
    std::vector<PlotTextLabel> labels;
};
