#pragma sokol @module overlay

#pragma sokol @vs vs
#pragma sokol @glsl_options flip_vert_y
#pragma sokol @include lib_post.glsl
void main()
{
	vec2 uv;
	gFullScreenTri(gl_Position, uv);
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
