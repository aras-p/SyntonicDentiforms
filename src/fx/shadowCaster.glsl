
#pragma sokol @module shadowCaster

#pragma sokol @block commonlib
#pragma sokol @include lib_lighting.glsl
#pragma sokol @end

#pragma sokol @vs vs
#pragma sokol @glsl_options fixup_clipspace // important: map clipspace z from -1..+1 to 0..+1 on GL
#pragma sokol @include_block commonlib
layout(location=0) in vec3 va_position;
void main()
{
	gl_Position = mWVP * vec4(va_position, 1.0);
}
#pragma sokol @end

#pragma sokol @fs fs
void main()
{
}
#pragma sokol @end

#pragma sokol @program prog vs fs
