#ifndef _POLYNOMIAL_HPP
#define _POLYNOMIAL_HPP

#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>

#include "sgd_exception.hpp"


struct PolynomialTerm {
	PolynomialTerm() { }

	PolynomialTerm(double coefficient, unsigned exponent)
		: coefficient(coefficient), exponent(exponent) {}

	double coefficient;
	unsigned exponent;

	PolynomialTerm & operator=(const PolynomialTerm & rhs) {
		this->coefficient = rhs.coefficient;
		this->exponent = rhs.exponent;

		return *this;
	}

	static bool compare_exponents(const PolynomialTerm & lhs, const PolynomialTerm & rhs) {
		return lhs.exponent < rhs.exponent;
	}

	bool operator==(const PolynomialTerm & rhs) const {
		return this->coefficient == rhs.coefficient && this->exponent == rhs.exponent;
	}

	bool operator!=(const PolynomialTerm & rhs) const {
		return !(*this == rhs);
	}
};

class Polynomial {
public:
	Polynomial() {}

	// Mutators

	void add_term(double coefficient, std::string variable_symbol, unsigned exponent) {
		terms.insert({ variable_symbol, PolynomialTerm(coefficient, exponent) });
	}

	// Operators

	bool operator==(const Polynomial & rhs) const {
		if (this->terms.size() != rhs.terms.size()) {
			return false;
		}

		if (this->terms != rhs.terms) {
			return false;
		}

		return true;
	}

	double & operator[](const std::string & variable_symbol) {
		// returns the coefficient of the <variable_symbol> term
		std::unordered_map<std::string, PolynomialTerm>::iterator find_result = terms.find(variable_symbol);

		if (find_result == terms.cend()) {
			throw InvalidVariableException();
		}

		return find_result->second.coefficient;
	}

	double operator()(const std::initializer_list<std::pair<std::string, double>> list) {
		std::unordered_map<std::string, double> params;
		for (const std::pair<std::string, double> param : list) {
			params.insert(param);
		}

		return this->operator()(params);
	}

	double operator()(const std::unordered_map<std::string, double> & evaluation_parameters) const {
		// Args:
		// - evaluation_parameters: a std::vector of pairs for which each pair is of the form <value, variable symbol>
		// Returns:
		//   the polynomial evaluated at given parameters
		// Throws:
		// - MissingParametersException: if there exists a missing value for any variable symbol

		if (!check_evaluation_parameters(evaluation_parameters)) {
			throw MissingParametersException();
		}

		double result = 0.0;
		for (std::unordered_map<std::string, PolynomialTerm>::const_iterator it = terms.cbegin(); it != terms.cend(); it++) {
			result += std::pow(evaluation_parameters.find(it->first)->second, it->second.exponent) * it->second.coefficient;
		}

		return result;
	}

	friend std::ostream & operator<<(std::ostream &, const Polynomial &);

	// Accessor

	Polynomial partial_derivative(const std::string & derivative_variable) const {
		Polynomial derived;

		for (auto term : terms) {
			if (term.second.coefficient == 0 || term.first != derivative_variable) {
				continue;
			}

			if (term.second.exponent > 0) {
				derived.add_term(term.second.coefficient * term.second.exponent, derivative_variable, term.second.exponent - 1);
			}
		}

		return derived;
	}

	const std::unordered_map<std::string, PolynomialTerm> & get_terms() const {
		return terms;
	}
private:
	bool check_evaluation_parameters(const std::unordered_map<std::string, double> & evaluation_parameters) const {
		for (std::unordered_map<std::string, PolynomialTerm>::const_iterator it = terms.cbegin(); it != terms.cend(); it++) {
			std::unordered_map<std::string, double>::const_iterator find_result = evaluation_parameters.find(it->first);

			if (find_result == evaluation_parameters.cend()) {
				return false;
			}
		}
		return true;
	}

	std::unordered_map<std::string, PolynomialTerm> terms;
};

std::ostream & operator<<(std::ostream & os, const Polynomial & polynomial) {
	unsigned term_index = 0;

	unsigned short term_counter = 0;
	std::unordered_map<std::string, PolynomialTerm>::const_iterator it = polynomial.terms.cbegin();
	for (; term_counter < polynomial.terms.size() - 1; it++, term_counter++) {
		const PolynomialTerm & term = it->second;
		os << '(' << term.coefficient << it->first << '^' << term.exponent << ") + ";
	}
	os << '(' << it->second.coefficient << it->first << '^' << it->second.exponent;

	return os;
}

#endif