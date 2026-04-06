// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __ANIMATION_CHANNEL_H
#define __ANIMATION_CHANNEL_H

#include "../utils/AbstractNotifier.h"
#include "AnimListener.h"
#include "AnimStream.h"
#include "AnimType.h"


namespace dingus {


// --------------------------------------------------------------------------

class IAnimChannel {
public:
	virtual ~IAnimChannel() = 0 { }

	virtual void perform() = 0;
	virtual eAnimType getType() const = 0;
};


// --------------------------------------------------------------------------

/**
 *  Animation channel.
 *
 *  @param _S Stream type.
 */
template<typename _S>
class CAnimChannel : public IAnimChannel, public CSimpleNotifier< IAnimListener< CAnimChannel<_S> > > {
public:
	typedef	_S					stream_type;
	typedef _S::TSharedPtr		streamptr_type;
	typedef _S::value_type		value_type;
	typedef CAnimChannel<_S>	this_type;
	typedef	IAnimListener<this_type> listener_type;

public:
	CAnimChannel( int numCurves, value_type* dest, int destStride = sizeof(value_type) )
		: mNumCurves(numCurves), mDest(dest), mDestStride(destStride), mStream(0) { }

	int getNumCurves() const { return mNumCurves; }
	void setNumCurves( int numCurves ) { mNumCurves = numCurves; }
	const value_type* getDest() const { return mDest; } // const as there's no point in modifying volatile data anyway
	void setDest( value_type* dest ) { mDest = dest; }
	int getDestStride() const { return mDestStride; }

	virtual void perform() {
		if( mStream.get() != NULL ) {
			// update stream
			mStream->update( mNumCurves, mDest, mDestStride );
			// notify listeners
			TListenerVector::iterator it, itEnd = getListeners().end();
			for( it = getListeners().begin(); it != itEnd; ++it ) {
				listener_type* l = *it;
				assert( l );
				l->onAnimate( *this );
			}
		}
	}

	virtual eAnimType getType() const { return stream_type::getType(); };

	const streamptr_type& getStream() const { return mStream; }
	streamptr_type& getStream() { return mStream; }
	void setStream( streamptr_type const& stream ) { mStream = stream; }
	void reset() { setStream( streamptr_type() ); }

private:
	int				mNumCurves;
	value_type*		mDest;
	int				mDestStride;
	streamptr_type	mStream;
};


// --------------------------------------------------------------------------

typedef CAnimChannel< IAnimStream<SVector3> > CVector3Channel;
typedef CAnimChannel< IAnimStream<SQuaternion> > CQuaternionChannel;
typedef CAnimChannel< IAnimStream<float> > CFloatChannel;


}; // namespace

#endif
