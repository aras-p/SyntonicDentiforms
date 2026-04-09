#pragma once

namespace stl_utils {

/** Deletes values in map (values by pointers). */
template<class T>
void wipe_map( T& cont )
{
	for( typename T::iterator it = cont.begin(); it != cont.end(); ++it )
		delete it->second;
	cont.clear();
};

}; // namespace stl_utils


