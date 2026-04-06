// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __RENDER_MESH_H
#define __RENDER_MESH_H

#include "Renderable.h"
#include "../gfx/Mesh.h"


namespace dingus {


class CRenderableMesh : public CRenderable {
public:
	CRenderableMesh( CMesh& mesh, int group, const SVector3* origin = 0, int priority = 0 );

	const CMesh& getMesh() const { return *mMesh; }
	int getGroup() const { return mGroup; }

	virtual void render( const CRenderContext& ctx );
	
private:
	CMesh*	mMesh;
	int		mGroup;
};


}; // namespace


#endif
