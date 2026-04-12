#include "Scene.h"
#include "Pipelines.h"
#include "Rendering.h"
#include "SceneData.h"

#include <assert.h>

Scene::Scene(int number)
    : mNumber(number)
{
    mAnimLight = new Anim((DataAnim)(DataAnimAnim0 + number), "Light");
    mAnimCamera = new Anim((DataAnim)(DataAnimAnim0 + number), "Camera");
}

Scene::~Scene()
{
    int n = mMeshes.size();
    for (int i = 0; i < n; ++i)
        delete mMeshes[i].mesh;
}

static const SceneData *kSceneDatas[] = {nullptr, &kScene1, &kScene2, &kScene3, &kScene4, &kScene5, &kScene6};

void Scene::initialize()
{
    const SceneData &sd = *kSceneDatas[mNumber];
    mLength = sd.length;

    for (int i = 0; i < sd.count; ++i)
    {
        const SceneEntityData &e = sd.entities[i];
        Vector3 rot0(e.rot0[0], e.rot0[1], e.rot0[2]);
        Vector3 rot1(e.rot1[0], e.rot1[1], e.rot1[2]);
        SceneMesh mesh;
        mesh.parentIdx = e.parentIdx;
        mesh.pos = Vector3(e.pos[0], e.pos[1], e.pos[2]);
        mesh.rot = rot0 * (M_PI / 180.0f);
        mesh.rotVel = (rot1 - rot0) * (mLength * M_PI / 180.0f);
        mesh.mesh = new MeshEntity(e.mesh);
        mMeshes.push_back(mesh);

        toMatrix(mesh.pos, mesh.rot, mesh.mesh->mMatrix);
    }

    //
    // calculate hierarchy traversal order

    for (int i = 0; i < (int)mMeshes.size(); ++i)
    {
        if (mMeshes[i].parentIdx == -1)
        {
            recurseAdd(i);
        }
    }
}

void Scene::recurseAdd(int idx)
{
    mEvalOrder.push_back(idx);
    int n = mMeshes.size();
    for (int i = 0; i < n; ++i)
    {
        if (mMeshes[i].parentIdx == idx)
        {
            recurseAdd(i);
        }
    }
}

MeshEntity *Scene::addStaticMesh(DataMesh data)
{
    SceneMesh mesh;
    mesh.mesh = new MeshEntity(data);
    mesh.pos.set(0, 0, 0);
    mesh.rot.set(0, 0, 0);
    mesh.rotVel.set(0, 0, 0);
    mesh.parentIdx = -1;
    mEvalOrder.push_back(mMeshes.size());
    mMeshes.push_back(mesh);
    return mesh.mesh;
}

void Scene::toMatrix(const Vector3 &pos, const Vector3 &rot, Matrix4x4 &m)
{
    Matrix4x4 mrx, mry, mrz;
    mrx.rotationX(-rot.x);
    mry.rotationY(-rot.z);
    mrz.rotationZ(-rot.y);
    m = mrx * mrz * mry;
    m.getOrigin() = pos;
}

void Scene::evaluate(float t, Matrix4x4 &light, Matrix4x4 &camera)
{
    // eval light/camera
    evaluateLight(t, light);
    evaluateCamera(t, camera);

    // eval mesh transforms
    evaluateMeshes(t);
}

void Scene::evaluateCamera(float t, Matrix4x4 &camera) const
{
    Matrix4x4 mr;
    mAnimCamera->sample(t, camera);
    mr.rotationX(M_PI / 2);
    camera = mr * camera;
}

void Scene::evaluateLight(float t, Matrix4x4 &light) const
{
    Matrix4x4 mr;
    mAnimLight->sample(t, light);
    mr.rotationX(M_PI / 2);
    light = mr * light;
}

void Scene::evaluateMeshes(float t)
{
    // eval mesh transforms
    int n = mMeshes.size();
    for (int i = 0; i < n; ++i)
    {
        int idx = mEvalOrder[i];
        SceneMesh &mesh = mMeshes[idx];
        // local transform
        Vector3 rot = mesh.rot + mesh.rotVel * t;
        Matrix4x4 &m = mesh.mesh->mMatrix;
        toMatrix(mesh.pos, rot, m);
        // into world space
        if (mesh.parentIdx >= 0)
        {
            m = m * mMeshes[mesh.parentIdx].mesh->mMatrix;
        }
    }
}

void Scene::render(eRenderMode renderMode, sg_bindings *binds)
{
    if (mMeshes.empty())
        return;

    switch (renderMode)
    {
    case RM_LIT_SHADOWED:
        pipeline_apply(pip_renderLitShadowed);
        break;
    case RM_LIT_SHADOWED_FLIP:
        pipeline_apply(pip_renderLitShadowedFlip);
        break;
    case RM_SHADOW_CASTER:
        pipeline_apply(pip_shadowCaster);
        break;
    case RM_REFLECTIVE:
        pipeline_apply(pip_renderReflective);
        break;
    case RM_LIT:
        pipeline_apply(pip_renderLit);
        break;
    default:
        assert(false);
    }

    sg_apply_uniforms(0, {&g_global_u, sizeof(g_global_u)});

    Plane planes[6];
    extractFrustumPlanes(gRenderCam.getViewProjMatrix(), planes);
    for (const SceneMesh &m : mMeshes)
        m.mesh->render(renderMode, binds, planes);
}

void Scene::addCut(float frame)
{
    mCutTimes.push_back(frame / getLength());
}

float Scene::getPastCut(float t) const
{
    int n = mCutTimes.size();
    int i;
    for (i = 0; i < n; ++i)
    {
        if (mCutTimes[i] > t)
            break;
    }
    if (i == 0)
        return -1.0f;
    else
        return mCutTimes[i - 1];
}
