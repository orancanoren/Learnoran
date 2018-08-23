#ifndef _IO_HELPER_HPP
#define _IO_HELPER_HPP

#define UNKNOWN 0

#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <sstream>

#include "sgd_exception.hpp"

class IOhelper {
public:
	IOhelper(const std::string & filename) : filename(filename) {
		stream.open(filename);
		if (!stream.is_open()) {
			throw CannotOpenFileException();
		}
	}

	std::vector<std::string> get_csv_header() const {
		return csv_header;
	}

	std::pair<std::vector<std::vector<double>>, std::vector<double>> read_csv(const unsigned row_count = UNKNOWN, const char delimiter = ',') {
		// Assumes the dataset is composed of numerical data only and the labels are residing at the last column!
		// Args:
		// - delimiter: the delimiter to seperate fields in the supplied CSV file
		// Returns:
		//   A std::pair consisting of features and labels respectively in the format of 2D and 1D arrays.

		std::vector<std::vector<double>> features;
		std::vector<double> labels;

		parse_csv_header(delimiter);

		const size_t column_count = csv_header.size();
		if (row_count != UNKNOWN) {
			features.resize(row_count);
			for (unsigned short i = 0; i < features.size(); i++) {
				features[i].resize(column_count - 1);
			}
			labels.resize(row_count);
		}

		std::string line_buffer;
		unsigned row_counter = 0;

		// To avoid unnecessary branching, pick one of static and dynamic reading functions and apply it in a loop
		if (row_count == UNKNOWN) {
			while (std::getline(stream, line_buffer)) {
				std::istringstream line_stream(line_buffer);
				dynamic_row_append(features, labels, line_stream, delimiter);
				row_counter++;
			}
		}
		else {
			while (std::getline(stream, line_buffer)) {
				std::istringstream line_stream(line_buffer);
				static_row_append(features, labels, line_stream, row_counter, delimiter);
				row_counter++;
			}
		}

		return std::make_pair(features, labels);
	}
private:
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

	void static_row_append(std::vector<std::vector<double>> & features, std::vector<double> & labels, std::istringstream & line_stream, const unsigned short row, const char delimiter = ',') {
		std::string column_value;
		for (unsigned short feature_column = 0; feature_column < csv_header.size() - 1; feature_column++) {
			std::getline(line_stream, column_value, delimiter);
			features[row][feature_column] = std::strtod(column_value.c_str(), nullptr);
		}

		std::getline(line_stream, column_value, delimiter);
		labels[row] = std::strtod(column_value.c_str(), nullptr);
	}

	void dynamic_row_append(std::vector<std::vector<double>> & features, std::vector<double> & labels, std::istringstream & line_stream, const char delimiter = ',') {
		std::string column_value;
		std::vector<double> current_row(csv_header.size() - 1);
		for (unsigned short feature_column = 0; feature_column < csv_header.size() - 1; feature_column++) {
			std::getline(line_stream, column_value, delimiter);
			current_row[feature_column] = std::strtod(column_value.c_str(), nullptr);
		}
		features.push_back(current_row);

		std::getline(line_stream, column_value, delimiter);
		labels.push_back(std::strtod(column_value.c_str(), nullptr));
	}

	const std::string & filename;
	std::fstream stream;
	std::vector<std::string> csv_header;
};

#endif