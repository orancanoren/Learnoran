#ifndef _LEARNORAN_EXCEPTION_HPP
#define _LEARNORAN_EXCEPTION_HPP

#include <stdexcept>

// MARK: Learnoran Exception

class LearnoranException : public std::runtime_error {
public:
	LearnoranException(char const * err = "Unknown exception") : std::runtime_error(err) { }
};

// MARK: Dataframe Exceptions

class DataframeException : public LearnoranException {
public:
	DataframeException(char const * err = "Unknown Dataframe Exception") : LearnoranException(err) { }
};

class EmptyDataframeException : public DataframeException {
public:
	EmptyDataframeException() : DataframeException("Dataframe was empty") { }
};

// MARK: IO Exceptions

class IOexception : public LearnoranException {
public:
	IOexception(char const * err = "Unknown IO Exception") : LearnoranException(err) { }
};

class CannotOpenFileException : public IOexception {
public:
	CannotOpenFileException() : IOexception("Cannot open the provided file") { }
};

// MARK: Polynomial Exceptions

class PolynomialException : public LearnoranException {
public:
	PolynomialException(char const * err = "Unknown Polynomial exception") : LearnoranException(err) { }
};

class MissingParametersException : public PolynomialException {
public:
	MissingParametersException() : PolynomialException("Missing parameters supplied, ensure if a value is given for each variable") { }
};

class InvalidVariableException : public PolynomialException {
public:
	InvalidVariableException() : PolynomialException("Provided variable does not exists in the polynomial") { }
};

#endif
