#include "model/mixture_properties.h"

#include "utilities.h"

namespace sol
{
	double calculateAverageMolarMass(
		const std::vector<Component>& components,
		const std::unordered_map<std::string, double>& concentrations
	)
	{
		double weight = 0;

		for (const auto& component : components)
		{
			auto molar_mass = utilities::UnitConverter::convert(component.molar_mass, component.mm_dim, MolarMassDimension::KG);
			weight += concentrations.at(component.name) * molar_mass;
		}

		return weight;
	}
}
