#ifndef __ANIMATION_BUNDLE_H
#define __ANIMATION_BUNDLE_H

#include "../animator/AnimationBunch.h"
#include "BundleSingleton.h"
#include "StorageResourceBundle.h"


namespace dingus {


class CAnimationBundle : public CStorageResourceBundle<CAnimationBunch>, public CBundleSingleton<CAnimationBundle>
{
public:
	static void init( const std::string& predir ) {
		CAnimationBundle* bundle = new CAnimationBundle( predir );
		assert( bundle );
		assignInstance( *bundle );
	}

protected:
	virtual CAnimationBunch* loadResourceById( const CResourceId& id, const CResourceId& fullName );
	virtual void deleteResource( CAnimationBunch& resource ) { delete &resource; }
	
private:
	CAnimationBundle( const std::string& predir );
	virtual ~CAnimationBundle() { clear(); };
	friend CBundleSingleton<CAnimationBundle>;

	void readSampledData( CAnimationBunch& bunch, FILE* f, int loop, int curves, int groups ) const;
};

}; // namespace

#endif
