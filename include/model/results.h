#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct CalculationResult
{
    std::vector<double> pressures;
    std::vector<double> volumes;
};

struct ApproximationResult
{
    CalculationResult gas;
    CalculationResult liquid;
};

struct KValuesResult
{
    std::vector<double> pressures;
    std::unordered_map<std::string, std::vector<double>> kvalues;
};
