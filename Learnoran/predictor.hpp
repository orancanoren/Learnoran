#ifndef _PREDICTOR_HPP
#define _PREDICTOR_HPP

#include <unordered_map>
#include <string>

#include "dataframe.hpp"
#include "encrypted_number.hpp"

namespace Learnoran {
	class Predictor {
	public:
		// Predictor::fit -> fits the model parameters to the supplied dataframe such that model error is minimal
		virtual void fit(const Dataframe<double> & dataframe, const unsigned short epochs, const double learning_rate) = 0;

		virtual void fit(const Dataframe<EncryptedNumber> & dataframe, const unsigned short epochs, const double learning_rate) = 0;

		// Predictor::predict -> performs prediction for the supplied feature row and returns the result
		virtual double predict(const std::unordered_map<std::string, double> & features) const = 0;

		virtual EncryptedNumber predict(const std::unordered_map<std::string, EncryptedNumber> & features, DecryptionManager * dec_manager = nullptr) const = 0;

		// Predictor::compute_mean_square_error -> computes the mean square error of the model for the supplied dataframe
		virtual EncryptedNumber compute_mean_square_error(const Dataframe<EncryptedNumber> & dataframe) const = 0;

		virtual double compute_mean_square_error(const Dataframe<double> & dataframe) const = 0;
	};
}

#endif