#pragma once

namespace dingus {

/// If o, delete o. Assign to NULL.
template< typename T > void safeDelete( T& o ) {
	if( o ) delete o;
	o = 0;
};
/// If o, delete[] o. Assign to NULL.
template< typename T > void safeDeleteArray( T& o ) {
	if( o ) delete[] o;
	o = 0;
};
/// If o, o->Release(). Assign to NULL.
template< typename T > void safeRelease( T& o ) {
	if( o ) o->Release();
	o = 0;
};


}; // namespace dingus

