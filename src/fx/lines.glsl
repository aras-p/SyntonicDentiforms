#pragma sokol @module lines

#pragma sokol @block commonlib
#pragma sokol @include lib_lighting.glsl
#pragma sokol @end

#pragma sokol @vs vs
#pragma sokol @include_block commonlib
layout(location=0) in vec3 va_position;
layout(location=1) in vec4 va_color;
layout(location=2) in vec2 va_uv;

out vec2 uv;
out vec4 color;

void main()
{
	gl_Position = mViewProj * vec4(va_position, 1.0);
	uv = va_uv;
	color = va_color;
}
#pragma sokol @end

#pragma sokol @fs fs
#pragma sokol @include_block commonlib

layout(binding=0) uniform texture2D tex;
layout(binding=0) uniform sampler smp;

in vec2 uv;
in vec4 color;
out vec4 frag_color;
void main()
{
	vec4 c = texture(sampler2D(tex, smp), uv) * color;
	if (c.a < 10.0/255.0)
		discard;
	frag_color = c;
}
#pragma sokol @end

#pragma sokol @program prog vs fs
