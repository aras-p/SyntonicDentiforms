#pragma sokol @module postComposeBloom

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
layout(binding = 0) uniform texture2D tex1;
layout(binding = 1) uniform texture2D tex2;
layout(binding = 0) uniform sampler smp;
in vec2 uv;
out vec4 frag_color;
void main()
{
    vec4 col1 = texture(sampler2D(tex1, smp), uv);
    vec4 col2 = texture(sampler2D(tex2, smp), uv);
    frag_color = col1 + col2;
}
#pragma sokol @end

#pragma sokol @program prog vs fs
