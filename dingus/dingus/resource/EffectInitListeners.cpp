#include "../stdafx.h"
#pragma hdrstop

#include "EffectInitListeners.h"
#include "TextureBundle.h"

using namespace dingus;


void CCommonEffectInitListener::onInitEffect( ID3DXEffect& fx )
{
	int i;
	D3DXEFFECT_DESC desc;
	fx.GetDesc( &desc );

	// get parameters
	for( i = 0; i < desc.Parameters; ++i ) {
		D3DXHANDLE hparam = fx.GetParameter( NULL, i );
		D3DXPARAMETER_DESC pdesc;
		fx.GetParameterDesc( hparam, &pdesc );
		// get annotations
		for( int j = 0; j < pdesc.Annotations; ++j ) {
			D3DXHANDLE hannot = fx.GetAnnotation( hparam, j );
			D3DXPARAMETER_DESC adesc;
			fx.GetParameterDesc( hannot, &adesc );
			const char* aname = adesc.Name;
			if( !strcmpi(aname,"tex") ) {
				const char* texName = NULL;
				fx.GetString( hannot, &texName );
				CD3DTexture* tex = CTextureBundle::getInstance().getResourceById( texName );
				assert( tex );
				fx.SetTexture( hparam, tex->getObject() );
			}
		}
	}
}

