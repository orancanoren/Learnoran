#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <chrono>
#include <memory>

#include "lo_exception.hpp"
#include "polynomial.hpp"
#include "io_helper.hpp"
#include "dataframe.hpp"
#include "linear_model.hpp"
#include "encryption_manager.hpp"
#include "decryption_manager.hpp"

using namespace std;
using namespace Learnoran;

template <typename T>
void print_dataframe(Dataframe<T> & df, const unsigned begin_index, const unsigned end_index) {
	const DataframeShape & shape = df.shape();

	// print the headers
	for (const string & header : df.get_headers()) {
		cout << header << '\t';
	}
	cout << endl;

	// print the rows
	for (unsigned row = begin_index; row < end_index; row++) {
		const vector<T> & row_features = df.get_row_feature_array(row);
		for (const T & feature : row_features) {
			cout << feature << '\t';
		}

		cout << df.get_row_label(row) << endl;
	}
}

int main() {
	string training_dataset_file;
	IOhelper iohelper;
	unsigned training_dataset_rows = 0;
	cout << "Enter the directory for the training dataset CSV\n>> ";
	cin >> training_dataset_file;
	iohelper.open_file(training_dataset_file.c_str());

	cout << "Enter the number of rows in the training dataset (enter 0 if unknown)\n>> ";
	cin >> training_dataset_rows;

	pair<vector<vector<double>>, vector<double>> dataset = iohelper.read_csv(training_dataset_rows);
	Dataframe<double> df(dataset, iohelper.get_csv_header());

	EncryptionManager encryption_manager;
	DecryptionManager decryption_manager(encryption_manager.get_secret_key());

	const DataframeShape shape = df.shape();
	
	cout << "Encrypting the dataframe [" << shape.rows << " rows and " << shape.columns << " columns]" << endl;
	chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now(), end;
	Dataframe<EncryptedNumber> encrypted_dataframe = encryption_manager.encrypt_dataframe(df);
	end = chrono::high_resolution_clock::now();
	cout << "Dataframe encrypted in " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms" << endl;

	// TRAINING STEP
	const double learning_rate = 0.00001;
	const unsigned short epochs = 1000;
	LinearModel model(encryption_manager);
	cout << "Training the linear model" << endl;
	model.fit(encrypted_dataframe, epochs, learning_rate, encryption_manager.encrypt(0.0));
	cout << "Model trained" << endl;

	cout << "Decrypting the dataframe" << endl;
	begin = chrono::high_resolution_clock::now();
	Dataframe<double> decrypted_dataframe = decryption_manager.decrypt_dataframe(encrypted_dataframe);
	end = chrono::high_resolution_clock::now();
	cout << "Dataframe decrypted in " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms" << endl;

	cout << "first ten rows are displayed below." << endl;
	print_dataframe<double>(decrypted_dataframe, 0, 10);
	return 0;
}

/*
int main() {
	EncryptionManager<double> manager;

	double val1 = 2.3;
	double val2 = 3.72;

	EncryptedNumber c1 = manager.encrypt(val1);
	EncryptedNumber c2 = manager.encrypt(val2);

	EncryptedNumber addition = c1 + c2;

	double dec_res = manager.decrypt(addition);
	cout << dec_res << endl;

	return 0;
}*/

/*int main() {
	try {
		string training_dataset_file;
		IOhelper iohelper;
		unsigned training_dataset_rows = 0;
		cout << "Enter the directory for the training dataset CSV\n>> ";
		cin >> training_dataset_file;
		iohelper.open_file(training_dataset_file.c_str());

		cout << "Enter the number of rows in the training dataset (enter 0 if unknown)\n>> ";
		cin >> training_dataset_rows;

		pair<vector<vector<double>>, vector<double>> dataset = iohelper.read_csv(training_dataset_rows);
		Dataframe df(dataset, iohelper.get_csv_header());

		// Encrypt the data frame

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
		iohelper.open_file(testing_dataset_file.c_str());

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
}*/