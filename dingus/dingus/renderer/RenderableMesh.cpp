// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "RenderableMesh.h"
#include "RenderContext.h"
#include "../utils/Errors.h"
#include "../kernel/D3DDevice.h"


using namespace dingus;

DEFINE_POOLED_ALLOC(dingus::CRenderableMesh,256,false);


CRenderableMesh::CRenderableMesh( CMesh& mesh, int group, const SVector3* origin, int priority )
:	CRenderable( origin, priority ),
	mMesh( &mesh ),
	mGroup( group )
{
	assert( group >= 0 && group < mesh.getGroupCount() );
	assert( mesh.isCreated() );
}


void CRenderableMesh::render( CRenderContext const& ctx )
{
	assert( mMesh );

	HRESULT hres;
	//IDirect3DIndexBuffer9* ib = &mMesh->getIB();
	//IDirect3DVertexBuffer9* vb = &mMesh->getVB();
	//assert( ib && vb );

	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	CRenderStats& stats = device.getStats();

	device.setIndexBuffer( &mMesh->getIB() );
	device.setVertexBuffer( 0, &mMesh->getVB(), 0, mMesh->getVertexStride() );
	device.setDeclaration( mMesh->getVertexDecl() );

	const CMesh::CGroup& group = mMesh->getGroup( mGroup );
	hres = dx.DrawIndexedPrimitive( 
		D3DPT_TRIANGLELIST,
		0,
		group.getFirstVertex(),
		group.getVertexCount(),
		group.getFirstPrim() * 3,
		group.getPrimCount() );
	if( FAILED( hres ) ) {
		THROW_DXERROR( hres, "failed to DIP" );
	}

	// stats
	stats.incDrawCalls();
	stats.incVerticesRendered( group.getVertexCount() );
	stats.incPrimsRendered( group.getPrimCount() );
}
