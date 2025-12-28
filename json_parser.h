#pragma once

#include "json.h"
#include "solution.h"
#include <fstream>
#include <deque>

struct ConcentrationState
{
	double temperature;
	sol::Phase phase;
};

struct ParsedData
{
	std::deque<std::string> names;
	std::vector<sol::Component> components;
	std::unordered_map<std::string, ConcentrationState> states;
	std::unordered_map<std::string, sol::ConcentrationCallback> callbacks;
};

namespace json
{
	class Parser
	{
	public:
		Parser(std::ifstream& json_file);

		ParsedData parseAll();

		std::vector<sol::Component> parseSolution(sol::PressureDimension p_dim);
		sol::Component parseComponent(const Node& component, sol::PressureDimension p_dim);

		std::deque<std::string> parseConcentrationNames(const Node& concentration_map);
		std::unordered_map<std::string, ConcentrationState> parseConcentrationState(const Node& concentration_map, const std::deque<std::string>& names);
		std::unordered_map<std::string, sol::ConcentrationCallback> parseConcentrationMap(const Node& concentration_map, const std::deque<std::string>& names, const std::vector<sol::Component>& components, sol::PressureDimension p_dim);

	private:
		Document solution_data_;
	};
}