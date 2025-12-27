#include "solution.h"
#include "constants.h"

namespace sol
{
	Solution::Solution(const std::vector<Component>& components, ConcentrationCallback callback, Correlation correlation, EOSType eos_type, const State& current_state, Phase phase)
		: components_(components)
		, concentations_(callback(current_state))
		, concentration_callback_(callback)
		, eos_type_(eos_type)
		, eos_(std::move(eos::EOSFactory::createEOS(eos_type)))
		, bip_(eos_->calculateBIP(components, correlation, phase))
		, current_state_(current_state)
		, correlation_(correlation)
		, phase_(phase)
	{
	}

	Solution::Solution(const Solution& other)
		: components_(other.components_)
		, concentations_(other.concentations_)
		, concentration_callback_(other.concentration_callback_)
		, eos_type_(other.eos_type_)
		, eos_(std::move(eos::EOSFactory::createEOS(eos_type_)))
		, bip_(other.bip_)
		, current_state_(other.current_state_)
		, correlation_(other.correlation_)
		, phase_(other.phase_)
	{

	}

	const std::vector<Component>& Solution::getComponents() const
	{
		return components_;
	}

	const std::unordered_map <std::string_view, double>& Solution::getConcentrations() const
	{
		return concentations_;
	}

	const Matrix<double>& Solution::getBIP() const
	{
		return bip_;
	}

	Phase Solution::getPhase() const
	{
		return phase_;
	}

	const State& Solution::getState()
	{
		if (!current_state_.volume)
		{
			current_state_.volume = calculateVolume();
		}
		if (!current_state_.specific_volume)
		{
			current_state_.specific_volume = calculateSpecificVolume();
		}

		return current_state_;
	}

	void Solution::setComponents(const std::vector<Component> components)
	{
		components_ = components;
		setBip(correlation_);
	}

	void Solution::setCallback(const ConcentrationCallback& callback)
	{
		concentration_callback_ = callback;
	}

	void Solution::setBip(Correlation correlation)
	{
		bip_ = eos_->calculateBIP(components_, correlation, phase_);
	}

	void Solution::setEOS(std::unique_ptr<eos::ICubicEOS> eos)
	{
		eos_ = std::move(eos);
	}

	void Solution::setState(const State& state)
	{
		current_state_ = state;
		concentations_ = concentration_callback_(state);
	}

	void Solution::setPhase(Phase phase)
	{
		phase_ = phase;
	}

	void Solution::setPressure(double pressure)
	{
		current_state_.pressure = pressure;
		setState(current_state_);
	}

	void Solution::setPressureDimension(PressureDimension new_dimension)
	{
		current_state_.pressure = utilities::UnitConverter::convert(current_state_.pressure, current_state_.p_dim, new_dimension);
		current_state_.p_dim = new_dimension;
	}

	void Solution::setVolumeDimension(VolumeDimension new_dimension)
	{
		if (current_state_.volume)
		{
			current_state_.volume = utilities::UnitConverter::convert(*current_state_.volume, current_state_.v_dim, new_dimension);
		}

		current_state_.v_dim = new_dimension;
	}

	void Solution::setSpecificVolumeDimension(SpecificVolumeDimension new_dimension)
	{
		if (current_state_.specific_volume)
		{
			current_state_.specific_volume = utilities::UnitConverter::convert(*current_state_.specific_volume, current_state_.sv_dim, new_dimension);
		}

		current_state_.sv_dim = new_dimension;
	}

	void Solution::setMolarMassDimension(MolarMassDimension new_dimension)
	{
		for (auto& component : components_)
		{
			component.molar_mass= utilities::UnitConverter::convert(component.molar_mass, component.mm_dim, new_dimension);
			component.mm_dim = new_dimension;
		}
	}

	double Solution::calculateVolume()
	{
		if (current_state_.volume)
		{
			return *current_state_.volume;
		}

		auto roots = eos_->calculateZFactor(components_, concentations_, bip_, current_state_);
		auto z_factor = 0.0;

		switch (phase_)
		{
		case sol::Phase::LIQUID:
			z_factor = *std::min_element(roots.begin(), roots.end());
			break;
		case sol::Phase::GAS:
			z_factor = *std::max_element(roots.begin(), roots.end());
			break;
		default:
			break;
		}

		auto R = constants::universal_gas_constant;
		auto t = current_state_.temperature;
		auto p = utilities::UnitConverter::convert(current_state_.pressure, current_state_.p_dim, sol::PressureDimension::PA);

		current_state_.volume = utilities::UnitConverter::convert((z_factor * R * t) / p, sol::VolumeDimension::M3, current_state_.v_dim);

		return *current_state_.volume;
	}

	double Solution::calculateSpecificVolume()
	{
		if (current_state_.specific_volume)
		{
			return *current_state_.specific_volume;
		}

		double volume = utilities::UnitConverter::convert(calculateVolume(), current_state_.v_dim, sol::VolumeDimension::M3);
		double weight = 0;

		for (const auto& component : components_)
		{
			auto molar_mass = utilities::UnitConverter::convert(component.molar_mass, component.mm_dim, sol::MolarMassDimension::KG);
			weight += concentations_[component.name] * molar_mass;
		}

		current_state_.specific_volume = utilities::UnitConverter::convert(volume / weight, sol::SpecificVolumeDimension::M3_PER_KG, current_state_.sv_dim);

		return *current_state_.specific_volume;
	}
}