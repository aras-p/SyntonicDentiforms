#pragma sokol @module compositeAlpha

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
	rgb.a = 1.0 - edge.a;
    frag_color = rgb;
}
#pragma sokol @end

#pragma sokol @program prog vs fs
