
#pragma sokol @module renderWhite

#pragma sokol @block commonlib
#pragma sokol @include lib_lighting.glsl
#pragma sokol @end

#pragma sokol @vs vs
#pragma sokol @include_block commonlib
layout(location=0) in vec3 va_position;
void main()
{
    vec4 worldPos = mWorld * vec4(va_position, 1.0);
    gl_Position = mViewProj * worldPos;
}
#pragma sokol @end

#pragma sokol @fs fs
out vec4 frag_color;
void main()
{
	frag_color = vec4(1.0);
}
#pragma sokol @end

#pragma sokol @program prog vs fs
