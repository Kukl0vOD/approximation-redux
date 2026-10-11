#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>

namespace app_paths
{
    inline std::filesystem::path findProjectRoot()
    {
        auto current = std::filesystem::current_path();

        while (!current.empty())
        {
            if (std::filesystem::exists(current / "resources"))
            {
                return current;
            }

            const auto parent = current.parent_path();
            if (parent == current)
            {
                break;
            }
            current = parent;
        }

        return std::filesystem::current_path();
    }

    inline std::filesystem::path resourcesDir()
    {
        return findProjectRoot() / "resources";
    }

    inline std::filesystem::path inputDir()
    {
        return resourcesDir() / "input";
    }

    inline std::filesystem::path outputDir()
    {
        return resourcesDir() / "output";
    }

    inline std::filesystem::path jsonOutputDir()
    {
        return outputDir() / "json";
    }

    inline std::filesystem::path plotsDir()
    {
        return outputDir() / "plots";
    }

    inline std::filesystem::path validateRelativePath(const std::string& filename)
    {
        const auto path = std::filesystem::path(filename).lexically_normal();
        if (path.empty() || path.is_absolute() || *path.begin() == "..")
        {
            throw std::invalid_argument("Resource path must stay inside the configured resource directory: " + filename);
        }

        return path;
    }

    inline std::filesystem::path inputDataPath(const std::string& filename)
    {
        return inputDir() / validateRelativePath(filename);
    }

    inline std::filesystem::path outputDataPath(const std::string& filename)
    {
        return jsonOutputDir() / validateRelativePath(filename);
    }

    inline std::filesystem::path plotPath(const std::string& filename)
    {
        return plotsDir() / validateRelativePath(filename);
    }
}
