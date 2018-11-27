#ifndef _BFV_PARAMETERS
#define _BFV_PARAMETERS

#include <stdint.h>

class BFVParameters {
public:
	// Set default parameters
	BFVParameters() : polynomial_modulus_degree(2048), plain_modulus(1 << 8) { }

	BFVParameters(size_t polynomial_modulus_degree, uint64_t plain_modulus)
		: polynomial_modulus_degree(polynomial_modulus_degree), plain_modulus(plain_modulus) { }
	size_t polynomial_modulus_degree;
	uint64_t plain_modulus;
};

class FractionalEncoderParameters {
public:
	FractionalEncoderParameters (const std::size_t & integer_coeff_count = 128, const std::size_t & fraction_coeff_count = 128) 
		: integer_coeff_count(integer_coeff_count), fraction_coeff_count(fraction_coeff_count) { }

	std::size_t integer_coeff_count;
	std::size_t fraction_coeff_count;
};

#endif