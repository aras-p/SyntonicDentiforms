#pragma once

#include "../utils/AssertHelper.h"
#include "../utils/Singleton.h"

template< typename T >
class CBundleSingleton : public CSingleton<T> {
private:
	static T* createInstance() { 
		ASSERT_FAIL_MSG( "must be initialized first" );
		return NULL;
	}
	static void deleteInstance( T& instance ) {
		delete &instance;
	}
	friend CSingleton<T>;
};

