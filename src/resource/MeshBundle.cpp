#include "MeshBundle.h"
#include "MeshSerializer.h"
#include "../utils/StringHelper.h"


CMeshBundle::CMeshBundle( const std::string& predir )
:	CStorageResourceBundle<CMesh>(predir)
{
	addExtension( ".dmesh" );
}


CMesh* CMeshBundle::loadResourceById( const CResourceId& id, const CResourceId& fullName )
{
	return CMeshSerializer::loadMeshFromFile(fullName.getUniqueName().c_str());
}
