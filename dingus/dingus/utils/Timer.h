// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __TIMER_H__
#define __TIMER_H__

namespace dingus {

//---------------------------------------------------------------------------

/**
 *  Quantized time value.
 *
 *  Unlike floats/doubles, doesn't lose precision. Internal 64 bit
 *  representation provides lots of range and precision: quantized at million
 *  in a second, provides ~290 thousand years.
 */
struct time_value {
public:
	/// Quantize into one million-th of a second.
	enum { FREQ = 1000*1000 };
public:
	typedef __int64 value_type;
public:
	explicit time_value( value_type v = 0 ) : value(v) { }

	static time_value fromsec( float seconds ) { return time_value( ((double)seconds)*FREQ ); }
	float tosec() const { return (float)value / FREQ; }

	void operator+=( const time_value& t ) { value += t.value; }
	void operator-=( const time_value& t ) { value -= t.value; }

public:
	value_type value;
};

inline time_value operator+( const time_value& a, const time_value& b ) {
	return time_value( a.value + b.value );
};
inline time_value operator-( const time_value& a, const time_value& b ) {
	return time_value( a.value - b.value );
};


//---------------------------------------------------------------------------

/**
 *  Timer class.
 */
class CTimer {
public:
	CTimer();

	void	reset() { mTime.value = mDeltaTime.value = 0; mDeltaTimeS = 0.0f; };
	void	start() { mStopped = false; }
	void	stop() { mStopped = true; mDeltaTime.value = 0; mDeltaTimeS = 0.0f; }
	void	advance( time_value v ) { mTime += v; mDeltaTime += v; setDeltaTimeS(); }

	/// Update timer
	void	update();
	/// Update with fixed step. For debugging purposes.
	void	updateFixed( time_value dt );

	const time_value& getTime() const { return mTime; }
	const time_value& getDeltaTime() const { return mDeltaTime; }
	const time_value& getAbsTime() const { return mAbsTime; }
	float getDeltaTimeS() const { return mDeltaTimeS; }

private:
	time_value fetchTime() const;
	void	setDeltaTimeS() { mDeltaTimeS = mDeltaTime.tosec(); }

private:
	double		mFreqDivMyFreq;
	bool		mUsingQPF; ///< QPF or timeGetTime
	bool		mStopped;

	/// App time (eg. paused periods don't count)
	time_value	mTime;
	/// Time between last updates
	time_value	mDeltaTime;
	/// Absolute time
	time_value	mAbsTime;
	/// Delta time pre-converted into seconds
	float		mDeltaTimeS;
};

//---------------------------------------------------------------------------

/**
 *  Global timer.
 */
extern CTimer	globalTimer;


}; // namespace dingus


#endif
