#include "json_parser.h"

namespace json
{
	Parser::Parser(std::ifstream& json_file)
		: solution_data_(Load(json_file))
	{
	}
	std::vector<sol::Component> Parser::parseSolution()
	{
		auto data = solution_data_.GetRoot().AsMap();
		auto solution_it = data.find("Solution");

		if (solution_it == data.end())
		{
			throw ParsingError("Incorect json file");
		}

		auto components = solution_it->second.AsArray();
		std::vector<sol::Component> solution(components.size());

		for (const auto& component : components)
		{
			solution.push_back(parseComponent(component));
		}

		return solution;
	}
	sol::Component Parser::parseComponent(const json::Node& component)
	{
		auto component_map = component.AsMap();

		auto name_it = component_map.find("name");
		auto critical_temperature_it = component_map.find("critical_temperature");
		auto critical_pressure_it = component_map.find("critical_pressure");
		auto accentric_factor_it = component_map.find("accentric_factor");
		auto molar_mass_it = component_map.find("molar_mass");

		if (name_it == component_map.end()
			|| critical_temperature_it == component_map.end()
			|| critical_pressure_it == component_map.end()
			|| accentric_factor_it == component_map.end()
			|| molar_mass_it == component_map.end())
		{
			throw ParsingError("Incorect json file");
		}

		return sol::Component
		{
			name_it->second.AsString(),
			critical_temperature_it->second.AsDouble(),
			critical_pressure_it->second.AsDouble(),
			accentric_factor_it->second.AsDouble(),
			molar_mass_it->second.AsDouble(),
			sol::PressureDimension::PA,
			sol::MolarMassDimension::KG
		};
	}
	ParsedData Parser::parseAll()
	{
		auto data = solution_data_.GetRoot().AsMap();
		auto concentration_it = data.find("Concentrations");

		if (concentration_it == data.end())
		{
			throw ParsingError("Incorect json file");
		}

		auto concentration_map = concentration_it->second;
		auto names = parseConcentrationNames(concentration_map);
		auto states = parseConcentrationState(concentration_map, names);
		auto components = parseSolution();
		auto callbacks = parseConcentrationMap(concentration_map, names, components);

		return ParsedData
		{
			names,
			components,
			states,
			callbacks
		};
	}
	std::deque<std::string> Parser::parseConcentrationNames(const Node& concentration_map)
	{
		auto concentration_data = concentration_map.AsMap();
		auto concentration_names_it = concentration_data.find("concentration_names");

		if (concentration_names_it == concentration_data.end())\
		{
			throw ParsingError("Incorect json file");
		}
		
		auto concentration_names = concentration_names_it->second.AsArray();
		std::deque<std::string> names(concentration_names.size());

		for (const auto& name : concentration_names)
		{
			names.push_back(name.AsString());
		}

		return names;
	}
	std::unordered_map<std::string_view, ConcentrationState> Parser::parseConcentrationState(const Node& concentration_map, const std::deque<std::string>& names)
	{
		auto concentration_data = concentration_map.AsMap();
		auto concetration_state_it = concentration_data.find("concentration_state");

		if (concetration_state_it == concentration_data.end())
		{
			throw ParsingError("Incorrect json file");
		}

		auto concentration_state_map = concetration_state_it->second.AsMap();
		std::unordered_map<std::string_view, ConcentrationState> concentration_states;

		for (const auto& name : names)
		{
			auto state_it = concentration_state_map.find(name);

			if (state_it == concentration_state_map.end())
			{
				throw ParsingError("Incorrect json file");
			}

			auto state = state_it->second.AsMap();
			auto temperature_it = state.find("temperature");
			auto phase_it = state.find("phase");

			if (temperature_it == state.end()
				|| phase_it == state.end())
			{
				throw ParsingError("Incorrect json file");
			}

			auto phase_string = phase_it->second.AsString();
			sol::Phase phase;

			if (phase_string == "gas")
			{
				phase = sol::Phase::GAS;
			}
			else
			{
				phase = sol::Phase::LIQUID;
			}

			concentration_states[name] = ConcentrationState
			{
				temperature_it->second.AsDouble(),
				phase
			};
		}

		return concentration_states;
	}
	std::unordered_map<std::string_view, sol::ConcentrationCallback> Parser::parseConcentrationMap(const Node& concentration_map, const std::deque<std::string>& names, const std::vector<sol::Component>& components)
	{
		auto concentration_data = concentration_map.AsMap();
		std::unordered_map<std::string_view, sol::ConcentrationCallback> callback_map;
		
		std::vector<std::vector<double>> pressures(names.size());

		for (const auto& name : names)
		{
			auto concentrations_it = concentration_data.find(name);

			if (concentrations_it == concentration_data.end())
			{
				throw ParsingError("Incorrect json file");
			}

			auto concentration = concentrations_it->second.AsMap();
			auto pressure_it = concentration.find("P");

			if (pressure_it == concentration.end())
			{
				throw ParsingError("Incorrect json file");
			}

			auto pressure_array = pressure_it->second.AsArray();
			std::vector<double> pressure_vector(pressure_array.size());

			for (const auto& p : pressure_array)
			{
				pressure_vector.push_back(p.AsDouble());
			}

			pressures.push_back(pressure_vector);

			std::vector<std::vector<double>> concentration_matrix(components.size());
		
			for (const auto& component : components)
			{
				auto concentration_vector_it = concentration.find(component.name);

				if (concentration_vector_it == concentration.end())
				{
					throw ParsingError("Incorrect json file");
				}

				auto concentrations_array = concentration_vector_it->second.AsArray();
				std::vector<double> concentration_vector(concentrations_array.size());

				for (const auto& c : concentrations_array)
				{
					concentration_vector.push_back(c.AsDouble());
				}

				concentration_matrix.push_back(concentration_vector);
			}

			auto concentration_callback = [=](const sol::State& state)
				{
					std::vector<double> result(concentration_matrix.size());

					for (const auto& component_concentration : concentration_matrix)
					{
						auto interpolated_pressure = utilities::LagrangeInterpolation(pressure_vector, component_concentration, state.pressure);
						result.push_back(interpolated_pressure);
					}

					return result;
				};
			callback_map[name] = concentration_callback;
		}

		return callback_map;
	}
}
