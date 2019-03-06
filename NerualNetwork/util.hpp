#ifndef _UTIL_HPP
#define _UTIL_HPP

#include <random>

namespace Learnoran {
	double random_double(const double lower_bound, const double upper_bound) {
		std::random_device rand_device;
		std::mt19937 generator(rand_device);
		std::uniform_real_distribution<> distribution(lower_bound, upper_bound);

		return distribution(generator);
	}
}

#endif