// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __CHUNK_SOURCE_H
#define __CHUNK_SOURCE_H

namespace dingus {


/**
 *  Base chunk source.
 *
 *  Given the request to lock some element count, it provides a chunk
 *  object that represents the locked elements.
 */
template<class CHUNK>
class IChunkSource {
public:
	/** Smart pointer to chunk type. */
	typedef	CHUNK::TSharedPtr	SHARED_CHUNK;

public:
	virtual ~IChunkSource() = 0 {};
	
	/**
	 *  Lock some element count.
	 *  @param count Element (eg. vertex or index) count.
	 *  @return Smart pointer to locked chunk.
	 */
	virtual SHARED_CHUNK lock( int count ) = 0;
};


}; // namespace

#endif
