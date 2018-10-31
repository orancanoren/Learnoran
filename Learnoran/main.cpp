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
		pair<vector<vector<double>>, vector<double>> dataset = iohelper.read_csv(333);
		Dataframe df(dataset, iohelper.get_csv_header());

		LinearModel predictor;

		const double learning_rate = 0.00001;
		const unsigned short epochs = 1000;

		chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();
		predictor.fit(df, epochs, learning_rate);
		chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

		cout << "Training done in " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms\n\n";

		// Predict test dataset
		iohelper.change_file("C:\\Users\\oranc\\Documents\\dev\\repos\\Learnoran\\dataset\\test.csv");
		pair<vector<vector<double>>, vector<double>> test_dataset = iohelper.read_csv(333, ',', false);
		Dataframe test_df(test_dataset, iohelper.get_csv_header());

		cout << "Predicting the test dataset" << endl;
		for (const string & header : df.get_feature_headers()) {
			cout << header << "\t";
		}
		cout << "PREDICTION" << endl;
		for (unsigned row = 0; row < test_df.shape().rows; row++) {
			vector<double> row_features = df.get_row_feature_array(row);
			for (unsigned col = 0; col < row_features.size(); col++) {
				cout << row_features[col] << "\t";
			}
			cout << predictor.predict(df.get_row_feature(row)) << endl;
		}
	}
	catch (const LearnoranException & exc) {
		cout << "Learnoran Exception encountered" << endl
			<< exc.what() << endl;
	}
	catch (...) {
		cout << "Unknown exception catched!" << endl;
		return 1;
	}

	return 0;
}