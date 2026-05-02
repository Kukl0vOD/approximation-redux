#include "model/z_factor_selector.h"

#include <algorithm>
#include <stdexcept>

namespace sol
{
	double selectZFactor(const std::vector<double>& roots, Phase phase)
	{
		if (roots.empty())
		{
			throw std::logic_error("Z-factor roots are empty");
		}

		auto not_negative_comparator = [](double a, double b)
			{
				bool a_valid = (a >= 0);
				bool b_valid = (b >= 0);

				if (a_valid && !b_valid) return true;
				if (!a_valid && b_valid) return false;
				if (!a_valid && !b_valid) return false;

				return a < b;
			};

		switch (phase)
		{
		case Phase::LIQUID:
			return *std::min_element(roots.begin(), roots.end(), not_negative_comparator);
		case Phase::GAS:
			return *std::max_element(roots.begin(), roots.end());
		default:
			throw std::logic_error("Unknown phase");
		}
	}
}
