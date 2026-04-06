// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __ANIM_STREAM_MIXER_H_
#define __ANIM_STREAM_MIXER_H_

#include "AnimStream.h"

namespace dingus {


/**
 *  Animation stream weighted mixer that is a stream itself.
 *
 *  Weight-blends any number of animation streams. Can optionally auto-remove
 *  ended streams.
 *
 *  @param _S Stream type. Must implement IAnimStream and have value_type, TSharedPtr subtypes.
 *  @param _DEL Should remove ended streams?
 */
template<typename _S, bool _DEL = true>
class CAnimStreamMixer : public IAnimStream<_S::value_type> {
public:
	typedef _S							stream_type;
	typedef _S::TSharedPtr				streamptr_type;
	typedef _S::value_type				value_type;
	typedef CAnimStreamMixer<_S,_DEL>	this_type;
	typedef intrusive_ptr<this_type>	TSharedPtr;

public:
	// ----------------------------------------------------------------------
	/**
	 *  Stream data.
	 *  Consists of stream ID, the stream and stream weight.
	 */
	class CStreamData {
	public:
		CStreamData( int id, const streamptr_type& stream, float weight )
			: mID(id), mStream(stream), mWeight(weight) { }
		CStreamData( const CStreamData& r )
			: mID(r.mID), mStream(r.mStream), mWeight(r.mWeight) { }

		int getID() const { return mID; }
		stream_type& getStream() const { return *mStream; }
		const float& getWeight() const { return mWeight; }
		float& getWeight() { return mWeight; }
	private:
		int				mID;
		streamptr_type	mStream;
		float			mWeight;
	};

public:
	// ----------------------------------------------------------------------

	CAnimStreamMixer() : mIDCounter(0) { }

	/**
	 *  Gets stream data by ID or NULL if stream is already lost.
	 *  @return Stream data or NULL.
	 */
	CStreamData* getStreamData( int id )
	{
		TStreamVector::iterator it = std::find_if(
			mStreams.begin(),
			mStreams.end(),
			FFinderById<CStreamData>( id ) );
		return it != mStreams.end() ? (&*it) : NULL;
	}

	/**
	 *  Adds a stream and returns it's ID.
	 */
	int addStream( const streamptr_type& stream, float weight )
	{
		++mIDCounter;
		mStreams.push_back( CStreamData( mIDCounter, stream, weight ) );
		return mIDCounter;
	}
	/**
	 *  Clears the streams.
	 */
	void clearStreams() { mStreams.clear(); }

	// ----------------------------------------------------------------------
	// IAnimStream

	virtual eAnimType getType() const {
		return static_cast<eAnimType>( traits::anim_type<value_type>::value );
	}
	
	virtual void pause() {
		std::for_each( mStreams.begin(), mStreams.end(), FPauser<true>() );
	}

	virtual void unpause() {
		std::for_each( mStreams.begin(), mStreams.end(), FPauser<false>() );
	}

	virtual void update( int numCurves, value_type* dest, int destStride ) {
		// remove ended streams if _DEL is true
		conditionalStreamRemove( bool2type<_DEL>() );
		int i, n = mStreams.size();

		// place for scratch values
		if( mScratchValues.size() < numCurves )
			mScratchValues.resize( numCurves );

		// sum weights
		float weightSum = 0.0f;
		for( i = 0; i < n; ++i ) {
			weightSum += mStreams[i].getWeight();
		}
		ASSERT_MSG( weightSum > 0.0f, "weight sum is non-positive" );
		float invWeightSum = 1.0f / weightSum;

		// go thru streams and weight-blend them into dest
		bool firstStream = true;
		float accumWeight = 0.0f; // for quat blending
		for( i = 0; i < n; ++i ) {
			const CStreamData& s = mStreams[i];
			float streamWeight = s.getWeight() * invWeightSum;
			// just skip zero-weight streams
			if( streamWeight <= 0.0f )
				continue;
			// sample stream into scratch
			s.getStream().update( numCurves, &mScratchValues[0], sizeof(value_type) );
			// blend all curves
			char* destVal = reinterpret_cast<char*>(dest);
			for( int c = 0; c < numCurves; ++c, destVal += destStride ) {
				value_type& dv = *reinterpret_cast<value_type*>(destVal);
				if( firstStream )
					SBlender<value_type>::calcFirst( dv, mScratchValues[c], streamWeight );
				else 
					SBlender<value_type>::calcBlend( dv, mScratchValues[c], streamWeight, accumWeight );
			}
			firstStream = false;
			accumWeight += streamWeight;
		}
	};

private:

	// ----------------------------------------------------------------------
	// Private stuff

	typedef std::vector<CStreamData> TStreamVector;
	typedef std::vector<value_type>	 TValueVector;

	template<bool N> struct bool2type { enum { value = N }; };

	// Pause or unpause functor
	template<bool N> struct FPauser {
		template<typename _A> void operator()( _A& tp ) { action( tp, bool2type<N>() ); }
		template<typename _A> void action( _A& tp, bool2type<false> ) { tp.getStream().unpause(); }
		template<typename _A> void action( _A& tp, bool2type<true> ) { tp.getStream().pause(); }
	};

	// Finder by ID functor.
	template<typename T>
	struct FFinderById {
		FFinderById( int id ) : mID(id) {}
		bool operator()( const T& p ) const { return p.id == mID; }
		int mID;
	};

	// Ended streams removal code
	void conditionalStreamRemove( bool2type<true> ) {
		for( TStreamVector::iterator it = mStreams.begin(); it != mStreams.end(); /**/ ) {
			CStreamData& s = *it;
			if( s.getStream().isFinished() )
				it = mStreams.erase( it );
			else
				++it;
		}
	}
	void conditionalStreamRemove( bool2type<false> ) { }

	// Blend specialization code.
	template<typename _T> struct SBlender {
		static void calcFirst( _T& dest, const _T& val, float weight ) {
			dest = val * weight;
		};
		static void calcBlend( _T& dest, const _T& val, float weight, float accumWeight ) {
			dest += val * weight;
		};
	};
	// Blend specialization code for quaternions.
	struct SBlender<SQuaternion> {
		static void calcFirst( SQuaternion& dest, const SQuaternion& val, float weight ) {
			dest = val;
		};
		static void calcBlend( SQuaternion& dest, const SQuaternion& val, float weight, float accumWeight ) {
			float w = weight / ( accumWeight + weight );
			dest.slerp( dest, val, w );
		};
	};

private:
	TStreamVector	mStreams;
	TValueVector	mScratchValues;
	value_type		mValue;
	int				mIDCounter;
};

}; // namespace

#endif
