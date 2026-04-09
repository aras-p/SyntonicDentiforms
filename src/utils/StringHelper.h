#pragma once

#include <ctype.h>
#include <string>

class CStringHelper {
public:
	static bool isLetter( char c ) { return isalpha(c)?true:false; }
	static bool isDigit( char c ) { return isdigit(c)?true:false; }
	///** checks is string format [whitespace] [sign] [digits] [.digits] [ {d | D | e | E }[sign]digits] */
	//static bool isFloat( const std::string& s );
	///** checks is string format [whitespace] [sign] [digits] */
	//static bool isInt( const std::string& s );
	
	static bool startsWith( const std::string& s, const std::string& subs ) {
		return s.find( subs ) == 0;
	}
	static bool endsWith( const std::string& s, const std::string& subs ) {
		return s.find( subs ) == (s.length() - subs.length());
	}
	static char toLower( char c ) { return tolower(c); }
	static char toUpper( char c ) { return toupper(c); }
	static void toLower( std::string& s );
	static void toUpper( std::string& s );
};

