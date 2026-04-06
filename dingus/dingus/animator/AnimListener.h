// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __ANIMATION_LISTENER_H
#define __ANIMATION_LISTENER_H

namespace dingus {

/**
 *  Animation channel listener.
 *  @param _C Animation channel type.
 */
template<typename _C>
class IAnimListener {
public:
	typedef _C	channel_type;

public:
	virtual ~IAnimListener() = 0 { }
	virtual void onAnimate( channel_type& channel ) = 0;
};

}; // namespace

#endif
