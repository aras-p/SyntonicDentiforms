// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "RenderableMesh.h"


using namespace dingus;

CRenderableMesh::CRenderableMesh(CMesh& mesh, int group)
:	mMesh( &mesh ),
	mGroup( group )
{
	assert( group >= 0 && group < mesh.getGroupCount() );
}


void CRenderableMesh::render()
{
	assert( mMesh );

	//@TODO
	/*
	HRESULT hres;
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();

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
	*/
}
