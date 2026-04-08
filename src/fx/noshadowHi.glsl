#pragma sokol @module noshadowHi

#pragma sokol @block commonlib
#pragma sokol @include _lib.fx
#pragma sokol @end

#pragma sokol @vs vs
#pragma sokol @include_block commonlib
layout(location=0) in vec3 va_position;
layout(location=1) in vec3 va_normal;

out vec3 hlf;
out vec3 normal;
out vec3 tolight;

void main()
{
	gl_Position = mWVP * vec4(va_position, 1.0);
	gVSLightTerms(va_position, va_normal, mWorld, normal, tolight, hlf);
}
#pragma sokol @end

#pragma sokol @fs fs
#pragma sokol @include_block commonlib

in vec3 hlf;
in vec3 normal;
in vec3 tolight;
out vec4 frag_color;
void main()
{
	frag_color = gPSLight(normal, tolight, normalize(hlf), 1.0);
}
#pragma sokol @end

#pragma sokol @program prog vs fs
