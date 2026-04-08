#pragma sokol @module overlay

#pragma sokol @vs vs
const vec2 pos[4] = {
    vec2(-1.0, -1.0),
    vec2(+1.0, -1.0),
    vec2(-1.0, +1.0),
    vec2(+1.0, +1.0),
};
void main()
{
    gl_Position = vec4(pos[gl_VertexIndex], 0.0, 1.0);
}
#pragma sokol @end

#pragma sokol @fs fs
layout(binding=0) uniform uniforms_fs {
	vec4 color;
};
out vec4 frag_color;
void main()
{
	frag_color = color;
}
#pragma sokol @end

#pragma sokol @program prog vs fs
