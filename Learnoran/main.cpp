#include <iostream>
#include <string>
#include <random>
#include <vector>

#include "sgd_exception.hpp"
#include "polynomial.hpp"
#include "io_helper.hpp"
#include "dataframe.hpp"
#include "linear_model.hpp"

using namespace std;

int main() {
	IOhelper iohelper("C:\\Users\\oranc\\source\\repos\\Learnoran\\Learnoran\\datasets\\boston\\train.csv");
	pair<vector<vector<double>>, vector<double>> dataset = iohelper.read_csv(3000);
	Dataframe df(dataset, iohelper.get_csv_header());

	LinearModel predictor;

	const double learning_rate = 0.01;
	const unsigned short epochs = 5;
	
	predictor.fit(df, epochs, learning_rate);

	return 0;
}