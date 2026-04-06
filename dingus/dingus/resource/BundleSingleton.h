// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __BUNDLE_SINGLETON_H
#define __BUNDLE_SINGLETON_H

#include "../utils/Singleton.h"

namespace dingus {

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

}; // namespace

#endif