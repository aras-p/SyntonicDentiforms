// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __D3DCONSOLE_H
#define __D3DCONSOLE_H

#include "Console.h"

namespace dingus {

class CD3DFont;


class CD3DTextBoxConsoleRenderingContext : public IConsoleRenderingContext {
public:
	CD3DTextBoxConsoleRenderingContext( CD3DFont& font, int x, int y, D3DCOLOR color );

	virtual void write( const std::string& message );
	virtual void flush();

private:
	CD3DFont&	mFont;
	int			mX;
	int			mY;
	D3DCOLOR	mColor;

	std::string	mBuffer;
};


}; // namespace

#endif
