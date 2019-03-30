#include <iostream>
#include <fstream>
#include <random>
#include <cmath>

#include "neural_net.hpp"
#include "matrix.hpp"
#include "../Learnoran/io_helper.hpp"

using namespace std;

int main() {
	const unsigned EPOCHS = 5;

	Learnoran::IOhelper iohelper;
	iohelper.open_file("../dataset/train.csv");
	pair<vector<vector<double>>, vector<double>> raw_dataset = iohelper.read_csv();

	vector<vector<double>> & features = raw_dataset.first;
	vector<double> & labels = raw_dataset.second;

	NeuralNetwork model;
	model.add_layer(14);
	model.add_layer(6);
	model.add_layer(1);

	model.learn(features, labels, 0.01, 1000);
	
	Matrix<double> result = model.forward_pass(features.at(0));

	cout << "Prediction for the first training row: " << result;

	return 0;
}