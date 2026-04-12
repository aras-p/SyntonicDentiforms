#pragma once

#include "Scene.h"

class SceneOut : public Scene
{
  public:
    SceneOut(int number) : Scene(number) {};

    virtual void initialize();

  protected:
    virtual void evaluateMeshes(float t) {};
};
