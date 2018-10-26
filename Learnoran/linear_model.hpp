#ifndef _LINEAR_MODEL_HPP
#define _LINEAR_MODEL_HPP

#include <random>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream> // std::cout, only for debug

#include "polynomial.hpp"
#include "dataframe.hpp"

class LinearModel {
public:
	LinearModel() { }

	void fit(const Dataframe & dataframe, const unsigned short epochs, const double learning_rate) {
		initialize_model(dataframe);

		for (unsigned short epoch = 0; epoch < epochs; epoch++) {
			mse_batch_gd(dataframe, learning_rate);
			if (epoch % 10 == 0) {
				std::cout << "loss after epoch " << epoch << ": " << compute_loss(dataframe) << std::endl;
			}
		}
		std::cout << "final loss: " << compute_loss(dataframe) << std::endl;
	}

	double predict(const std::unordered_map<std::string, double> features) const {
		return model(features);
	}

	double predict(const std::initializer_list<std::pair<std::string, double>> features) const {
		std::unordered_map<std::string, double> feature_map;
		
		for (const std::pair<std::string, double> & feature : features) {
			feature_map.insert(feature);
		}

		return predict(feature_map);
	}

private:
	Polynomial model;

	static double random_standard_normal() {
		std::mt19937 generator;
		std::normal_distribution<double> sn_distribution(0, 1);
		return sn_distribution(generator);
	}

	bool symbol_exists(const std::vector<std::string> variable_symbols, const std::string search_symbol) const {
		for (const std::string & symbol : variable_symbols) {
			if (symbol == search_symbol) {
				return true;
			}
		}
		return false;
	}

	void initialize_model(const Dataframe & dataframe) {
		// construct a linear polynomial with random coefficients from the standard normal distribution
		const std::vector<std::string> variable_symbols = dataframe.get_feature_headers();

		for (const std::string & variable : variable_symbols) {
			model.add_term(random_standard_normal(), variable, 1);
		}

		// add the bias term
		model.set_constant_term(random_standard_normal(), "bias");
	}

	void print_model_coefficients(std::ostream & os) {
		const std::unordered_map<std::string, PolynomialTerm> terms = model.get_terms();
		for (const std::pair<std::string, PolynomialTerm> & term : terms) {
			os << term.first << '\t';
		}
		os << '\n';
		for (const std::pair<std::string, PolynomialTerm> & term : terms) {
			os << term.second.coefficient << '\t';
		}
		os << '\n';
	}

	void mse_batch_gd(const Dataframe & dataframe, const double learning_rate) {
		// applies gradient descent to MSE cost function

		DataframeShape shape = dataframe.shape();

		// go over each parameter and optimize them one by one
		for (std::pair<std::string, PolynomialTerm> term : model.get_terms()) {
			const std::string current_parameter = term.first;
			const unsigned current_parameter_exponent = term.second.exponent;

			double derivative_cost_function = 0.0;

			for (unsigned row = 0; row < shape.rows; row++) {
				const double real_value = dataframe.get_row_label(row);
				std::unordered_map<std::string, double> row_features = dataframe.get_row_feature(row);

				const double model_prediction = model(row_features);
				const double model_error = model_prediction - real_value;
				const double inner_derivative = pow(row_features[current_parameter], current_parameter_exponent);
				const double normalizer = 1.0 / shape.rows;

				double current_row_error = normalizer * model_error * inner_derivative;
				derivative_cost_function += current_row_error;
			}

			double current_parameter_value = model[current_parameter];
			double parameter_new_value = current_parameter_value - (learning_rate * derivative_cost_function);


			model[current_parameter] = parameter_new_value;
		}
	}

	double compute_loss(const Dataframe & dataframe) {
		DataframeShape shape = dataframe.shape();

		double loss = 0;
		for (unsigned row = 0; row < shape.rows; row++) {
			const double real_value = dataframe.get_row_label(row);
			std::unordered_map<std::string, double> row_features = dataframe.get_row_feature(row);

			const double model_prediction = model(row_features);
			const double model_error = model_prediction - real_value;
			loss += model_error * model_error;
		}
		loss *= 1.0 / (2 * shape.rows);
		
		return loss;
	}
};

#endif