#pragma sokol @module postBlurStep

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
layout(binding=0) uniform uniforms_fs {
	vec4 offset;
};

in vec2 uv;
out vec4 frag_color;
void main()
{
	float c0 = texture(sampler2D(tex, smp), uv + vec2(-offset.x,-offset.y)).a;
	float c1 = texture(sampler2D(tex, smp), uv + vec2( offset.x,-offset.y)).a;
	float c2 = texture(sampler2D(tex, smp), uv + vec2(-offset.x, offset.y)).a;
	float c3 = texture(sampler2D(tex, smp), uv + vec2( offset.x, offset.y)).a;
	float c = (c0 + c1 + c2 + c3) * 0.25;
	frag_color = vec4(1,0.97,0.92,1) * c; // a bit of sepia-ish tone
}
#pragma sokol @end

#pragma sokol @program prog vs fs
