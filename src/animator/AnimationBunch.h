#pragma once

#include "src/utils/noncopyable.h"

#include "Animation.h"
#include "../math/Vector3.h"
#include "../math/Quaternion.h"
#include <string>
#include <map>
#include <vector>
#include <assert.h>


class CAnimationBunch : public noncopyable {
public:
	typedef IAnimation<SVector3>	TVector3Animation;
	typedef IAnimation<SQuaternion>	TQuatAnimation;
	typedef IAnimation<float>		TFloatAnimation;

public:
	CAnimationBunch() { };
	~CAnimationBunch();

	/// Adds named Vector3 animation.
	void addVector3Anim( const std::string& name, TVector3Animation& anim );
	/// Adds named Quaternion animation.
	void addQuatAnim( const std::string& name, TQuatAnimation& anim );
	/// Adds named Float animation.
	void addFloatAnim( const std::string& name, TFloatAnimation& anim );

	/// Finds Vector3 animation by name, or returns NULL.
	TVector3Animation* findVector3Anim( const std::string& name );
	/// Finds Quaternion animation by name, or returns NULL.
	TQuatAnimation* findQuatAnim( const std::string& name );
	/// Finds Float animation by name, or returns NULL.
	TFloatAnimation* findFloatAnim( const std::string& name );

	//
	// curve descriptors

	void	addCurveDesc(const std::string& name);
	/// Get curve count.
	int		getCurveCount() const;
	/// Get curve's name by index.
	const std::string& getCurveName( int curveIdx ) const;
	/// Gets curve's index by it's name, or -1 if no such curve exists.
	int		getCurveIndexByName( const std::string& name ) const;

private:
	typedef std::map<std::string, TVector3Animation*>	TVector3AnimMap;
	typedef std::map<std::string, TQuatAnimation*>		TQuatAnimMap;
	typedef std::map<std::string, TFloatAnimation*>		TFloatAnimMap;

	typedef std::vector<std::string> TCurveDescVector;

private:
	TVector3AnimMap		mVector3Anims;
	TQuatAnimMap		mQuatAnims;
	TFloatAnimMap		mFloatAnims;

	TCurveDescVector	mCurveDescs;
};


// --------------------------------------------------------------------------

inline void CAnimationBunch::addVector3Anim( const std::string& name, TVector3Animation& anim )
{
	bool unique = mVector3Anims.insert( std::make_pair( name, &anim ) ).second;
	assert( unique );
}
inline void CAnimationBunch::addQuatAnim( const std::string& name, TQuatAnimation& anim )
{
	bool unique = mQuatAnims.insert( std::make_pair( name, &anim ) ).second;
	assert( unique );
}
inline void CAnimationBunch::addFloatAnim( const std::string& name, TFloatAnimation& anim )
{
	bool unique = mFloatAnims.insert( std::make_pair( name, &anim ) ).second;
	assert( unique );
}

inline CAnimationBunch::TVector3Animation* CAnimationBunch::findVector3Anim( const std::string& name )
{
	TVector3AnimMap::const_iterator it = mVector3Anims.find( name );
	return (it!=mVector3Anims.end()) ? it->second : 0;
}
inline CAnimationBunch::TQuatAnimation* CAnimationBunch::findQuatAnim( const std::string& name )
{
	TQuatAnimMap::const_iterator it = mQuatAnims.find( name );
	return (it!=mQuatAnims.end()) ? it->second : 0;
}
inline CAnimationBunch::TFloatAnimation* CAnimationBunch::findFloatAnim( const std::string& name )
{
	TFloatAnimMap::const_iterator it = mFloatAnims.find( name );
	return (it!=mFloatAnims.end()) ? it->second : 0;
}

inline void CAnimationBunch::addCurveDesc(const std::string& name)
{
	assert( getCurveIndexByName(name) == -1 );
	mCurveDescs.push_back(name);
}
inline int CAnimationBunch::getCurveCount() const
{
	return mCurveDescs.size();
}
inline const std::string& CAnimationBunch::getCurveName( int curveIdx ) const
{
	assert( curveIdx >= 0 && curveIdx < mCurveDescs.size() );
	return mCurveDescs[curveIdx];
}
inline int	CAnimationBunch::getCurveIndexByName( const std::string& name ) const
{
	for( int i = 0; i < mCurveDescs.size(); ++i ) {
		if( getCurveName(i) == name )
			return i;
	}
	return -1;
}

CAnimationBunch* load_animation(const char* path);

