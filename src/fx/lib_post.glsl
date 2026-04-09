
void gFullScreenTri(out vec4 pos, out vec2 uv)
{
    uv = vec2((gl_VertexIndex & 1) != 0 ? 2.0 : 0.0, (gl_VertexIndex & 2) != 0 ? -1.0 : 1.0);
    pos = vec4(uv.x * 2.0 - 1.0, -(uv.y * 2.0 - 1.0), 0.0, 1.0);
}
