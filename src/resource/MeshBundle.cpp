// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "MeshBundle.h"
#include "MeshSerializer.h"
#include "../utils/Errors.h"
#include "../utils/StringHelper.h"

using namespace dingus;

CMeshBundle::CMeshBundle( const std::string& predir )
:	CStorageResourceBundle<CMesh>(predir)
{
	addExtension( ".dmesh" );
}


CMesh* CMeshBundle::loadResourceById( const CResourceId& id, const CResourceId& fullName )
{
	return CMeshSerializer::loadMeshFromFile(fullName.getUniqueName().c_str());
}
