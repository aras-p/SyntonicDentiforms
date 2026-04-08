#pragma once

#include "Scene.h"


class CSceneOut : public CScene {
public:
	CSceneOut( int number ) : CScene(number) { };

	virtual void initialize();

protected:
	virtual void	evaluateMeshes( float t ) { };
};

