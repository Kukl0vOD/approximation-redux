#pragma once

#include <string>

class PlotFileNameFormatter
{
public:
    static std::string pngName(const std::string& name, double temperature);
};
