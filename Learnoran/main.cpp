#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <chrono>

#include "lo_exception.hpp"
#include "polynomial.hpp"
#include "io_helper.hpp"
#include "dataframe.hpp"
#include "linear_model.hpp"

using namespace std;

int main() {
	try {
		IOhelper iohelper("C:\\Users\\oranc\\Documents\\dev\\repos\\Learnoran\\dataset\\train.csv");
		pair<vector<vector<double>>, vector<double>> dataset = iohelper.read_csv(3000);
		Dataframe df(dataset, iohelper.get_csv_header());

		LinearModel predictor;

		const double learning_rate = 0.01;
		const unsigned short epochs = 5;

		chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();
		predictor.fit(df, epochs, learning_rate);
		chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

		cout << "Training done in " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms\n";
	}
	catch (const LearnoranException & exc) {
		const char * err = exc.what();
		cout << err << endl;
	}

	return 0;
}