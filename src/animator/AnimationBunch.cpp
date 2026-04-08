#include "AnimationBunch.h"
#include "src/utils/STLUtils.h"

using namespace dingus;

CAnimationBunch::~CAnimationBunch()
{
	stl_utils::wipe_map( mVector3Anims );
	stl_utils::wipe_map( mQuatAnims );
	stl_utils::wipe_map( mFloatAnims );
}

void CAnimationBunch::endCurves()
{
	int n = getCurveCount();
	for( int i = 0; i < n; ++i ) {
		int ipar = mCurveDescs[i].mParentIndex;
		int j;
		for( j = i; j < n; ++j ) {
			if( mCurveDescs[j].mParentIndex == ipar )
				break;
		}
		mCurveDescs[i].mChildrenCount = (j-i);
	}
}
