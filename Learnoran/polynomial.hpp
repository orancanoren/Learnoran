#ifndef _POLYNOMIAL_HPP
#define _POLYNOMIAL_HPP

#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <ostream>

#include "lo_exception.hpp"
#include "encrypted_number.hpp"

namespace Learnoran {
	template <typename T>
	struct PolynomialTerm {
		PolynomialTerm() { }

		PolynomialTerm(T coefficient, unsigned exponent)
			: coefficient(coefficient), exponent(exponent) {}

		T coefficient;
		unsigned exponent;

		PolynomialTerm & operator=(const PolynomialTerm<T> & rhs) {
			this->coefficient = rhs.coefficient;
			this->exponent = rhs.exponent;

			return *this;
		}

		static bool compare_exponents(const PolynomialTerm<T> & lhs, const PolynomialTerm<T> & rhs) {
			return lhs.exponent < rhs.exponent;
		}

		bool operator==(const PolynomialTerm<T> & rhs) const {
			return this->coefficient == rhs.coefficient && this->exponent == rhs.exponent;
		}

		bool operator!=(const PolynomialTerm<T> & rhs) const {
			return !(*this == rhs);
		}
	};

	template <typename T>
	class Polynomial {
	public:
		Polynomial() {}

		// Mutators

		void add_term(T coefficient, std::string variable_symbol, unsigned exponent) {
			terms.insert({ variable_symbol, PolynomialTerm<T>(coefficient, exponent) });
		}

		void set_constant_term(const T & constant_term, const std::string & constant_term_symbol) {
			add_term(constant_term, constant_term_symbol, 0);
			this->constant_term_symbol = constant_term_symbol;
		}

		// Operators

		bool operator==(const Polynomial<T> & rhs) const {
			if (this->terms.size() != rhs.terms.size()) {
				return false;
			}

			if (this->terms != rhs.terms) {
				return false;
			}

			return true;
		}

		T & operator[](const std::string & variable_symbol) {
			// returns the coefficient of the <variable_symbol> term
			typename std::unordered_map<std::string, PolynomialTerm<T>>::iterator find_result = terms.find(variable_symbol);

			if (find_result == terms.cend()) {
				throw InvalidVariableException();
			}

			return find_result->second.coefficient;
		}

		//double operator()(const std::initializer_list<std::pair<std::string, T>> list) {
		//	std::unordered_map<std::string, double> params;
		//	for (const std::pair<std::string, double> param : list) {
		//		params.insert(param);
		//	}

		//	return this->operator()(params);
		//}

		// below function is template-specialized for EncryptedNumber type
		T operator()(const std::unordered_map<std::string, T> & evaluation_parameters, EncryptedNumber encrypted_zero) const;

		// below function is template-specilaized for double type
		T operator()(const std::unordered_map<std::string, T> & evaluation_parameters) const;

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

		const std::unordered_map<std::string, PolynomialTerm<T>> & get_terms() const {
			return terms;
		}

		std::string constant_term_symbol;
	private:
		template <typename K>
		bool check_evaluation_parameters(const std::unordered_map<std::string, K> & evaluation_parameters) const {
			for (typename std::unordered_map<std::string, PolynomialTerm<K>>::const_iterator it = terms.cbegin(); it != terms.cend(); it++) {
				if (it->second.exponent == 0) {
					// no need to check if a value for constant term is provided or not
					continue;
				}
				typename std::unordered_map<std::string, K>::const_iterator find_result = evaluation_parameters.find(it->first);

				if (find_result == evaluation_parameters.cend()) {
					return false;
				}
			}
			return true;
		}

		typename std::unordered_map<std::string, PolynomialTerm<T>> terms;
	};

	// template specialization for EncryptedNumber type
	template <>
	EncryptedNumber Polynomial<EncryptedNumber>::operator()(const std::unordered_map<std::string, EncryptedNumber> & evaluation_parameters, EncryptedNumber encrypted_zero) const {
		// Args:
		// - evaluation_parameters: a std::vector of pairs for which each pair is of the form <value, variable symbol>
		// Returns:
		//   the polynomial evaluated at given parameters
		// Throws:
		// - MissingParametersException: if there exists a missing value for any variable symbol

		if (!check_evaluation_parameters(evaluation_parameters)) {
			throw MissingParametersException();
		}

		EncryptedNumber result = encrypted_zero;
		for (std::unordered_map<std::string, PolynomialTerm<EncryptedNumber>>::const_iterator it = terms.cbegin(); it != terms.cend(); it++) {
			if (it->first == constant_term_symbol) {
				continue;
			}
			EncryptedNumber variable_expression(EncryptedNumber::pow(evaluation_parameters.find(it->first)->second, it->second.exponent));
			result += variable_expression * it->second.coefficient;
		}

		// add the constant term
		if (this->constant_term_symbol != "") {
			const EncryptedNumber constant_term = terms.find(constant_term_symbol)->second.coefficient;
			result += constant_term;
		}
		return result;
	}

	// template specialization for double type
	template <>
	double Polynomial<double>::operator()(const std::unordered_map<std::string, double> & evaluation_parameters) const {
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
		for (auto it = terms.cbegin(); it != terms.cend(); it++) {
			if (it->first == constant_term_symbol) {
				continue;
			}
			result += pow(evaluation_parameters.find(it->first)->second, it->second.exponent) * it->second.coefficient;
		}

		// add the constant term
		if (this->constant_term_symbol != "") {
			const double constant_term = terms.find(constant_term_symbol)->second.coefficient;
			result += constant_term;
		}
		return result;
	}

	template <typename T>
	std::ostream & operator<<(std::ostream & os, const Polynomial<double> & polynomial) {
		unsigned term_index = 0;

		unsigned short term_counter = 0;
		typename std::unordered_map<std::string, PolynomialTerm<T>>::const_iterator it = polynomial.terms.cbegin();
		for (; term_counter < polynomial.terms.size() - 1; it++, term_counter++) {
			const PolynomialTerm<T> & term = it->second;
			os << '(' << term.coefficient << it->first << '^' << term.exponent << ") + ";
		}
		os << '(' << it->second.coefficient << it->first << '^' << it->second.exponent;

		return os;
	}
}

#endif
