// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __ANIM_TIME_H
#define __ANIM_TIME_H

#include "../kernel/SystemClock.h"


namespace dingus {


// kind of HACK
inline float anim_time() {
	return (float)CSystemClock::getInstance().getTime();
};


};


#endif
