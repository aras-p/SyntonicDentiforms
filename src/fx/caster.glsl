
#pragma sokol @module caster

#pragma sokol @block commonlib
#pragma sokol @include _lib.fx
#pragma sokol @end

#pragma sokol @vs vs
#pragma sokol @include_block commonlib
layout(location=0) in vec3 va_position;
void main()
{
	gl_Position = mWVP * vec4(va_position, 1.0);
}
#pragma sokol @end

#pragma sokol @fs fs
#pragma sokol @include_block commonlib
out vec4 frag_color;
void main()
{
	frag_color = vShadowID;
}
#pragma sokol @end

#pragma sokol @program prog vs fs
