#ifndef _SGD_EXCEPTION_HPP
#define _SGD_EXCEPTION_HPP

#include <exception>

// MARK: SGDException

class SGDException : public std::exception {
public:
	SGDException(const char * err = "Unknown exception") { }

	const char * what() const {
		return err;
	}
private:
	const char * err;
};

// MARK: Dataframe Exceptions

class DataframeException : public SGDException {
public:
	DataframeException(const char * err = "Unknown Dataframe Exception") : SGDException(err) { }
};

class EmptyDataframeException : public DataframeException {
public:
	EmptyDataframeException() : DataframeException("Dataframe was empty") { }
};

// MARK: IO Exceptions

class IOexception : public SGDException {
public:
	IOexception(const char * err = "Unknown IO Exception") : SGDException(err) { }
};

class CannotOpenFileException : public IOexception {
public:
	CannotOpenFileException() : IOexception("Cannot open the provided file") { }
};

// MARK: Polynomial Exceptions

class PolynomialException : public SGDException {
public:
	PolynomialException(const char * err = "Unknown Polynomial exception") : SGDException(err) { }
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