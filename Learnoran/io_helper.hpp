#ifndef _IO_HELPER_HPP
#define _IO_HELPER_HPP

#define UNKNOWN 0

#include <seal/seal.h>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <sstream>

#include "lo_exception.hpp"

namespace Learnoran {
	class IOhelper {
	public:
		IOhelper() { }

		void open_file(char const * filename) {
			if (stream.is_open()) {
				stream.close();
			}
			stream.open(filename);
			if (!stream.is_open()) {
				throw CannotOpenFileException();
			}
		}

		std::vector<std::string> get_csv_header() const {
			return csv_header;
		}

		std::pair<std::vector<std::vector<double>>, std::vector<double>> read_csv(const unsigned row_count = UNKNOWN, const char delimiter = ',', bool data_contains_labels = true) {
			// Reads the provided CSV file and returns two vectors, one containing the features and the other containing labels
			// Args:
			// - row_count: number of rows in the dataset - speeds up the operation. If row_count is larger than the 
			// actual number of rows, the dataset size is shrinked to fit to actual size
			// - delimiter: the delimiter to seperate fields in the supplied CSV file
			// - data_contains_labels: if set to false, it is assumed that the data set does not contains labels. Returned labels array will be empty
			// Returns:
			//   A std::pair consisting of features and labels respectively in the format of 2D and 1D arrays.

			std::vector<std::vector<double>> features;
			std::vector<double> labels;

			parse_csv_header(delimiter);

			const size_t column_count = csv_header.size();
			const size_t feature_columns = data_contains_labels ? column_count - 1 : column_count;
			if (row_count != UNKNOWN) {
				features.resize(row_count);
				for (unsigned short i = 0; i < features.size(); i++) {
					features[i].resize(feature_columns);
				}
				if (data_contains_labels) {
					labels.resize(row_count);
				}
			}

			std::string line_buffer;
			unsigned row_counter = 0;

			// To avoid unnecessary branching, pick one of static and dynamic reading functions and apply it in a loop
			if (row_count == UNKNOWN) {
				while (std::getline(stream, line_buffer)) {
					std::istringstream line_stream(line_buffer);
					dynamic_row_append(features, labels, line_stream, delimiter, data_contains_labels);
					row_counter++;
				}
			}
			else {
				while (std::getline(stream, line_buffer)) {
					std::istringstream line_stream(line_buffer);
					static_row_append(features, labels, line_stream, row_counter, delimiter, data_contains_labels);
					row_counter++;
				}
				// row count might be larger than the size of actual data. To prevent future issues, dataset must be shrinked to actual size
				features.resize(row_counter);
				if (data_contains_labels) {
					labels.resize(row_counter);
				}
			}
			// shrink the vector capacity to its size, to reduce the memory usage.
			//This is a good way of reducing the memory usage if there will be no more insertions to the dataset
			features.shrink_to_fit();
			labels.shrink_to_fit();

			return std::make_pair(features, labels);
		}
	
		seal::SecretKey read_secret_key(const char * const filename) {
			stream.close();
			stream.open(filename, std::ios::binary);
			if (!stream.is_open()) {
				throw CannotOpenFileException();
			}

			seal::SecretKey secret_key;
			secret_key.load(stream);

			return secret_key;
		}
	protected:
		void parse_csv_header(const char delimiter = ',') {
			std::vector<std::string> columns;

			stream.seekg(0);
			std::string first_line;
			std::getline(stream, first_line);
			std::istringstream line_stream(first_line);

			std::string column;
			while (std::getline(line_stream, column, delimiter)) {
				columns.push_back(column);
			}

			csv_header = columns;
		}

		void static_row_append(std::vector<std::vector<double>> & features, std::vector<double> & labels, std::istringstream & line_stream, const unsigned short row, const char delimiter = ',', bool populate_labels = true) {
			std::string column_value;
			const size_t feature_columns = populate_labels ? csv_header.size() - 1 : csv_header.size();

			for (unsigned short feature_column = 0; feature_column < feature_columns; feature_column++) {
				std::getline(line_stream, column_value, delimiter);
				features[row][feature_column] = std::strtod(column_value.c_str(), nullptr);
			}

			if (populate_labels) {
				std::getline(line_stream, column_value, delimiter);
				labels[row] = std::strtod(column_value.c_str(), nullptr);
			}
		}

		void dynamic_row_append(std::vector<std::vector<double>> & features, std::vector<double> & labels, std::istringstream & line_stream, const char delimiter = ',', bool populate_labels = true) {
			std::string column_value;
			const size_t feature_columns = populate_labels ? csv_header.size() - 1 : csv_header.size();

			std::vector<double> current_features(feature_columns);
			for (unsigned short feature_column = 0; feature_column < feature_columns; feature_column++) {
				std::getline(line_stream, column_value, delimiter);
				current_features[feature_column] = std::strtod(column_value.c_str(), nullptr);
			}
			features.push_back(current_features);

			if (populate_labels) {
				std::getline(line_stream, column_value, delimiter);
				labels.push_back(std::strtod(column_value.c_str(), nullptr));
			}
		}

		std::fstream stream;
		std::vector<std::string> csv_header;
	};
}

#endif