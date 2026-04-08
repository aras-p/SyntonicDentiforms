#pragma sokol @module billboards

#pragma sokol @vs vs
layout(location=0) in vec3 va_position;
layout(location=1) in vec4 va_color;
layout(location=2) in vec2 va_uv;
out vec2 uv;
out vec4 color;
void main()
{
	vec4 pos = vec4(va_position, 1.0);
	pos.y = -pos.y;
	gl_Position = pos;
	color = va_color;
	uv = va_uv;
}
#pragma sokol @end

#pragma sokol @fs fs
layout(binding = 0) uniform texture2D tex;
layout(binding = 0) uniform sampler smp;
in vec2 uv;
in vec4 color;
out vec4 frag_color;
void main()
{
    vec4 col = texture(sampler2D(tex, smp), uv);
	col *= color;
    frag_color = col;
}
#pragma sokol @end

#pragma sokol @program prog vs fs
