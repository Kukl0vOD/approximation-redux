#include "view/plot_file_name_formatter.h"

#include <sstream>

std::string PlotFileNameFormatter::pngName(const std::string& name, double temperature)
{
    std::stringstream ss;
    ss << name << "-" << temperature << ".png";
    return ss.str();
}
