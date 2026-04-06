// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __ABSTRACT_NOTIFIER_H
#define __ABSTRACT_NOTIFIER_H

namespace dingus {

/**
 *  Base class for all notifiers. Implements common listener notification
 *  behavior.
 *
 *  In order to use must be parametrized and must provide implementation for
 *  notifyListener( T& listener ) method.
 */
template<class T>
class CAbstractNotifier {
public:
	virtual ~CAbstractNotifier() {};

	virtual void addListener( T& listener ) { mListeners.push_back( &listener ); }
	virtual void removeListener( T& listener ) { mListeners.remove( &listener ); }
	virtual void clear() { mListeners.clear(); }
	void notify() {
		for( TListenerVector::iterator it = mListeners.begin(); it != mListeners.end(); ++it )
			notifyListener( *(*it) );
	}

protected:
	typedef fastvector<T*>	TListenerVector;

protected:
	/**
	 *  Implement this method for listener notification. It gets called
	 *  for every listener from notify().
	 */
	virtual void notifyListener( T& listener ) = 0;
		
private:
	TListenerVector	mListeners;
};


/**
 *  Base class for all data notifiers. Implements common listener notification
 *  behavior.
 *
 *  In order to use must be parametrized and must provide implementation for
 *  notifyListener( T& listener, U data ) method.
 */
template<class T, class U>
class CAbstractNotifierData {
public:
	virtual ~CAbstractNotifierData() = 0 {};

	virtual void addListener( T& listener ) { mListeners.push_back( &listener ); }
	virtual void removeListener( T& listener ) { mListeners.remove( &listener ); }
	virtual void clear() { mListeners.clear(); }
	void notify( U data ) {
		for( TListenerVector::iterator it = mListeners.begin(); it != mListeners.end(); ++it )
			notifyListener( *(*it), data );
	}

protected:
	typedef fastvector<T*>	TListenerVector;

protected:
	/**
	 *  Implement this method for listener notification. It gets called
	 *  for every listener from notify().
	 *
	 *  Isn't defined as pure virtual to allow non-const or const data objects
	 *  to be passed to the listeners.
	 */
	virtual void notifyListener( T& listener, U data ) { };
	
private:
	TListenerVector mListeners;
};


/**
 *  Simple notifier class. Provides listener registry, listener notification
 *  is left to subclasses.
 */
template<class T>
class CSimpleNotifier {
public:
	typedef fastvector<T*>	TListenerVector;
public:
	virtual ~CSimpleNotifier() {};

	void addListener( T& listener ) { mListeners.push_back( &listener ); }
	void removeListener( T& listener ) { mListeners.remove( &listener ); }
	void clear() { mListeners.clear(); }

	TListenerVector const& getListeners() const { return mListeners; }
	TListenerVector& getListeners() { return mListeners; }

private:
	TListenerVector mListeners;
};


}; // namespace

#endif
