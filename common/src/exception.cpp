/*
	8/14/2013

	Exception class - base class for exceptions.
*/

#include <string>

#include <boost/lexical_cast.hpp>

#include "exception.h"

std::string Exception::getDescription() {
	return "In file '" + mFile + "', Line "
			+ boost::lexical_cast<std::string>(mLine) + ": " + mMessage;
}

std::string Exception::getMessage() {
	return mMessage;
}

const char *Exception::what() {
	return mMessage.c_str();
}

