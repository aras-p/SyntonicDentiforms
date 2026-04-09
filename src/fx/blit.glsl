#pragma sokol @module blit

#pragma sokol @vs vs
#pragma sokol @glsl_options flip_vert_y
#pragma sokol @include lib_post.glsl
out vec2 uv;
void main()
{
	gFullScreenTri(gl_Position, uv);
}
#pragma sokol @end

#pragma sokol @fs fs
layout(binding = 0) uniform texture2D tex;
layout(binding = 0) uniform sampler smp;
in vec2 uv;
out vec4 frag_color;
void main()
{
    vec4 col = texture(sampler2D(tex, smp), uv);
    frag_color = col;
}
#pragma sokol @end

#pragma sokol @program prog vs fs
