#pragma once

#include <assert.h>

#define ASSERT_MSG( condition, message ) assert( (condition) || !message )
#define ASSERT_FAIL_MSG( message ) assert( !message )
#define ASSERT_NOT_IMPL ASSERT_FAIL_MSG("not implemented")

