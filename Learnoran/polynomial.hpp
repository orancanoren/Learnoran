#ifndef _POLYNOMIAL_HPP
#define _POLYNOMIAL_HPP

#define _DEBUG

#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <ostream>

#include "lo_exception.hpp"
#include "encrypted_number.hpp"
#include "decryption_manager.hpp"

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
			this->constant_term = std::make_pair(constant_term_symbol, PolynomialTerm<T>(constant_term, 0));
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

		// below function is template-specialized for EncryptedNumber type - DEBUG: DecryptionManager parameter added, delete afterwards
		EncryptedNumber operator()(const std::unordered_map<std::string, T> & evaluation_parameters, const EncryptedNumber & encrypted_zero, const DecryptionManager * dec_man = nullptr) const {
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

			int noise_budget = 0;
			int mid_noise_budget = 0;
			if (dec_man != nullptr) {
				noise_budget = dec_man->get_noise_budget_bits(result);
			}

#ifdef _DEBUG
			bool decryption_manager_valid = dec_man != nullptr;
			if (!decryption_manager_valid) {
				std::cout << "Decryption interface not supplied to polynomial evaluation operator\nnoise budget calculations will not be supported!\n";
			}
			else {
				std::cout << "initial noise budget for result ciphertext: " << noise_budget << std::endl;
			}
#endif

			for (std::unordered_map<std::string, PolynomialTerm<EncryptedNumber>>::const_iterator term = terms.cbegin(); term != terms.cend(); term++) {
				const EncryptedNumber & variable_value = evaluation_parameters.find(term->first)->second;
				const EncryptedNumber power = pow(variable_value, term->second.exponent);
				if (dec_man != nullptr) {
					mid_noise_budget = dec_man->get_noise_budget_bits(power);
					mid_noise_budget = dec_man->get_noise_budget_bits(term->second.coefficient);
				}
				result += term->second.coefficient * power;
				
				if (dec_man != nullptr) {
					noise_budget = dec_man->get_noise_budget_bits(result);
				}
#ifdef _DEBUG
				if (decryption_manager_valid) {
					std::cout << "mid noise budget for result ciphertext: " << noise_budget << std::endl;
				}
#endif
			}

			// add the constant term
			if (this->constant_term_symbol != "") {
				const EncryptedNumber & constant_term = this->constant_term.second.coefficient;
				result += constant_term;
			}
			return result;
		}

		// below function is template-specilaized for double type
		double operator()(const std::unordered_map<std::string, T> & evaluation_parameters) const {
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
			for (std::unordered_map<std::string, PolynomialTerm<double>>::const_iterator term = terms.cbegin(); term != terms.cend(); term++) {
				const double & variable_value = evaluation_parameters.find(term->first)->second;
				result += std::pow(variable_value, term->second.exponent) * term->second.coefficient;
			}

			// add the constant term
			if (this->constant_term_symbol != "") {
				const double & constant_term = this->constant_term.second.coefficient;
				result += constant_term;
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

		const std::unordered_map<std::string, PolynomialTerm<T>> & get_terms() const {
			return terms;
		}

		const std::pair<std::string, PolynomialTerm<T>> & get_constant_term() const {
			return constant_term;
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
		typename std::pair<std::string, PolynomialTerm<T>> constant_term;
	};

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
