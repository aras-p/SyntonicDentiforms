// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __MESH_BUNDLE_H
#define __MESH_BUNDLE_H

#include "BundleSingleton.h"
#include "StorageResourceBundle.h"
#include "DeviceResource.h"
#include "../gfx/Mesh.h"

namespace dingus {


class CMeshBundle :	public CStorageResourceBundle<CMesh>,
					public CBundleSingleton<CMeshBundle>,
					public IDeviceResource
{
public:
	static void init( const std::string& predir ) {
		CMeshBundle* bundle = new CMeshBundle( predir );
		assert( bundle );
		assignInstance( *bundle );
	}

	virtual void createResource();
	virtual void activateResource();
	virtual void passivateResource();
	virtual void deleteResource();

protected:
	virtual CMesh* loadResourceById( const CResourceId& id, const CResourceId& fullName );
	virtual void deleteResource( CMesh& resource ) { delete &resource; }
	
private:
	CMeshBundle( const std::string& predir );
	virtual ~CMeshBundle() { clear(); };
	friend CBundleSingleton<CMeshBundle>;
	
	/// @return false on not found
	bool loadMesh( const CResourceId& id, const CResourceId& fullName, CMesh& mesh ) const;
};


}; // namespace

#endif
