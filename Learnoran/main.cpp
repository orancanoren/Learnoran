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
#include "encryption_manager.hpp"
#include "decryption_manager.hpp"

#include "linear_model.hpp"
#include "neural_net.hpp"

#define TRAIN_ENCRYPTED
//#define TRAIN_PLAIN_PREDICT_ENCRYPTED

using namespace std;
using namespace Learnoran;

int random_number() {
	static random_device rd;
	static mt19937 mt(rd());
	static uniform_int_distribution<int> dist(1, 1000);

	return dist(mt);
}

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

Dataframe<EncryptedNumber> * encrypt_dataframe(const Dataframe<double> & df, shared_ptr<EncryptionManager> encryption_manager) {
	const DataframeShape shape = df.shape();

	cout << "Encrypting the dataframe [" << shape.rows << " rows and " << shape.columns << " columns]" << endl;
	chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now(), end;
	Dataframe<EncryptedNumber> * encrypted_dataframe = new Dataframe<EncryptedNumber>(encryption_manager->encrypt_dataframe(df));
	end = chrono::high_resolution_clock::now();
	cout << "Dataframe encrypted in " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms" << endl;

	return encrypted_dataframe;
}

void train_encrypted_model(Predictor & predictor, const Dataframe<EncryptedNumber> & df, const DecryptionManager * dec_man = nullptr, const unsigned short epochs = 2, const double learning_rate = 0.00001) {
	chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();
	predictor.fit(df, epochs, learning_rate, dec_man);
	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
	cout << "Training done [" << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms]" << endl;
}

void train_plaintext_model(Predictor & predictor, const Dataframe<double> & df, const unsigned short epochs = 2, const double learning_rate = 0.00001) {
	chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();
	predictor.fit(df, epochs, learning_rate);
	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
	cout << "Training done [" << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms]" << endl;
}

void test_plain_model(const Predictor & predictor, const unordered_map<string, double> & features) {
	chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();
	const double prediction = predictor.predict(features);
	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

	cout << "Model prediction: " << prediction << " [" << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms]" << endl;
}

void test_encrypted_model(const Predictor & predictor, const unordered_map<string, EncryptedNumber> & features, const DecryptionManager & dec_manager) {
	chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();
	const EncryptedNumber prediction = predictor.predict(features);
	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

	const double plain_prediction = dec_manager.decrypt(prediction);

	cout << "Model prediction: " << plain_prediction << " [" << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms]" << endl;
}

void multiplication_benchmark() {
	EncryptionManager enc_manager;
	vector<EncryptedNumber> ciphertexts;
	EncryptedNumber const_multip_operand = enc_manager.encrypt(random_number());

	double average_time = 0.0;
	unsigned benchmark_size = 1000;

	// initialize ciphertexts
	ciphertexts.resize(benchmark_size);
	for (unsigned i = 0; i < benchmark_size; i++) {
		ciphertexts[i] = enc_manager.encrypt(random_number());
	}

	// perform the benchmark
	chrono::high_resolution_clock::time_point begin, end;
	for (unsigned i = 0; i < benchmark_size; i++) {
		begin = chrono::high_resolution_clock::now();
		EncryptedNumber result = ciphertexts[i] * const_multip_operand;
		end = chrono::high_resolution_clock::now();
		average_time += chrono::duration_cast<chrono::milliseconds>(end - begin).count() / static_cast<double>(benchmark_size);
	}

	cout << "Average multiplication time for " << benchmark_size << " multiplications: " << average_time << " ms" << endl;
}

Dataframe<double> read_dataset(const std::string & csv_file, unsigned num_rows = 0) {
	std::pair < std::vector< std::vector<double>>, std::vector<double>> dataset;
	IOhelper reader;
	reader.open_file(csv_file.c_str());

	dataset = reader.read_csv(num_rows);

	Dataframe<double> dataframe(dataset, reader.get_csv_header());
	return dataframe;
}

