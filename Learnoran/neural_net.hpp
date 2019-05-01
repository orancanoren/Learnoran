#ifndef _NEURAL_NET_HPP
#define _NEURAL_NET_HPP

#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>

#include "predictor.hpp"
#include "matrix.hpp"
#include "math_util.hpp"
#include "dataframe.hpp"
#include "decryption_manager.hpp"

namespace Learnoran {
	class NeuralNetwork : public Predictor {
	public:
		NeuralNetwork(std::ostream & info_stream, bool descriptive_info_output = false) 
			: info_stream(info_stream), descriptive_info_output(descriptive_info_output) { }

		void add_layer(const unsigned neurons, std::vector<std::string> * feature_symbols = nullptr) {
			// Adds a new layer to the end of the network
			// Connections to the new layer are initialized with random values
			// from the standard normal distribution
			// Args:
			// - neurons: number of neurons to be contained in the new layer
			// - feature_symbols: identifiers for the input layer neurons in an ordered sequence
			// i.e. [price, size, lifetime] corresponds to first neuron having the identiifer "price" and so on

			if (layers.size() > 0) {
				const unsigned prev_layer_size = layers.at(layers.size() - 1).get_shape().cols;

				connections.push_back(Matrix<double>(prev_layer_size, neurons).map(snd_random));
				gradients.push_back(Matrix<double>(1, neurons));
			}
			else {
				assert(feature_symbols->size() == neurons);

				input_layer_symbols.resize(feature_symbols->size());
				for (unsigned i = 0; i < feature_symbols->size(); i++) {
					input_layer_symbols[i] = feature_symbols->at(i);
				}
			}

			layers.push_back(Matrix<double>(1, neurons));
			biases.push_back(snd_random());
		}

		double predict(const std::unordered_map<std::string, double> & inputs) override {
			// NOTE: currently the NN interface only supports regression problems; for which the NN architecture has only one output layer neuron
			const Matrix<double> outputs = forward_pass(map_to_vector(inputs));
			return outputs[0][0];
		}

		EncryptedNumber predict(const std::unordered_map<std::string, EncryptedNumber> & inputs, const DecryptionManager * dec_man) override  {
			// TODO
			return EncryptedNumber();
		}

		void fit(const Dataframe<double> & dataframe, const unsigned short epochs, const double learning_rate) override {
			// Applies batch gradient descent
			// currently only supports regression problems (i.e. single output neuron)
			assert(layers[layers.size() - 1].get_shape().rows == 1);

			const std::vector<std::vector<double>> & features = dataframe.get_features();
			const std::vector<double> & labels = dataframe.get_labels();

			for (unsigned epoch = 0; epoch < epochs; epoch++) {
				for (unsigned i = 0; i < features.size(); i++) {
					back_propagation(features[i], std::vector<double>{labels[i]}, learning_rate);
				}

				if (epoch % 10 == 0) {
					double average_mse = compute_mean_square_error(Dataframe<double>(features, labels, input_layer_symbols), 100);
					output_error(epoch, epochs, average_mse);
				}
			}

			double final_average_mse = compute_mean_square_error(Dataframe<double>(features, labels, input_layer_symbols), 100);
			output_error(epochs, epochs, final_average_mse);
		}

		void fit(const Dataframe<EncryptedNumber> & dataframe, const unsigned short epochs, const double learning_rate, const DecryptionManager * dec_man) override {
			// TODO
		}

		double compute_mean_square_error(const Dataframe<double> & dataframe, const unsigned num_rows) override {
			const std::vector<std::vector<double>> & features = dataframe.get_features();
			const std::vector<double> & labels = dataframe.get_labels();

			double average_mse = 0.0;
			unsigned total_rows = std::min(static_cast<unsigned>(features.size()), num_rows);

			for (unsigned training_row = 0; training_row < total_rows; training_row++) {
				compute_forward_pass(features[training_row]);
				double prediction = layers[layers.size() - 1][0][0];
				average_mse += mse(prediction, labels[training_row]);
			}

			average_mse /= total_rows;
			return average_mse;
		}
	
		EncryptedNumber compute_mean_square_error(const Dataframe<EncryptedNumber> & dataframe, const unsigned num_rows) override {
			// TODO
			return EncryptedNumber();
		}
	private:
		Matrix<double> forward_pass(const std::vector<double> & inputs) {
			compute_forward_pass(inputs);
			return layers[layers.size() - 1];
		}

		void output_error(const unsigned epoch, const unsigned total_epochs, const double error) {
			if (descriptive_info_output) {
				info_stream << "Epoch " << epoch << '/' << total_epochs << " - MSE for first 100 rows: " << error << '\n';
			}
			else {
				info_stream << epoch << ' ' << error << '\n';
			}
		}

		void compute_forward_pass(const std::unordered_map<std::string, double> & inputs) {
			assert(layers.size() > 1);

			const unsigned num_layers = layers.size();

			// 1 - enter the provided values to the input layer
			fill_input_layer(inputs);

			// 2 - forward propagate hidden layers (use activation function)
			for (unsigned i = 0; i < num_layers - 2; i++) {
				layers.at(i + 1) = layers.at(i).dot(connections.at(i)).map(&apply_bias, biases.at(i)).map(sigmoid);
			}

			// 3 - compute the output layer outputs
			layers.at(num_layers - 1) = layers.at(num_layers - 2).dot(connections.at(num_layers - 2)).map(&apply_bias, biases.at(num_layers - 2));
		}

