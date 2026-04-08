#include "Errors.h"


EBaseError::EBaseError( const std::string& msg, const char* fileName, int lineNumber )
:	std::runtime_error( msg ), mFileName( fileName ), mLineNumber( lineNumber )
{
	char buf[100];
	_itoa( lineNumber, buf, 10 );
	mWhereMsg = msg + ' ' + fileName + ':' + buf;
};
