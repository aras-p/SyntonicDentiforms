#include "stdafx.h"
#include "MusicPlayer.h"


CMusicPlayer::CMusicPlayer()
:	mSound( NULL )
{
	assert( FSOUND_GetVersion() >= FMOD_VERSION );

	//FSOUND_SetOutput( FSOUND_OUTPUT_DSOUND );
	FSOUND_SetOutput( FSOUND_OUTPUT_WINMM );
	
	FSOUND_SetDriver(0);

	char c = FSOUND_Init( 44100, 16, 0 );
	assert( c );
}

CMusicPlayer::~CMusicPlayer()
{
	if( mSound )
		FSOUND_Stream_Close( mSound );
	FSOUND_Close();
}

void CMusicPlayer::play( const char* fileName )
{
	if( mSound )
		FSOUND_Stream_Close( mSound );

	mSound = FSOUND_Stream_Open( fileName, FSOUND_2D, 0, 0 );
	assert( mSound );

	FSOUND_Stream_Play( 1, mSound );
}

double CMusicPlayer::getTime() const
{
	if( !mSound )
		return 0.0;
	return FSOUND_Stream_GetTime( mSound ) / 1000.0;
}

double CMusicPlayer::getTotalTime() const
{
	if( !mSound )
		return 0.0;
	return FSOUND_Stream_GetLengthMs( mSound ) / 1000.0;
}

void CMusicPlayer::setTime( double t )
{
	if( mSound )
		FSOUND_Stream_SetTime( mSound, int( t * 1000 ) );
}