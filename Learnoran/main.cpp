#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <exception>

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
	try {
		// 1 - READ DATASET
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

		// 2 - DATASET ENCRYPTION

		shared_ptr<EncryptionManager> encryption_manager = make_shared<EncryptionManager>();
		DecryptionManager decryption_manager(encryption_manager->get_secret_key());

		const DataframeShape shape = df.shape();

		cout << "Encrypting the dataframe [" << shape.rows << " rows and " << shape.columns << " columns]" << endl;
		chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now(), end;
		Dataframe<EncryptedNumber> encrypted_dataframe = encryption_manager->encrypt_dataframe(df);
		end = chrono::high_resolution_clock::now();
		cout << "Dataframe encrypted in " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms" << endl;

		// 3 - MODEL TRAINING
		const double learning_rate = 0.00001;
		const unsigned short epochs = 2;
		cout << "\n--- Initiating training benchmarks ---\n" << endl
			<< "1. Training a plaintext model\n" << endl;

		LinearModel plaintext_model;
		begin = chrono::high_resolution_clock::now();
		plaintext_model.fit(df, epochs, learning_rate);
		end = chrono::high_resolution_clock::now();
		cout << "Plaintext model training done [" << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms]" << endl
			<< "\n2. Training an encrypted model\n" << endl;

		LinearModel encrypted_model(encryption_manager);
		begin = chrono::high_resolution_clock::now();
		encrypted_model.fit(encrypted_dataframe, epochs, learning_rate);
		end = chrono::high_resolution_clock::now();
		cout << "\nEncrypted model training done [" << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms]" << endl;

		// 4 - MODEL ACCURACY ASSESSMENT
		cout << "\n--- Initiating model accuracy assessment ---\n" << endl
			<< "1. Plaintext model predictions" << endl;
		
		const double zn_value = -2.2;
		const double indus_value = 5;
		const unordered_map<string, double> plaintext_features = { { "zn", zn_value }, { "indus", indus_value } };
		begin = chrono::high_resolution_clock::now();
		const double plaintext_prediction = plaintext_model.predict(plaintext_features);
		end = chrono::high_resolution_clock::now();

		cout << "Plaintext model prediction [ with features \"zn\": " << zn_value << "," << "\"indus\": " << indus_value << "  ]: " << plaintext_prediction << " [took " 
			<< chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms]" << endl;

		const unordered_map<string, EncryptedNumber> encrypted_features = { { "zn", encryption_manager->encrypt(zn_value) }, { "indus", encryption_manager->encrypt(indus_value) } };
		begin = chrono::high_resolution_clock::now();
		const EncryptedNumber encrypted_prediction = encrypted_model.predict(encrypted_features);
		end = chrono::high_resolution_clock::now();

		// decrypt the prediction
		const double decrypted_prediction = decryption_manager.decrypt(encrypted_prediction);
		cout << "Encrypted model prediction [ with features \"zn\": E(" << zn_value << ")" << "," << "\"indus\": E(" << indus_value << ") ]: " << decrypted_prediction << " [took "
			<< chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms]" << endl;

		// 5 - DATASET DECRYPTION

		cout << "\nDecrypting the dataframe" << endl;
		begin = chrono::high_resolution_clock::now();
		Dataframe<double> decrypted_dataframe = decryption_manager.decrypt_dataframe(encrypted_dataframe);
		end = chrono::high_resolution_clock::now();
		cout << "Dataframe decrypted in " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms" << endl;

		cout << "first two rows of the decrypted dataframe are displayed below." << endl;
		print_dataframe<double>(decrypted_dataframe, 0, 2);
		return 0;
	}
	catch (const LearnoranException & exc) {
		cerr << "\nLearnoran exception catched - see the description below:" << endl
			<< exc.what() << endl;
		return 1;
	}
	catch (const runtime_error & exc) {
		cerr << "\nUnknown runtime error catched - see the description below:" << endl
			<< exc.what() << endl;
		return 1;
	}
	catch (const exception & exc) {
		cerr << "\nException catched - see the description below:" << endl
			<< exc.what() << endl;
		return 1;
	}
	catch (...) {
		cerr << "\nUnknown exception encountered :(" << endl;
		return 1;
	}
}