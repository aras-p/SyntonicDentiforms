#ifndef __FX_INIT_LISTENERS_H
#define __FX_INIT_LISTENERS_H

#include "EffectBundle.h"

namespace dingus {


class CCommonEffectInitListener : public IEffectInitListener {
public:
	virtual void onInitEffect( ID3DXEffect& fx );
};


};


#endif
