// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#pragma once

#include <stdexcept>

namespace dingus {

#define THROW_ERROR(msg) { \
	assert(false); \
	throw EBaseError( msg, __FILE__, __LINE__ ); \
}

class EBaseError : public std::runtime_error {
public:
	EBaseError( const std::string& msg, const char* fileName, int lineNumber );

	// override runtime_error::what
	virtual const char* what() const {	return mWhereMsg.c_str(); }

	const char* getFileName() const { return mFileName; }
	int getLineNumber() const { return mLineNumber; }

protected:
	std::string	mWhereMsg;
	const char*	mFileName;
	int			mLineNumber;
};

}; // namespace
