// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __ANIM_CONTEXT_H
#define __ANIM_CONTEXT_H

#include "AnimChannel.h"

namespace dingus {

class CAnimContext {
public:
	typedef	fastvector<IAnimChannel*> TChannelVector;

	void addChannel( IAnimChannel& chn ) { mChannels.push_back(&chn); }
	void removeChannel( IAnimChannel& chn ) { mChannels.remove(&chn); }

	void perform() {
		TChannelVector::iterator it, itEnd = mChannels.end();
		for( it = mChannels.begin(); it != itEnd; ++it ) {
			assert( *it );
			(*it)->perform();
		}
	}

private:
	TChannelVector mChannels;
};

}; // namespace

#endif
