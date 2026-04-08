#pragma once

#include "BundleSingleton.h"
#include "StorageResourceBundle.h"
#include "../gfx/Mesh.h"


class CMeshBundle :	public CStorageResourceBundle<CMesh>,
					public CBundleSingleton<CMeshBundle>
{
public:
	static void init( const std::string& predir ) {
		CMeshBundle* bundle = new CMeshBundle( predir );
		assert( bundle );
		assignInstance( *bundle );
	}

protected:
	virtual CMesh* loadResourceById( const CResourceId& id, const CResourceId& fullName );
	virtual void deleteResource( CMesh& resource ) { delete &resource; }
	
private:
	CMeshBundle( const std::string& predir );
	virtual ~CMeshBundle() { clear(); };
	friend CBundleSingleton<CMeshBundle>;
};


