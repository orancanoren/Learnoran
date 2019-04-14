#include <iostream>
#include <fstream>
#include <random>
#include <cmath>

#include "neural_net.hpp"
#include "matrix.hpp"
#include "../Learnoran/io_helper.hpp"

using namespace std;

int main() {
	Learnoran::IOhelper iohelper;
	iohelper.open_file("../dataset/train.csv");
	auto raw_dataset = iohelper.read_csv();

	auto & features = raw_dataset.first;
	auto & labels = raw_dataset.second;

	NeuralNetwork model;
	model.add_layer(14);
	model.add_layer(6);
	model.add_layer(1);

	model.learn(features, labels, 0.01, 5000, cout, true);
	return 0;
}