int main() {
	try {
		// 1 - READ DATASET
		unsigned dataset_rows = 0;
		std::string csv_file;

		cout << "Enter the directory for the training dataset CSV\n>> ";
		cin >> csv_file;

		cout << "Enter the number of rows in the training dataset (enter 0 if unknown)\n>> ";
		cin >> dataset_rows;

		Dataframe<double> df = read_dataset(csv_file, dataset_rows);

		// 2 - DATASET ENCRYPTION
		shared_ptr<EncryptionManager> encryption_manager = make_shared<EncryptionManager>();
		DecryptionManager decryption_manager(encryption_manager->get_secret_key());

#ifdef TRAIN_ENCRYPTED
		Dataframe<EncryptedNumber> * encrypted_df = encrypt_dataframe(df, encryption_manager);
#endif

		// 3 - MODEL TRAINING
		LinearModel plaintext_linear_model;
		NeuralNetwork plaintext_nn(std::cout, true);
		cout << "\n--- Initiating training benchmarks ---\n" << endl
			 << "1. Training a plaintext model\n" << endl;
		train_plaintext_model(plaintext_linear_model, df);

#ifdef TRAIN_ENCRYPTED
		LinearModel encrypted_linear_model(encryption_manager);

		cout << "\n2. Training an encrypted model\n" << endl;
		train_encrypted_model(encrypted_linear_model, *encrypted_df, &decryption_manager);
#endif

		// 4 - MODEL ACCURACY ASSESSMENT
		cout << "\n--- Initiating model accuracy assessment ---\n" << endl
			<< "1. Plaintext model predictions" << endl;
		
		const double id = 1;
		const double crim = 0.00632;
		const double zn = 18;
		const double indus = 2.31;
		const double chas = 0;
		const double nox = 2.538;
		const double rm = 6.575;
		const double age = 65.2;
		const double dis = 4.09;
		const double rad = 1;
		const double tax = 296;
		const double ptratio = 15.3;
		const double black = 396.9;
		const double lstat = 4.98;

		const unordered_map<string, double> plaintext_features = { { "zn", zn }, { "indus", indus },
		{"chas", chas}, {"nox", nox}, {"rm", rm}, {"age", age}, {"dis", dis}, {"rad", rad}, {"tax", tax},
		{"ptratio", ptratio}, {"black", black}, {"lstat", lstat}, {"ID", id}, {"crim", crim} };
		
		const unordered_map<string, EncryptedNumber> encrypted_features = { { "zn", encryption_manager->encrypt(zn) }, { "indus", encryption_manager->encrypt(indus) },
		{"chas", encryption_manager->encrypt(chas)}, {"nox", encryption_manager->encrypt(nox)}, {"rm",encryption_manager->encrypt(rm)},
		{"age", encryption_manager->encrypt(age)}, {"dis", encryption_manager->encrypt(dis)}, {"rad", encryption_manager->encrypt(rad)}, {"tax", encryption_manager->encrypt(tax)},
		{"ptratio", encryption_manager->encrypt(ptratio)}, {"black", encryption_manager->encrypt(black)}, {"lstat", encryption_manager->encrypt(lstat)},
		{"ID", encryption_manager->encrypt(id)}, {"crim", encryption_manager->encrypt(crim)} };
		
		cout << "Performing prediction on the plaintext model" << endl;
		test_plain_model(plaintext_linear_model, plaintext_features);

#ifdef TRAIN_PLAIN_PREDICT_ENCRYPTED
		linear_model.encrypt_model(encryption_manager);
		cout << "\nPerforming prediction on the encrypted model [training done on plaintexts]" << endl;
		test_encrypted_model(linear_model, encrypted_features, decryption_manager);
#elif defined(TRAIN_ENCRYPTED)
		cout << "\nPerforming prediction on the encrypted model [training done on ciphertexts]" << endl;
		test_encrypted_model(encrypted_linear_model, encrypted_features, decryption_manager);
#endif
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