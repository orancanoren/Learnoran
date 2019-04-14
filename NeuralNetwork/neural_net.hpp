#ifndef _NEURAL_NET_HPP
#define _NEURAL_NET_HPP

#include "matrix.hpp"
#include <vector>
#include <random>
#include <iostream>
#include <algorithm>

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
	return exp(-x) / (pow(1 + exp(-x), 2));
}


class NeuralNetwork {
public:
	NeuralNetwork() { }

	void add_layer(const unsigned neurons) {
		// Adds a new layer to the end of the network
		// Connections to the new layer are initialized with random values
		// from the standard normal distribution

		if (layers.size() > 0) {
			const unsigned prev_layer_size = layers.at(layers.size() - 1).get_shape().cols;

			connections.push_back(Matrix<double>(prev_layer_size, neurons).map(snd_random));
			gradients.push_back(Matrix<double>(1, neurons));
		}

		layers.push_back(Matrix<double>(1, neurons));
		biases.push_back(snd_random());
	}

	Matrix<double> forward_pass(const std::vector<double> & inputs) {
		compute_forward_pass(inputs);
		return layers[layers.size() - 1];
	}

	std::ostream & learn(const std::vector<std::vector<double>> & features, const std::vector<double> & labels, const double learning_rate, const unsigned epochs, std::ostream & os, bool descriptive_output = false) {
		// Applies batch gradient descent
		// currently only supports regression problems (i.e. single output neuron)
		assert(layers[layers.size() - 1].get_shape().rows == 1);

		for (unsigned epoch = 0; epoch < epochs; epoch++) {
			for (unsigned i = 0; i < features.size(); i++) {
				back_propagation(features[i], std::vector<double>{labels[i]}, learning_rate);
			}

			if (epoch % 10 == 0) {
				double average_mse = compute_average_mse(features, labels, 100);
				if (descriptive_output) {
					os << "Epoch " << epoch << '/' << epochs << " - MSE for first 100 rows: " << average_mse << '\n';
				}
				else {
					os << epoch << ' ' << average_mse << '\n';
				}
			}
		}

		double final_average_mse = compute_average_mse(features, labels, 100);
		if (descriptive_output) {
			os << "Epoch " << epochs << '/' << epochs << " - MSE for first 100 rows: " << final_average_mse << '\n';
		}
		else {
			os << epochs << ' ' << final_average_mse << '\n';
		}

		return os;
	}

	double compute_accuracy(const std::vector<std::vector<double>> & features, const std::vector<double> & labels) {
		assert(features.size() == labels.size());
		
		return compute_average_mse(features, labels, features.size());
	}

private:
	void compute_forward_pass(const std::vector<double> & inputs) {
		assert(inputs.size() == layers[0].get_shape().cols);
		assert(layers.size() > 1);

		const unsigned num_layers = layers.size();

		// 1 - enter the provided values to the input layer
		for (unsigned i = 0; i < inputs.size(); i++) {
			layers[0][0][i] = inputs[i];
		}

		// 2 - forward propagate hidden layers (use activation function)
		for (unsigned i = 0; i < num_layers - 2; i++) {
			layers.at(i + 1) = layers.at(i).dot(connections.at(i)).map(&apply_bias, biases.at(i)).map(sigmoid);
		}

		// 3 - compute the output layer outputs

		layers.at(num_layers - 1) = layers.at(num_layers - 2).dot(connections.at(num_layers - 2)).map(&apply_bias, biases.at(num_layers - 2));
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

	double compute_average_mse(const std::vector<std::vector<double>> & features, const std::vector<double> & labels, const unsigned num_rows) {
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

	std::vector<double> biases;
	std::vector<Matrix<double>> layers;
	std::vector<Matrix<double>> gradients;
	std::vector<Matrix<double>> connections;
};

#endif