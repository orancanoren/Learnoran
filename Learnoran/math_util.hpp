#ifndef _MATH_UTIL_HPP
#define _MATH_UTIL_HPP

#include <random>

namespace Learnoran {
	double snd_random() {
		std::random_device rd{ };
		std::mt19937 gen{ rd() };

		std::normal_distribution<double> snd(0, 1);

		return snd(gen);
	}

	double snd_random(double x) {
		std::random_device rd{ };
		std::mt19937 gen{ rd() };

		std::normal_distribution<double> snd(0, 1);

		return snd(gen);
	}

	double sigmoid(double x) {
		return 1 / (1 + exp(-x));
	}

	double sigmoid_prime(double x) {
		return exp(-x) / (std::pow(1 + exp(-x), 2));
	}
}

#endif