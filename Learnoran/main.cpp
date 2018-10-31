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
        string training_dataset_file;
        unsigned training_dataset_rows = 0;
        cout << "Enter the directory for the training dataset CSV\n>> ";
        cin >> training_dataset_file;
		IOhelper iohelper(training_dataset_file.c_str());

        cout << "Enter the number of rows in the training dataset (enter 0 if unknown)\n>> ";
        cin >> training_dataset_rows;

		pair<vector<vector<double>>, vector<double>> dataset = iohelper.read_csv(training_dataset_rows);
		Dataframe df(dataset, iohelper.get_csv_header());

		LinearModel predictor;

		const double learning_rate = 0.00001;
		const unsigned short epochs = 1000;

		chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();
		predictor.fit(df, epochs, learning_rate);
		chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

		cout << "Training done in " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms\n\n";

		// Predict test dataset
        string testing_dataset_file;
        unsigned testing_dataset_rows;
        cout << "Enter the directory for the testing dataset CSV\n>> ";
        cin >> testing_dataset_file;
		iohelper.change_file(testing_dataset_file.c_str());

        cout << "Enter the number of rows in the testing dataset (enter 0 if unknown)\n>> ";
        cin >> testing_dataset_rows;

		pair<vector<vector<double>>, vector<double>> test_dataset = iohelper.read_csv(testing_dataset_rows, ',', false);
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
