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

void test_plain_model(Predictor & predictor, const unordered_map<string, double> & features) {
	chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();
	const double prediction = predictor.predict(features);
	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

	cout << "Model prediction: " << prediction << " [" << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms]" << endl;
}

void test_encrypted_model(Predictor & predictor, const unordered_map<string, EncryptedNumber> & features, const DecryptionManager & dec_manager) {
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

Dataframe<EncryptedNumber> * read_enc_dataset(const std::string & csv_file, shared_ptr<EncryptionManager> enc_manager, unsigned num_rows = 0) {
	std::pair < std::vector< std::vector<double>>, std::vector<double>> dataset;
	IOhelper reader;
	reader.open_file(csv_file.c_str());

	dataset = reader.read_csv(num_rows);

	Dataframe<double> dataframe(dataset, reader.get_csv_header());
	Dataframe<EncryptedNumber> * enc_df = encrypt_dataframe(dataframe, enc_manager);
	return enc_df;
}

double plain_neural_network_test(const Dataframe<double> & df, const unordered_map<string, double> test_features) {
	NeuralNetwork nn(std::cout, true);

	nn.add_layer(14, &df.get_feature_headers());
	nn.add_layer(6);
	nn.add_layer(1);

	train_plaintext_model(nn, df, 1000, 0.00001);

	double prediction = nn.predict(test_features);

	return prediction;
}

double plain_linear_regressor_test(const Dataframe<double> & df, const unordered_map<string, double> test_features) {
	LinearModel regressor;

	regressor.fit(df, 1000, 0.00001);

	double prediction = regressor.predict(test_features);

	return prediction;
}

EncryptedNumber encrypted_linear_regressor_test(const Dataframe<EncryptedNumber> & df, const unordered_map<string, EncryptedNumber> test_features, shared_ptr<EncryptionManager> enc_manager, const DecryptionManager * dec_manager) {
	LinearModel regressor(enc_manager);

	regressor.fit(df, 3, 0.00001, dec_manager);

	EncryptedNumber prediction = regressor.predict(test_features, dec_manager);

	return prediction;
}

EncryptedNumber encrypted_linear_regressor_pred_test(const Dataframe<double> & df, const unordered_map<string, EncryptedNumber> test_features, shared_ptr<EncryptionManager> enc_manager, const DecryptionManager * dec_manager) {
	LinearModel regressor;

	regressor.fit(df, 1000, 0.00001);
	regressor.encrypt_model(enc_manager);

	EncryptedNumber prediction = regressor.predict(test_features, dec_manager);

	return prediction;
}

Dataframe<double> read_dataset() {
	unsigned dataset_rows = 0;
	std::string csv_file;
	
	cout << "Enter the directory for the training dataset CSV [for plaintext processing]\n>> ";
	cin >> csv_file;
	
	cout << "Enter the number of rows in the training dataset (enter 0 if unknown)\n>> ";
	cin >> dataset_rows;
	
	return read_dataset(csv_file, dataset_rows);
}

Dataframe<EncryptedNumber> * read_enc_dataset(shared_ptr<EncryptionManager> enc_manager) {
	unsigned dataset_rows = 0;
	std::string csv_file;

	cout << "Enter the directory for the training dataset CSV [for homomorphic processing]\n>> ";
	cin >> csv_file;

	cout << "Enter the number of rows in the training dataset (enter 0 if unknown)\n>> ";
	cin >> dataset_rows;

	return read_enc_dataset(csv_file, enc_manager, dataset_rows);
}

int main() {
	try {
		// 1 - IO
		Dataframe<double> df = read_dataset();

		const unordered_map<string, double> plaintext_features = { { "zn", zn }, { "indus", indus },
			{"chas", chas}, {"nox", nox}, {"rm", rm}, {"age", age}, {"dis", dis}, {"rad", rad}, {"tax", tax},
			{"ptratio", ptratio}, {"black", black}, {"lstat", lstat}, {"ID", id}, {"crim", crim} };

		shared_ptr<EncryptionManager> enc_manager = make_shared<EncryptionManager>();
		DecryptionManager dec_manager(enc_manager->get_secret_key());

		Dataframe<EncryptedNumber> * enc_df = read_enc_dataset(enc_manager);
		const unordered_map<string, EncryptedNumber> encrypted_features = { { "zn", enc_manager->encrypt(zn) }, { "indus", enc_manager->encrypt(indus) },
			{"chas", enc_manager->encrypt(chas) }, {"nox", enc_manager->encrypt(nox) }, {"rm", enc_manager->encrypt(rm) }, {"age", enc_manager->encrypt(age) }, 
			{"dis", enc_manager->encrypt(dis) }, {"rad", enc_manager->encrypt(rad) }, {"tax", enc_manager->encrypt(tax) },
			{"ptratio", enc_manager->encrypt(ptratio) }, {"black", enc_manager->encrypt(black) }, {"lstat", enc_manager->encrypt(lstat) },
			{"ID", enc_manager->encrypt(id) }, {"crim", enc_manager->encrypt(crim) }
		};

		// 2 - Model runs
		cout << "neural network prediction result: " << plain_neural_network_test(df, plaintext_features) << endl;

		cout << "linear regressor prediction result: " << plain_linear_regressor_test(df, plaintext_features) << endl;

		cout << "Training a linear model with encrypted features..." << endl;
		EncryptedNumber linear_reg_enc_pred = encrypted_linear_regressor_test(*enc_df, encrypted_features, enc_manager, &dec_manager);
		double decrypted_pred = dec_manager.decrypt(linear_reg_enc_pred);
		cout << "encrypted linear regressor prediction result: " << decrypted_pred << endl;

		cout << "Training a linear model on plaintext and obtaining ciphertext predictions..." << endl;
		linear_reg_enc_pred = encrypted_linear_regressor_pred_test(df, encrypted_features, enc_manager, &dec_manager);
		decrypted_pred = dec_manager.decrypt(linear_reg_enc_pred);
		cout << "encrypted linear regressor [training on plaintext] result: " << decrypted_pred << endl;
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

//int main() {
//	try {
//		// 1 - READ DATASET
//		unsigned dataset_rows = 0;
//		std::string csv_file;
//
//		cout << "Enter the directory for the training dataset CSV\n>> ";
//		cin >> csv_file;
//
//		cout << "Enter the number of rows in the training dataset (enter 0 if unknown)\n>> ";
//		cin >> dataset_rows;
//
//		Dataframe<double> df = read_dataset(csv_file, dataset_rows);
//
//		// 2 - DATASET ENCRYPTION
//		shared_ptr<EncryptionManager> encryption_manager = make_shared<EncryptionManager>();
//		DecryptionManager decryption_manager(encryption_manager->get_secret_key());
//
//		Dataframe<EncryptedNumber> * encrypted_df = encrypt_dataframe(df, encryption_manager);
//
//		// 3 - MODEL TRAINING
//		LinearModel plaintext_linear_model;
//		NeuralNetwork plaintext_nn(std::cout, true);
//		cout << "\n--- Initiating training benchmarks ---\n" << endl
//			 << "1. Training a plaintext linear regressor\n" << endl;
//		train_plaintext_model(plaintext_linear_model, df);
//
//		cout << "2. Training a plaintext neural network regressor [20 epochs]" << endl;
//		train_plaintext_model(plaintext_nn, df, 20);
//
//		cout << "\n3. Training an encrypted model\n" << endl;
//		LinearModel encrypted_linear_model(encryption_manager);
//		train_encrypted_model(encrypted_linear_model, *encrypted_df, &decryption_manager);
//
//		// 4 - MODEL ACCURACY ASSESSMENT
//		cout << "\n--- Initiating model accuracy assessment ---\n" << endl
//			<< "1. Plaintext model predictions" << endl;
//		
//		const double id = 1;
//		const double crim = 0.00632;
//		const double zn = 18;
//		const double indus = 2.31;
//		const double chas = 0;
//		const double nox = 2.538;
//		const double rm = 6.575;
//		const double age = 65.2;
//		const double dis = 4.09;
//		const double rad = 1;
//		const double tax = 296;
//		const double ptratio = 15.3;
//		const double black = 396.9;
//		const double lstat = 4.98;
//
//		const unordered_map<string, double> plaintext_features = { { "zn", zn }, { "indus", indus },
//		{"chas", chas}, {"nox", nox}, {"rm", rm}, {"age", age}, {"dis", dis}, {"rad", rad}, {"tax", tax},
//		{"ptratio", ptratio}, {"black", black}, {"lstat", lstat}, {"ID", id}, {"crim", crim} };
//		
//		const unordered_map<string, EncryptedNumber> encrypted_features = { { "zn", encryption_manager->encrypt(zn) }, { "indus", encryption_manager->encrypt(indus) },
//		{"chas", encryption_manager->encrypt(chas)}, {"nox", encryption_manager->encrypt(nox)}, {"rm",encryption_manager->encrypt(rm)},
//		{"age", encryption_manager->encrypt(age)}, {"dis", encryption_manager->encrypt(dis)}, {"rad", encryption_manager->encrypt(rad)}, {"tax", encryption_manager->encrypt(tax)},
//		{"ptratio", encryption_manager->encrypt(ptratio)}, {"black", encryption_manager->encrypt(black)}, {"lstat", encryption_manager->encrypt(lstat)},
//		{"ID", encryption_manager->encrypt(id)}, {"crim", encryption_manager->encrypt(crim)} };
//		
//		cout << "Performing prediction on the plaintext model" << endl;
//		test_plain_model(plaintext_linear_model, plaintext_features);
//
//
//		cout << "\nPerforming prediction on the encrypted model [training done on ciphertexts]" << endl;
//		test_encrypted_model(encrypted_linear_model, encrypted_features, decryption_manager);
//
//		return 0;
//	}
//	catch (const LearnoranException & exc) {
//		cerr << "\nLearnoran exception catched - see the description below:" << endl
//			<< exc.what() << endl;
//		return 1;
//	}
//	catch (const runtime_error & exc) {
//		cerr << "\nUnknown runtime error catched - see the description below:" << endl
//			<< exc.what() << endl;
//		return 1;
//	}
//	catch (const exception & exc) {
//		cerr << "\nException catched - see the description below:" << endl
//			<< exc.what() << endl;
//		return 1;
//	}
//	catch (...) {
//		cerr << "\nUnknown exception encountered :(" << endl;
//		return 1;
//	}
//}