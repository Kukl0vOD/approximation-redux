#include "application/json_result_writer.h"

#include "json_builder.h"

#include <fstream>

namespace
{
    json::Array toJsonArray(const std::vector<double>& values)
    {
        json::Array array;
        for (const auto value : values)
        {
            array.push_back(value);
        }
        return array;
    }
}

void JsonResultWriter::writeCalculationResult(
    const CalculationResult& result,
    const std::string& output_filename,
    const std::string& value_key
)
{
    std::ofstream json_out(output_filename);

    auto output_doc = json::Document(json::Builder{}
        .StartDict()
            .Key("P")
                .StartArray()
                    .Value(toJsonArray(result.pressures))
                .EndArray()
            .Key(value_key)
                .StartArray()
                    .Value(toJsonArray(result.volumes))
                .EndArray()
        .EndDict().Build());

    json::Print(output_doc, json_out);
}

void JsonResultWriter::writeKValuesResult(
    const KValuesResult& result,
    const std::vector<sol::Component>& components,
    const std::string& output_filename
)
{
    std::ofstream json_out(output_filename);

    json::Builder builder;
    builder.StartDict()
        .Key("P")
        .StartArray();

    for (const auto pressure : result.pressures)
    {
        builder.Value(pressure);
    }

    builder.EndArray();

    for (const auto& component : components)
    {
        builder.Key(component.name).StartArray();
        for (const auto kvalue : result.kvalues.at(component.name))
        {
            builder.Value(kvalue);
        }
        builder.EndArray();
    }

    auto output_doc = json::Document(builder.EndDict().Build());
    json::Print(output_doc, json_out);
}

void JsonResultWriter::writeMolarApproximationResult(
    const ApproximationResult& result,
    const approx::Constants& constants,
    const std::string& output_filename
)
{
    std::ofstream json_out(output_filename);

    auto output_doc = json::Document(json::Builder{}
        .StartDict()
            .Key("Constants")
                .StartDict()
                    .Key("alpha").Value(constants.alpha)
                    .Key("beta").Value(constants.beta)
                    .Key("bg").Value(constants.bg)
                    .Key("bl").Value(constants.bl)
                    .Key("p*").Value(constants.pextra)
                .EndDict()
            .Key("P")
                .StartArray()
                    .Value(toJsonArray(result.gas.pressures))
                .EndArray()
            .Key("V_Gas")
                .StartArray()
                    .Value(toJsonArray(result.gas.volumes))
                .EndArray()
            .Key("V_Liquid")
                .StartArray()
                    .Value(toJsonArray(result.liquid.volumes))
                .EndArray()
        .EndDict().Build());

    json::Print(output_doc, json_out);
}

void JsonResultWriter::writeSpecificApproximationResult(
    const ApproximationResult& result,
    const std::string& output_filename
)
{
    std::ofstream json_out(output_filename);

    auto output_doc = json::Document(json::Builder{}
        .StartDict()
            .Key("P")
                .StartArray()
                    .Value(toJsonArray(result.gas.pressures))
                .EndArray()
            .Key("SV_Gas")
                .StartArray()
                    .Value(toJsonArray(result.gas.volumes))
                .EndArray()
            .Key("SV_Liquid")
                .StartArray()
                    .Value(toJsonArray(result.liquid.volumes))
                .EndArray()
        .EndDict().Build());

    json::Print(output_doc, json_out);
}

void JsonResultWriter::writeErrorAnalysisResult(
    const ErrorAnalysisResult& result,
    const std::string& output_filename
)
{
    std::ofstream json_out(output_filename);

    auto output_doc = json::Document(json::Builder{}
        .StartDict()
        .Key("P")
        .StartArray()
        .Value(toJsonArray(result.pressures))
        .EndArray()
        .Key("gas_molar_error")
        .StartArray()
        .Value(toJsonArray(result.gas_molar_error))
        .EndArray()
        .Key("gas_spec_error")
        .StartArray()
        .Value(toJsonArray(result.gas_specific_error))
        .EndArray()
        .Key("liquid_molar_error")
        .StartArray()
        .Value(toJsonArray(result.liquid_molar_error))
        .EndArray()
        .Key("liquid_spec_error")
        .StartArray()
        .Value(toJsonArray(result.liquid_specific_error))
        .EndArray()
        .Key("max_gas_molar_err")
        .Value(result.max_gas_molar_error)
        .Key("max_liquid_molar_err")
        .Value(result.max_liquid_molar_error)
        .Key("max_gas_spec_err")
        .Value(result.max_gas_specific_error)
        .Key("max_liquid_spec_err")
        .Value(result.max_liquid_specific_error)
        .EndDict().Build());

    json::Print(output_doc, json_out);
}
