#include "solution.h"
#include "constants.h"

namespace sol
{
	Solution::Solution(const std::vector<Component>& components, ConcentrationCallback callback, Correlation correlation, std::unique_ptr<eos::ICubicEOS> eos, const State& current_state)
		: components_(components)
		, concentations_(callback(current_state))
		, concentration_callback_(callback)
		, bip_(eos->calculateBIP(components, correlation, current_state.phase))
		, eos_(std::move(eos))
		, current_state_(current_state)
		, correlation_(correlation)
	{
	}

	const std::vector<Component>& Solution::getComponents() const
	{
		return components_;
	}

	const std::vector<double>& Solution::getConcentrations() const
	{
		return concentations_;
	}

	const Matrix<double>& Solution::getBIP() const
	{
		return bip_;
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
		bip_ = eos_->calculateBIP(components_, correlation, current_state_.phase);
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

	double Solution::calculateVolume()
	{
		if (current_state_.volume)
		{
			return *current_state_.volume;
		}

		auto roots = eos_->calculateZFactor(components_, concentations_, bip_, current_state_);
		auto z_factor = 0.0;

		switch (current_state_.phase)
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

		for (size_t i = 0; i < components_.size(); i++)
		{
			auto molar_mass = utilities::UnitConverter::convert(components_[i].molar_mass, components_[i].mm_dim, sol::MolarMassDimension::KG);
			weight += concentations_[i] * molar_mass;
		}

		current_state_.specific_volume = utilities::UnitConverter::convert(volume / weight, sol::SpecificVolumeDimension::M3_PER_KG, current_state_.sv_dim);

		return *current_state_.specific_volume;
	}
}