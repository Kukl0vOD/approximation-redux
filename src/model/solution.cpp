#include "solution.h"
#include "model/thermo_calculator.h"

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

	const std::unordered_map <std::string, double>& Solution::getConcentrations() const
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
		auto converted_pressure = utilities::UnitConverter::convert(state.pressure, state.p_dim, sol::PressureDimension::PA);
		auto copy_state = state;
		copy_state.pressure = converted_pressure;

		concentations_ = concentration_callback_(copy_state);
	}

	void Solution::setPhase(Phase phase)
	{
		phase_ = phase;
	}

	void Solution::setPressure(double pressure)
	{
		State new_state = current_state_;
		new_state.pressure = pressure;
		new_state.volume = std::nullopt;
		new_state.specific_volume = std::nullopt;

		setState(new_state);
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

		current_state_.volume = ThermoCalculator::calculateMolarVolume(*eos_, components_, concentations_, bip_, current_state_, phase_);

		return *current_state_.volume;
	}

	double Solution::calculateSpecificVolume()
	{
		if (current_state_.specific_volume)
		{
			return *current_state_.specific_volume;
		}

		current_state_.specific_volume = ThermoCalculator::calculateSpecificVolume(*eos_, components_, concentations_, bip_, current_state_, phase_);

		return *current_state_.specific_volume;
	}
}
