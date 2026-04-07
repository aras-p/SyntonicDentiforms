// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#pragma once

#include "../gfx/Mesh.h"


namespace dingus {


class CRenderableMesh {
public:
	CRenderableMesh(CMesh& mesh, int group);

	const CMesh& getMesh() const { return *mMesh; }
	int getGroup() const { return mGroup; }

	void render();
	
private:
	CMesh*	mMesh;
	int		mGroup;
};


}; // namespace
