#ifndef _NEURON_HPP
#define _NEURON_HPP

#include <vector>
#include <cmath>
#include "util.hpp"

// ********************** NEURON_HPP **********************
// Implements the neuron abstraction for feedforward neural networks
// ********************************************************

namespace Learnoran {
	typedef std::vector<Neuron> Layer;

	struct Connection {
		Connection() : weight(0), delta_weight(0) {	}

		double weight;
		double delta_weight;
	};

	class Neuron {
	public:
		Neuron(const unsigned num_outputs, const unsigned self_index) : self_index(self_index) {
			// Args:
			// * num_outputs: number of output neurons this neuron will connect to
			// * self_index: index of the current neuron for the current layer
			outputs.resize(num_outputs);
		}


		void feed_forward(const Layer & previous_layer) {
			// Sum the previous layer's outputs [which are the inputs of the current layer]
			// Include the bias node from the previous layer
			double sum = 0.0;

			for (unsigned i = 0; i < previous_layer.size(); i++) {
				const Neuron & prev_neuron = previous_layer[i];
				sum += prev_neuron.get_output_value() * prev_neuron.outputs[self_index].weight;
			}

			set_output_value(activation_function(sum));
		}

		double activation_function(double input) {
			return std::tanh(input);
		}

		void set_output_value(const double value) {
			output_value = value;
		}

		double get_output_value() const noexcept {
			return output_value;
		}

	private:
		std::vector<Connection> outputs;
		double output_value;
		const unsigned self_index;
	};
}

#endif