// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "Timer.h"

#include <mmsystem.h>

using namespace dingus;

CTimer globalTimer;

//---------------------------------------------------------------------------

CTimer::CTimer()
:	mTime(0), mDeltaTime(0)
{
	// Use QueryPerformanceFrequency() to get frequency of timer. If QPF
	// is not supported, we will timeGetTime() which returns milliseconds.
	LARGE_INTEGER ticksPerSec;
	mUsingQPF = QueryPerformanceFrequency( &ticksPerSec ) ? true : false;
	if( mUsingQPF )
		mFreqDivMyFreq = (double)time_value::FREQ / (double)ticksPerSec.QuadPart;
	else
		mFreqDivMyFreq = (double)time_value::FREQ / 1000.0;

	// Fetch absolute time
	mAbsTime = fetchTime();
}


time_value CTimer::fetchTime() const
{
	// get current time
	if( mUsingQPF ) {
		LARGE_INTEGER qpft;
		QueryPerformanceFrequency( &qpft );
		return time_value( (double)qpft.QuadPart * mFreqDivMyFreq );
	} else {
		DWORD tmt = timeGetTime();
		return time_value( (double)tmt * mFreqDivMyFreq );
	}
}

void CTimer::update()
{
	// get current time
	time_value now = fetchTime();

	// delta
	mDeltaTime = now - mAbsTime;

	// absolute time
	mAbsTime = now;

	// time - if we're paused, don't advance time and set delta to zero
	if( mStopped ) {
		mDeltaTime.value = 0;
	} else {
		mTime += mDeltaTime;
	}

	// preconvert dt into seconds for convenience and (possibly) efficiency
	setDeltaTimeS();
}

void CTimer::updateFixed( time_value dt )
{
	mDeltaTime = dt;
	mAbsTime += dt;

	// time - if we're paused, don't advance time and set delta to zero
	if( mStopped ) {
		mDeltaTime.value = 0;
	} else {
		mTime += mDeltaTime;
	}

	// preconvert dt into seconds for convenience and (possibly) efficiency
	setDeltaTimeS();
}
