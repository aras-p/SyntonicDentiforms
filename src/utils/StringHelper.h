#pragma once

#include <ctype.h>
#include <string>

class CStringHelper {
public:	
	static bool startsWith( const std::string& s, const std::string& subs ) {
		return s.find( subs ) == 0;
	}
	static bool endsWith( const std::string& s, const std::string& subs ) {
		return s.find( subs ) == (s.length() - subs.length());
	}
};

