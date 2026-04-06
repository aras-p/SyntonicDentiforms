// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "MeshBundle.h"
#include "VertexDeclBundle.h"
#include "MeshSerializer.h"
#include "../utils/Errors.h"
#include "../kernel/D3DDevice.h"
#include "../utils/StringHelper.h"

using namespace dingus;

CMeshBundle::CMeshBundle( const std::string& predir )
:	CStorageResourceBundle<CMesh>(predir)
{
	addExtension( ".dmesh" );
	//addExtension( ".x" ); // legacy
};


bool CMeshBundle::loadMesh( const CResourceId& id, const CResourceId& fullName, CMesh& mesh ) const
{
	// our own format
	assert( !mesh.isCreated() );
	bool ok = CMeshSerializer::loadMeshFromFile( fullName.getUniqueName().c_str(), mesh );
	if( !ok )
		return false;

	mesh.computeAABBs();
	CONSOLE.write( "mesh loaded '" + id.getUniqueName() + "'" );
	return true;
}


CMesh* CMeshBundle::loadResourceById( const CResourceId& id, const CResourceId& fullName )
{
	CMesh* mesh = new CMesh();
	bool ok = loadMesh( id, fullName, *mesh );
	if( !ok ) {
		delete mesh;
		return NULL;
	}
	return mesh;
}

void CMeshBundle::createResource()
{
	const TStringVector& ext = getExtensions();
	int n = ext.size();

	// reload all objects
	TResourceMap::iterator it;
	for( it = mResourceMap.begin(); it != mResourceMap.end(); ++it ) {
		CMesh& res = *it->second;
		assert( !res.isCreated() );
		// try to load into created object by trying all extensions
		for( int i = 0; i < n; ++i ) {
			CResourceId idWithPathExt( getPreDir() + it->first.getUniqueName() + ext[i] );
			bool ok = loadMesh( it->first, idWithPathExt, res );
			if( ok )
				break;
		}
		assert( res.isCreated() );
	}
}

void CMeshBundle::activateResource()
{
}

void CMeshBundle::passivateResource()
{
}

void CMeshBundle::deleteResource()
{
	// unload all objects
	TResourceMap::iterator it;
	for( it = mResourceMap.begin(); it != mResourceMap.end(); ++it ) {
		CMesh& res = *it->second;
		assert( res.isCreated() );
		res.deleteResource();
	}
}
