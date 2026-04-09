#include "DynamicVBManager.h"
#include <assert.h>

static sg_buffer s_dynamic_buffer;

void dynamic_vb_init(size_t capacityBytes)
{
	sg_buffer_desc desc = {};
	desc.usage.vertex_buffer = true;
	desc.size = capacityBytes;
	desc.usage.immutable = false;
	desc.usage.dynamic_update = true;
	s_dynamic_buffer = sg_make_buffer(&desc);
    assert(sg_query_buffer_state(s_dynamic_buffer) == SG_RESOURCESTATE_VALID);
}

void dynamic_vb_shutdown()
{
	sg_destroy_buffer(s_dynamic_buffer);
	s_dynamic_buffer = {};
}

int dynamic_vb_append(const void* data, size_t size)
{
	int offset = sg_append_buffer(s_dynamic_buffer, {data, size});
    assert(!sg_query_buffer_overflow(s_dynamic_buffer));
	return offset;
}

sg_buffer dynamic_vb_get()
{
	return s_dynamic_buffer;
}