		void compute_forward_pass(const std::vector<double> & inputs) {
			assert(layers.size() > 1);

			const unsigned num_layers = layers.size();

			// 1 - enter the provided values to the input layer
			fill_input_layer(inputs);

			// 2 - forward propagate hidden layers (use activation function)
			for (unsigned i = 0; i < num_layers - 2; i++) {
				layers.at(i + 1) = layers.at(i).dot(connections.at(i)).map(&apply_bias, biases.at(i)).map(sigmoid);
			}

			// 3 - compute the output layer outputs
			layers.at(num_layers - 1) = layers.at(num_layers - 2).dot(connections.at(num_layers - 2)).map(&apply_bias, biases.at(num_layers - 2));
		}

		std::vector<double> map_to_vector(const std::unordered_map<std::string, double> & map) const {
			assert(input_layer_symbols.size() > 0);

			std::vector<double> vec(map.size());

			for (const std::pair<std::string, double> & variable : map) {
				const std::string & variable_symbol = variable.first;
				const double variable_value = variable.second;

				std::vector<std::string>::const_iterator find_result = std::find(input_layer_symbols.begin(), input_layer_symbols.end(), variable_symbol);

				assert(find_result != input_layer_symbols.end());

				unsigned variable_index = std::distance(input_layer_symbols.cbegin(), find_result);
				vec[variable_index] = variable_value;
			}

			return vec;
		}

		void fill_input_layer(const std::unordered_map<std::string, double> & inputs) {
			assert(inputs.size() == layers[0].get_shape().cols);

			for (unsigned i = 0; i < input_layer_symbols.size(); i++) {
				const std::string & variable_symbol = input_layer_symbols.at(i);
				const double variable_value = inputs.find(variable_symbol)->second; // TODO: this find could fail in case of supplement of wrong inputs arg - fix later
				layers[0][0][i] = variable_value;
			}
		}

		void fill_input_layer(const std::vector<double> & inputs) {
			assert(inputs.size() == layers[0].get_shape().cols);
			
			for (unsigned i = 0; i < inputs.size(); i++) {
				layers[0][0][i] = inputs[i];
			}
		}

		void back_propagation(const std::vector<double> & feature_row, const std::vector<double> target, const double learning_rate) {
			// updates model parameters for a single data point
			// uses MSE loss
			// arg <target> is the target values for the output layer
			assert(layers.size() > 1); // backpropagate only if the network is multi-layer

			const std::vector<double> outputs = forward_pass(feature_row).get_1d_vector();

			// The gradients for the output layer is computed differently than for the hidden layers
			// 1- Compute the gradients for the output layer
			for (unsigned i = 0; i < outputs.size(); i++) {
				gradients[gradients.size() - 1][0][i] = (target[i] - outputs[i]) * sigmoid_prime(outputs[i]);
			}

			// 2- Compute gradients for the hidden layers
			for (int hid_layer_index = layers.size() - 3; hid_layer_index >= 0; hid_layer_index--) {
				const std::vector<double> & current_layer = layers[hid_layer_index + 1].get_1d_vector();

				for (unsigned neuron = 0; neuron < current_layer.size(); neuron++) {
					double derivative = sigmoid_prime(current_layer[neuron]);

					const std::vector<double> weights_cur_neuron = connections[hid_layer_index + 1][neuron];
					double weighted_grad_sum = 0.0;
					for (unsigned edge = 0; edge < weights_cur_neuron.size(); edge++) {
						weighted_grad_sum += weights_cur_neuron[edge] * gradients[hid_layer_index + 1][0][edge];
					}

					gradients[hid_layer_index][0][neuron] = derivative * weighted_grad_sum;
				}
			}

			// 3- Compute deltas and update weights
			for (unsigned hid_layer = 0; hid_layer < connections.size(); hid_layer++) {
				const Matrix<double> current_connections = connections.at(hid_layer);
				const unsigned num_source = current_connections.get_shape().rows;
				const unsigned num_dest = current_connections.get_shape().cols;

				for (unsigned source_neuron = 0; source_neuron < num_source; source_neuron++) {
					for (unsigned dest_neuron = 0; dest_neuron < num_dest; dest_neuron++) {
						const double delta = learning_rate * gradients[hid_layer][0][dest_neuron] * layers[hid_layer][0][source_neuron];

						// update the weight value
						connections[hid_layer][source_neuron][dest_neuron] += delta;
					}
				}
			}
		}

		static double apply_bias(const double x, const double bias) {
			return x + bias;
		}

		// loss functions
		double mse(const double real, const double prediction) {
			const double error = real - prediction;
			return 0.5 * error * error;
		}

		std::vector<double> biases;
		std::vector<Matrix<double>> layers;
		std::vector<Matrix<double>> gradients;
		std::vector<Matrix<double>> connections;
		std::vector<std::string> input_layer_symbols;

		std::ostream & info_stream;
		const bool descriptive_info_output;
	};
}

#endif