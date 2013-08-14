/*
	8/14/2013

	Exception class - base class for exceptions.

	This class is meant to be extended into different sub-classes of Exception
	types, such that exceptions can be caught and handled in the appropriate
	place via polymorphism.

	When extending this class, the only thing that needs to be done is pass the
	arguments of the child class's constructor on to the base class constructor.
	getDescription(), getMessage(), and what() are already implemented and
	should not need to be modified.

	Use the THROW_EXCEPT macro instead of throw ExceptionType(), because it
	automatically adds the appropriate file and line number to the constructor.
*/

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>

/**
	THROW_EXCEPT macro

	Use this to throw exceptions instead of throw ExceptionType(). It will
	fill in the file and line number for the Exception constructor.

	type is the class name of the Exception type to throw
	msg is the message to pass into the Exception
*/
#define THROW_EXCEPT(type, msg) throw type(msg, __FILE__, __LINE__)

class Exception : public std::exception {
	public:
		Exception(const Exception &e);
		Exception(const std::string &msg, const std::string &file, int line);
		~Exception() throw() { }

		/**
			Get a full text description of the error.
			Contains file ane line number that threw the error, and the error message.

			Example:
				In file '/projects/main.cpp', Line 50: An exception occurred.
		*/
		std::string getDescription();

		/**
			Retrieve the error message, without file and line number.
		*/
		std::string getMessage();

		/**
			Returns a const char* version of the error message.
			Inherited from std::exception
		*/
		virtual const char *what();

	private:
		std::string mMessage, // Error message
		            mFile;    // File where the Exception originated from
		int         mLine;    // Line number that threw the error
};

#endif

