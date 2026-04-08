#pragma sokol @module filterToon

#pragma sokol @vs vs
const vec2 pos[4] = {
    vec2(-1.0, -1.0),
    vec2(+1.0, -1.0),
    vec2(-1.0, +1.0),
    vec2(+1.0, +1.0),
};
out vec2 uv;
void main()
{
    gl_Position = vec4(pos[gl_VertexIndex], 0.0, 1.0);
    uv = pos[gl_VertexIndex] * vec2(0.5, -0.5) + 0.5;
}
#pragma sokol @end

#pragma sokol @fs fs
layout(binding = 0) uniform texture2D tex;
layout(binding = 1) uniform texture2D texLUT;
layout(binding = 0) uniform sampler smp;

in vec2 uv;
out vec4 frag_color;
void main()
{
	vec4 c = texture(sampler2D(tex,smp), uv);
	vec4 clut = texture(sampler2D(texLUT,smp), c.ar);
	clut.a = 0;

	float cU = textureOffset(sampler2D(tex, smp), uv, ivec2(0,-1)).r;
	float cD = textureOffset(sampler2D(tex, smp), uv, ivec2(0,+1)).r;
	float cL = textureOffset(sampler2D(tex, smp), uv, ivec2(-1,0)).r;
	float cR = textureOffset(sampler2D(tex, smp), uv, ivec2(+1,0)).r;
	float dUD = (cU - cD)*4;
	float dLR = (cL - cR)*4;
	dUD = dUD * dUD;
	dLR = dLR * dLR;
	float edge = clamp(1 - dUD - dLR, 0.0, 1.0);

	frag_color = clut * edge;
}
#pragma sokol @end

#pragma sokol @program prog vs fs
