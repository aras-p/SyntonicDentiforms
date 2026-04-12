#include "SceneOut.h"

void SceneOut::initialize()
{
    mLength = 1350;

    // create meshes
    for (int ix = -1; ix <= 1; ++ix)
    {
        for (int iy = -1; iy <= 1; ++iy)
        {
            for (int iz = -1; iz <= 1; ++iz)
            {
                MeshEntity *m = addStaticMesh(DataMeshBox);
                Vector3 &o = m->mMatrix.getOrigin();
                o.x = ix * 25;
                o.y = iy * 25;
                o.z = iz * 25;
            }
        }
    }
}
