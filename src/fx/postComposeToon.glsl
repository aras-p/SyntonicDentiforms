#pragma sokol @module postComposeToon

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
layout(binding = 1) uniform texture2D texAlpha;
layout(binding = 2) uniform texture2D texEdgeLUT;
layout(binding = 0) uniform sampler smp;
in vec2 uv;
out vec4 frag_color;
void main()
{
    vec4 rgb = texture(sampler2D(tex, smp), uv);
    vec4 alp = texture(sampler2D(texAlpha, smp), uv);
	vec4 edge = texture(sampler2D(texEdgeLUT, smp), vec2(alp.ar));
	rgb.rgb *= edge.rgb;
	rgb.a = edge.a;
    frag_color = rgb;
}
#pragma sokol @end

#pragma sokol @program prog vs fs
