#pragma once

namespace view::plot_config
{
    inline constexpr int default_width = 1280;
    inline constexpr int default_height = 960;

    inline constexpr double default_series_line_width = 2.0;
    inline constexpr double default_point_marker_size = 10.0;
    inline constexpr double exact_line_width = 2.0;
    inline constexpr double wilson_line_width = 4.0;
    inline constexpr double approximation_line_width = 6.0;
    inline constexpr double component_line_width = 4.0;

    inline constexpr double default_base_font_size = 14.0;
    inline constexpr double k_value_base_font_size = 18.0;
    inline constexpr double default_axis_font_size = 24.0;
    inline constexpr double default_legend_font_size = 14.0;
    inline constexpr double default_label_font_size = 36.0;

    inline constexpr double default_label_offset_x = -1.0;
    inline constexpr double default_label_offset_y = 1.0;
    inline constexpr double first_end_label_offset_x = -3.0;
    inline constexpr double other_end_label_offset_x = -1.0;
    inline constexpr double middle_label_offset_x = 0.5;
    inline constexpr double middle_label_offset_y = 1.0;
    inline constexpr double intersection_label_offset_x = 1.0;
    inline constexpr double intersection_label_offset_y = -3.0;
    inline constexpr double label_overlap_x_threshold = 0.08;
    inline constexpr double label_overlap_y_threshold = 0.08;
    inline constexpr double label_overlap_offset_step_y = 4.0;

    inline constexpr float axes_left = 0.115f;
    inline constexpr float axes_bottom = 0.09f;
    inline constexpr float axes_width = 0.855f;
    inline constexpr float axes_height = 0.88f;
    inline constexpr float axis_tick_length = 0.75f;

    inline constexpr double label_offset_scale = 0.0125;
    inline constexpr double log_scale_base = 10.0;
    inline constexpr int gnuplot_full_border_mask = 31;
    inline constexpr int gnuplot_border_line_width = 1;
}
