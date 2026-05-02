#pragma once

#include "approximation.h"
#include "application/error_analysis_service.h"
#include "model/results.h"
#include "solution_properties.h"

#include <string>
#include <vector>

class JsonResultWriter
{
public:
    static void writeCalculationResult(
        const CalculationResult& result,
        const std::string& output_filename,
        const std::string& value_key
    );

    static void writeKValuesResult(
        const KValuesResult& result,
        const std::vector<sol::Component>& components,
        const std::string& output_filename
    );

    static void writeMolarApproximationResult(
        const ApproximationResult& result,
        const approx::Constants& constants,
        const std::string& output_filename
    );

    static void writeSpecificApproximationResult(
        const ApproximationResult& result,
        const std::string& output_filename
    );

    static void writeErrorAnalysisResult(
        const ErrorAnalysisResult& result,
        const std::string& output_filename
    );
};
