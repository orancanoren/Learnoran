#ifndef _DATAFRAME_HPP
#define _DATAFRAME_HPP

#include <vector>
#include <utility>
#include <string>
#include <iostream> // std::std::cout, for debugging purposes only
#include <unordered_map>

#include "lo_exception.hpp"

namespace Learnoran {
	struct DataframeShape {
		DataframeShape(unsigned rows, unsigned short columns) : rows(rows), columns(columns) { }

		unsigned rows;
		size_t columns;
	};

	template <typename T>
	class Dataframe {
	public:
		// Constructors
		Dataframe(std::pair<std::vector<std::vector<T>>, std::vector<T>> dataset, std::vector<std::string> csv_header)
			: features(dataset.first), labels(dataset.second), columns(csv_header) {}

		Dataframe(std::vector<std::vector<T>> features, std::vector<T> labels, std::vector<std::string> csv_header)
			: features(features), labels(labels), columns(csv_header) { }

		Dataframe(const Dataframe<T> & rhs) {
			features = rhs.features;
			labels = rhs.labels;
			columns = rhs.columns;
		}

		Dataframe(const Dataframe<T> && rhs) {
			features = rhs.features;
			labels = rhs.labels;
			columns = rhs.columns;
		}
		// Accessors
		DataframeShape shape() const {
			// Returns:
			//   A std::pair <# rows, # columns>
			try {
				return DataframeShape(features.size(), features[0].size() + 1);
			}
			catch (DataframeException & exc) {
				std::cout << exc.what();
			}
		}

		const std::vector<T> & get_labels() const {
			return labels;
		}

		const std::vector<std::vector<T>> & get_features() const {
			return features;
		}

		std::unordered_map<std::string, T> get_row_feature(const unsigned index) const {
			const std::vector<T> & values = features[index];
			std::unordered_map<std::string, T> feature_row;

			for (unsigned short index = 0; index < values.size(); index++) {
				feature_row.insert({ columns[index], values[index] });
			}
			return feature_row;
		}

		const T & get_row_label(const unsigned index) const {
			return labels[index];
		}

		const std::vector<T> & get_row_feature_array(const unsigned index) const {
			return features[index];
		}

		void print_interval(unsigned lower_index, unsigned higher_index) const {
			print_header();

			for (unsigned i = lower_index; i < higher_index; i++) {
				for (unsigned j = 0; j < columns.size() - 1; j++) {
					std::cout << features[i][j] << '\t';
				}
				std::cout << labels[i] << '\n';
			}
		}

		std::vector<std::string> get_headers() const {
			return std::vector<std::string>(columns.begin(), columns.end());
		}

		std::vector<std::string> get_feature_headers() const {
			return std::vector<std::string>(columns.begin(), columns.end() - 1);
		}

		std::string get_label_header() const {
			return columns[columns.size() - 1];
		}
	private:
		void print_header() const {
			for (unsigned short i = 0; i < columns.size(); i++) {
				std::cout << columns[i] << '\t';
			}
			std::cout << '\n';
		}

		std::vector<std::vector<T>> features;
		std::vector<T> labels;
		std::vector<std::string> columns;
	};
}

#endif
