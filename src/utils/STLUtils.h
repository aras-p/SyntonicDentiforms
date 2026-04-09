#pragma once

namespace stl_utils {

/** Deletes items in pointer container. */
template<class T>
void wipe( T& cont )
{
	for( typename T::iterator it = cont.begin(); it != cont.end(); ++it )
		delete *it;
	cont.clear();
};

/** Deletes values in map (values by pointers). */
template<class T>
void wipe_map( T& cont )
{
	for( typename T::iterator it = cont.begin(); it != cont.end(); ++it )
		delete it->second;
	cont.clear();
};

/** Same as "for_each", but for the whole container at once. */
template<typename _T, typename _O>
void for_all( _T& cont, _O& op )
{
	for( typename _T::iterator it = cont.begin(); it != cont.end(); ++it )
		op( *it );
};

/** Same as "for_each", but for the whole container at once. */
template<typename _T, typename _O>
void for_all( _T const& cont, _O& op )
{
	for( typename _T::const_iterator it = cont.begin(); it != cont.end(); ++it )
		op( *it );
};

}; // namespace stl_utils


