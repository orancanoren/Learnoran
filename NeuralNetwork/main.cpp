#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include "matrix.hpp"
#include "../Learnoran/io_helper.hpp"

using namespace std;

// sample implementation of an ANN

Matrix<double> X, W1, H, W2, Y, B1, B2, Y2, dJdB1, dJdB2, dJdW1, dJdW2;

double random(double x) {
	random_device rd;
	mt19937 mt(rd());
	normal_distribution<> dist{ 5, 2 };

	return dist(mt);
}

double sigmoid(double x) {
	return 1 / (1 + exp(-x));
}

double sigmoidPrime(double x) {
	return exp(-x) / (pow(1 + exp(-x), 2));
}

void init(int inputNeuron, int hiddenNeuron, int outputNeuron) {
	W1 = Matrix<double>(inputNeuron, hiddenNeuron);
	W2 = Matrix<double>(hiddenNeuron, outputNeuron);
	B1 = Matrix<double>(1, hiddenNeuron);
	B2 = Matrix<double>(1, outputNeuron);

	W1 = W1.map(random);
}

Matrix<double> computeOutput(vector<double> input) {
	X = Matrix<double>({ input });
	H = (X.dot(W1) + B1).map(sigmoid);
	Y = (H.dot(W2) + B2).map(sigmoid);

	return Y;
}

void learn(vector<double> expectedOutput, double learningRate) {
	Y2 = Matrix<double>({ expectedOutput });

	// compute gradients
	dJdB2 = (Y - Y2) * ((H.dot(W2) + B2).map(sigmoidPrime));
	dJdB1 = dJdB2.dot(W2.transpose()) * ((X.dot(W1) + B1).map(sigmoidPrime));
	dJdW2 = H.transpose().dot(dJdB2);
	dJdW1 = X.transpose().dot(dJdB1);

	// update weights
	W1 = W1 - (dJdW1 * learningRate);
	W2 = W2 - (dJdW2 * learningRate);
	B1 = B1 - (dJdB1 * learningRate);
	B2 = B2 - (dJdB2 * learningRate);
}

void loadTraining(const char *filename, vector<vector<double> > &input, vector<vector<double> > &output)
{
	int trainingSize = 946;
	input.resize(trainingSize);
	output.resize(trainingSize);

	ifstream file(filename);
	if (file)
	{
		string line;
		int n;

		for (int i = 0; i < trainingSize; i++) // load 946 examples
		{
			for (int h = 0; h < 32; h++) // 'images' are 32*32 pixels
			{
				getline(file, line);
				for (int w = 0; w < 32; w++)
				{
					input[i].push_back(atoi(line.substr(w, 1).c_str()));
				}
			}
			getline(file, line);
			output[i].resize(10); // output is a vector of size 10 because there is 10 categories (0 to 9)
			n = atoi(line.substr(0, 1).c_str()); // get the number that is represented by the array
			output[i][n] = 1; // set value to one at the index that represents the number. Other values are automatically 0 because of the resize()
		}
	}
	file.close();
}

int main() {
	const unsigned EPOCHS = 50;

	Learnoran::IOhelper iohelper;
	iohelper.open_file("../dataset/train.csv");
	pair<vector<vector<double>>, vector<double>> raw_dataset = iohelper.read_csv();

	vector<vector<double>> & features = raw_dataset.first;
	vector<double> & labels = raw_dataset.second;

	init(14, 20, 1);
	
	for (int epoch = 0; epoch < EPOCHS; epoch++) {
		for (int row = 0; row < features.size(); row++) {
			computeOutput(features[row]);
			learn(vector<double>({ labels[row] }), 0.0001);
		}

		cout << "Epoch " << epoch + 1 << " / 50\n";
	}

	iohelper.open_file("../dataset/test.csv");
	pair <vector<vector<double>>, vector<double>> test_dataset = iohelper.read_csv(0, ',', false);

	vector<vector<double>> & test_features = test_dataset.first;
	vector<double> & test_labels = test_dataset.second;

	cout << "STARTING TEST RUNS\n\n";
	for (int row = 0; row < test_features.size(); row++) {
		cout << "row " << row << ": ";
		for (int col = 0; col < test_features[row].size(); col++) {
			cout << test_features[row][col] << ' ';
		}

		Matrix<double> prediction_results = computeOutput(test_features[row]);
		
		cout << " -> " << prediction_results << endl;
	}

	system("pause");

	return 0;
}