// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __ASSERT_HELP_H
#define __ASSERT_HELP_H

#define ASSERT_MSG( condition, message ) assert( (condition) || !message )
#define ASSERT_FAIL_MSG( message ) assert( !message )
#define ASSERT_NOT_IMPL ASSERT_FAIL_MSG("not implemented")

#endif
