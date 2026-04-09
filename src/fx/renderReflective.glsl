#pragma sokol @module renderReflective

#pragma sokol @block commonlib
#pragma sokol @include lib_lighting.glsl
#pragma sokol @end

#pragma sokol @vs vs
#pragma sokol @include_block commonlib
layout(location=0) in vec3 va_position;

out vec4 uvRefl;

void main()
{
	gl_Position = mWVP * vec4(va_position, 1.0);
	vec4 uv = mWorldView * vec4(va_position, 1.0);
	uvRefl = mViewTexProj * uv;
}
#pragma sokol @end

#pragma sokol @fs fs
#pragma sokol @include_block commonlib

layout(binding=0) uniform texture2D texRefl;
layout(binding=0) uniform sampler smpRefl;

in vec4 uvRefl;
out vec4 frag_color;
void main()
{
	vec4 refl = textureProj(sampler2D(texRefl, smpRefl), uvRefl);
	refl.a = 0.25;
	frag_color = refl;
}
#pragma sokol @end

#pragma sokol @program prog vs fs
