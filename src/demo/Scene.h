#pragma once

#include "Anim.h"
#include "MeshEntity.h"

class Scene
{
  public:
    struct SceneMesh
    {
        MeshEntity *mesh;
        Vector3 pos;
        Vector3 rot;
        Vector3 rotVel; // over all animation
        int parentIdx;
    };

  public:
    Scene(int number);
    virtual ~Scene();

    virtual void initialize();

    void evaluate(float t, Matrix4x4 &light, Matrix4x4 &camera);
    void evaluateCamera(float t, Matrix4x4 &camera) const;
    void evaluateLight(float t, Matrix4x4 &light) const;
    void render(eRenderMode renderMode, sg_bindings *binds);

    int getLength() const { return mLength; }

    MeshEntity *addStaticMesh(DataMesh data);

    void addCut(float frame);
    float getPastCut(float t) const;
    int getCutCount() const { return mCutTimes.size(); }

  protected:
    virtual void evaluateMeshes(float t);
    static void toMatrix(const Vector3 &pos, const Vector3 &rot, Matrix4x4 &m);

  protected:
    void recurseAdd(int idx);

  protected:
    int mNumber;
    std::vector<SceneMesh> mMeshes; // owns meshes
    std::vector<int> mEvalOrder; // depth-first order for hierarchy
    Anim *mAnimLight;
    Anim *mAnimCamera;
    int mLength; // in max frames

    std::vector<float> mCutTimes; // 0..1
};
