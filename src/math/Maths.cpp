#include "Maths.h"

SVector3 SVector3::transformCoord(const SMatrix4x4 &pm) const
{
    SVector3 out;
    float norm = pm.m[0][3] * x + pm.m[1][3] * y + pm.m[2][3] * z + pm.m[3][3];

    out.x = (pm.m[0][0] * x + pm.m[1][0] * y + pm.m[2][0] * z + pm.m[3][0]) / norm;
    out.y = (pm.m[0][1] * x + pm.m[1][1] * y + pm.m[2][1] * z + pm.m[3][1]) / norm;
    out.z = (pm.m[0][2] * x + pm.m[1][2] * y + pm.m[2][2] * z + pm.m[3][2]) / norm;
    return out;
}

SMatrix4x4::SMatrix4x4(const SVector3 &pos, const SQuaternion &rot)
{
    identify();
    m[0][0] = 1.0f - 2.0f * (rot.y * rot.y + rot.z * rot.z);
    m[0][1] = 2.0f * (rot.x * rot.y + rot.z * rot.w);
    m[0][2] = 2.0f * (rot.x * rot.z - rot.y * rot.w);
    m[1][0] = 2.0f * (rot.x * rot.y - rot.z * rot.w);
    m[1][1] = 1.0f - 2.0f * (rot.x * rot.x + rot.z * rot.z);
    m[1][2] = 2.0f * (rot.y * rot.z + rot.x * rot.w);
    m[2][0] = 2.0f * (rot.x * rot.z + rot.y * rot.w);
    m[2][1] = 2.0f * (rot.y * rot.z - rot.x * rot.w);
    m[2][2] = 1.0f - 2.0f * (rot.x * rot.x + rot.y * rot.y);
    getOrigin() = pos;
}

SMatrix4x4 operator*(const SMatrix4x4 &a, const SMatrix4x4 &b)
{
    SMatrix4x4 out;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            out.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
        }
    }
    return out;
}

void SMatrix4x4::perspectiveFovLH(float fovy, float aspect, float zn, float zf)
{
    identify();
    m[0][0] = 1.0f / (aspect * tanf(fovy / 2.0f));
    m[1][1] = 1.0f / tanf(fovy / 2.0f);
    m[2][2] = zf / (zf - zn);
    m[2][3] = 1.0f;
    m[3][2] = (zf * zn) / (zn - zf);
    m[3][3] = 0.0f;
}

void SMatrix4x4::orthoLH(float w, float h, float zn, float zf)
{
    identify();
    m[0][0] = 2.0f / w;
    m[1][1] = 2.0f / h;
    m[2][2] = 1.0f / (zf - zn);
    m[3][2] = zn / (zn - zf);
}

void SMatrix4x4::invert()
{
    float det, t[3], v[16];

    t[0] = m[2][2] * m[3][3] - m[2][3] * m[3][2];
    t[1] = m[1][2] * m[3][3] - m[1][3] * m[3][2];
    t[2] = m[1][2] * m[2][3] - m[1][3] * m[2][2];
    v[0] = m[1][1] * t[0] - m[2][1] * t[1] + m[3][1] * t[2];
    v[4] = -m[1][0] * t[0] + m[2][0] * t[1] - m[3][0] * t[2];

    t[0] = m[1][0] * m[2][1] - m[2][0] * m[1][1];
    t[1] = m[1][0] * m[3][1] - m[3][0] * m[1][1];
    t[2] = m[2][0] * m[3][1] - m[3][0] * m[2][1];
    v[8] = m[3][3] * t[0] - m[2][3] * t[1] + m[1][3] * t[2];
    v[12] = -m[3][2] * t[0] + m[2][2] * t[1] - m[1][2] * t[2];

    det = m[0][0] * v[0] + m[0][1] * v[4] + m[0][2] * v[8] + m[0][3] * v[12];
    if (det == 0.0f)
        return;

    t[0] = m[2][2] * m[3][3] - m[2][3] * m[3][2];
    t[1] = m[0][2] * m[3][3] - m[0][3] * m[3][2];
    t[2] = m[0][2] * m[2][3] - m[0][3] * m[2][2];
    v[1] = -m[0][1] * t[0] + m[2][1] * t[1] - m[3][1] * t[2];
    v[5] = m[0][0] * t[0] - m[2][0] * t[1] + m[3][0] * t[2];

    t[0] = m[0][0] * m[2][1] - m[2][0] * m[0][1];
    t[1] = m[3][0] * m[0][1] - m[0][0] * m[3][1];
    t[2] = m[2][0] * m[3][1] - m[3][0] * m[2][1];
    v[9] = -m[3][3] * t[0] - m[2][3] * t[1] - m[0][3] * t[2];
    v[13] = m[3][2] * t[0] + m[2][2] * t[1] + m[0][2] * t[2];

    t[0] = m[1][2] * m[3][3] - m[1][3] * m[3][2];
    t[1] = m[0][2] * m[3][3] - m[0][3] * m[3][2];
    t[2] = m[0][2] * m[1][3] - m[0][3] * m[1][2];
    v[2] = m[0][1] * t[0] - m[1][1] * t[1] + m[3][1] * t[2];
    v[6] = -m[0][0] * t[0] + m[1][0] * t[1] - m[3][0] * t[2];

    t[0] = m[0][0] * m[1][1] - m[1][0] * m[0][1];
    t[1] = m[3][0] * m[0][1] - m[0][0] * m[3][1];
    t[2] = m[1][0] * m[3][1] - m[3][0] * m[1][1];
    v[10] = m[3][3] * t[0] + m[1][3] * t[1] + m[0][3] * t[2];
    v[14] = -m[3][2] * t[0] - m[1][2] * t[1] - m[0][2] * t[2];

    t[0] = m[1][2] * m[2][3] - m[1][3] * m[2][2];
    t[1] = m[0][2] * m[2][3] - m[0][3] * m[2][2];
    t[2] = m[0][2] * m[1][3] - m[0][3] * m[1][2];
    v[3] = -m[0][1] * t[0] + m[1][1] * t[1] - m[2][1] * t[2];
    v[7] = m[0][0] * t[0] - m[1][0] * t[1] + m[2][0] * t[2];

    v[11] = -m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) +
            m[1][0] * (m[0][1] * m[2][3] - m[0][3] * m[2][1]) -
            m[2][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]);

    v[15] = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
            m[1][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]) +
            m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]);

    det = 1.0f / det;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] = v[4 * i + j] * det;
}

