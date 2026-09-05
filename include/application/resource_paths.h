#pragma once

#include <filesystem>
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

    inline std::filesystem::path inputDataDir()
    {
        return resourcesDir() / "data" / "input";
    }

    inline std::filesystem::path outputDataDir()
    {
        return resourcesDir() / "data" / "output";
    }

    inline std::filesystem::path plotsDir()
    {
        return resourcesDir() / "plots";
    }

    inline std::filesystem::path inputDataPath(const std::string& filename)
    {
        const std::filesystem::path path(filename);
        if (path.is_absolute() || std::filesystem::exists(path))
        {
            return path;
        }

        const auto input_path = inputDataDir() / path;
        if (std::filesystem::exists(input_path))
        {
            return input_path;
        }

        return resourcesDir() / "data" / path;
    }

    inline std::filesystem::path outputDataPath(const std::string& filename)
    {
        const std::filesystem::path path(filename);
        if (path.is_absolute() || path.has_parent_path())
        {
            return path;
        }

        return outputDataDir() / path;
    }

    inline std::filesystem::path plotPath(const std::string& filename)
    {
        const std::filesystem::path path(filename);
        if (path.is_absolute() || path.has_parent_path())
        {
            return path;
        }

        return plotsDir() / path;
    }
}
