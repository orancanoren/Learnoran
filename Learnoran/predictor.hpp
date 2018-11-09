#ifndef _PREDICTOR_HPP
#define _PREDICTOR_HPP

#include <unordered_map>
#include <string>

#include "dataframe.hpp"

namespace Learnoran {
	template <typename T>
	class Predictor {
	public:
		// Predictor::fit -> fits the model parameters to the supplied dataframe such that model error is minimal
		virtual void fit(const Dataframe<T> & dataframe, const unsigned short epochs, const double learning_rate) = 0;

		// Predictor::predict -> performs prediction for the supplied feature row and returns the result
		virtual double predict(const std::unordered_map<std::string, T> features) const = 0;

		// Predictor::compute_mean_square_error -> computes the mean square error of the model for the supplied dataframe
		virtual double compute_mean_square_error(const Dataframe<T> & dataframe) const = 0;
	};
}

#endif