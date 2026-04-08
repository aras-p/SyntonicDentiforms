
#pragma sokol @module compositeAdd

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