void extractFrustumPlanes(const SMatrix4x4 &viewProj, SPlane planes[6])
{
    planes[0] = SPlane(viewProj.m[0][0] + viewProj.m[0][3], viewProj.m[1][0] + viewProj.m[1][3], viewProj.m[2][0] + viewProj.m[2][3], viewProj.m[3][0] + viewProj.m[3][3]); // left
    planes[1] = SPlane(viewProj.m[0][3] - viewProj.m[0][0], viewProj.m[1][3] - viewProj.m[1][0], viewProj.m[2][3] - viewProj.m[2][0], viewProj.m[3][3] - viewProj.m[3][0]); // right
    planes[2] = SPlane(viewProj.m[0][1] + viewProj.m[0][3], viewProj.m[1][1] + viewProj.m[1][3], viewProj.m[2][1] + viewProj.m[2][3], viewProj.m[3][1] + viewProj.m[3][3]); // bottom
    planes[3] = SPlane(viewProj.m[0][3] - viewProj.m[0][1], viewProj.m[1][3] - viewProj.m[1][1], viewProj.m[2][3] - viewProj.m[2][1], viewProj.m[3][3] - viewProj.m[3][1]); // top
    planes[4] = SPlane(viewProj.m[0][2], viewProj.m[1][2], viewProj.m[2][2], viewProj.m[3][2]);                                                                             // near
    planes[5] = SPlane(viewProj.m[0][3] - viewProj.m[0][2], viewProj.m[1][3] - viewProj.m[1][2], viewProj.m[2][3] - viewProj.m[2][2], viewProj.m[3][3] - viewProj.m[3][2]); // far
}

void SQuaternion::slerp(const SQuaternion &a, const SQuaternion &b, float t)
{
    float temp = 1.0f - t;
    float dot = a.dot(b);
    if (dot < 0.0f)
    {
        t = -t;
        dot = -dot;
    }

    if (1.0f - dot > 0.001f)
    {
        float theta = acosf(dot);
        temp = sinf(theta * temp) / sinf(theta);
        t = sinf(theta * t) / sinf(theta);
    }

    x = temp * a.x + t * b.x;
    y = temp * a.y + t * b.y;
    z = temp * a.z + t * b.z;
    w = temp * a.w + t * b.w;
}

bool CAABox::frustumCull(const SMatrix4x4 &world, const SPlane planes[6]) const
{
    // calculate OBB center in world space
    SVector3 center = (mMin + mMax) * 0.5f;
    SVector3 cw(
        center.x * world.m[0][0] + center.y * world.m[1][0] + center.z * world.m[2][0] + world.m[3][0],
        center.x * world.m[0][1] + center.y * world.m[1][1] + center.z * world.m[2][1] + world.m[3][1],
        center.x * world.m[0][2] + center.y * world.m[1][2] + center.z * world.m[2][2] + world.m[3][2]);
    // calculate OBB half extents
    SVector3 h = (mMax - mMin) * 0.5f;
    const SVector3 &axX = world.getAxisX();
    const SVector3 &axY = world.getAxisY();
    const SVector3 &axZ = world.getAxisZ();

    for (int i = 0; i < 6; ++i)
    {
        const SPlane &p = planes[i];
        SVector3 n(p.a, p.b, p.c);
        float dc = n.dot(cw) + p.d;
        // OBB support radius along n
        float r = h.x * fabsf(n.dot(axX)) + h.y * fabsf(n.dot(axY)) + h.z * fabsf(n.dot(axZ));
        if (dc < -r)
            return true;
    }
    return false;
}
