#include "Vector4.h"
#include "Matrix4x4.h"

namespace dingus {

inline SVector4 SVector4Transform(const SVector4* pv, const SMatrix4x4* pm)
{
    SVector4 out;
    out.x = pm->m[0][0] * pv->x + pm->m[1][0] * pv->y + pm->m[2][0] * pv->z + pm->m[3][0] * pv->w;
    out.y = pm->m[0][1] * pv->x + pm->m[1][1] * pv->y + pm->m[2][1] * pv->z + pm->m[3][1] * pv->w;
    out.z = pm->m[0][2] * pv->x + pm->m[1][2] * pv->y + pm->m[2][2] * pv->z + pm->m[3][2] * pv->w;
    out.w = pm->m[0][3] * pv->x + pm->m[1][3] * pv->y + pm->m[2][3] * pv->z + pm->m[3][3] * pv->w;
    return out;
}

void SVector4TransformArray(SVector4* out, const SVector4* in, const SMatrix4x4* matrix, int elements)
{
    for (int i = 0; i < elements; ++i)
    {
        out[i] = SVector4Transform(&in[i], matrix);
    }
}

}; // namespace
