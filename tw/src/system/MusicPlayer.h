#ifndef __MUSIC_PLAYER_H
#define __MUSIC_PLAYER_H

#include "../fmod.h"


class CMusicPlayer {
public:
	CMusicPlayer();
	~CMusicPlayer();

	void play( const char* fileName );

	double getTime() const;
	void setTime( double t );
	double getTotalTime() const;

private:
	FSOUND_STREAM* mSound;
};


#endif
