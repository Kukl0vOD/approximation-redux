#pragma once

#include "view/plot_spec.h"

class MatplotRenderer
{
public:
    void render(const PlotSpec& spec) const;
};
