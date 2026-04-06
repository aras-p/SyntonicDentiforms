#include "_lib.fx"

float4x4	mWorld;
float4x4	mWorldView;
float4x4	mWVP;


// --------------------------------------------------------------------------
//  2.0

#ifdef ENABLE_PS20_PATH


struct VS_OUTPUT20 {
	float4	pos 	: POSITION;
	half3	hlf			: TEXCOORD0;
	half3	normal		: TEXCOORD1;
	half3	tolight		: TEXCOORD2;
};

VS_OUTPUT20 vsMain20( VS_PN i ) {
	VS_OUTPUT20	o;
	o.pos = mul( i.pos, mWVP );
	gVSLightTerms( i.pos, i.normal, mWorld, o.normal, o.tolight, o.hlf );
	return o;
}

half4 psMain20( VS_OUTPUT20 i ) : COLOR
{
	return gPSLight( i.normal, i.tolight, nrnorm(i.hlf), 1.0 );
}

technique tecPS20 {
	pass P0 {
		VertexShader = compile vs_1_1 vsMain20();
		PixelShader = compile ps_2_0 psMain20();
	}
	pass PLast {
		Texture[0] = NULL;
		Texture[1] = NULL;
	}
}

#endif


// --------------------------------------------------------------------------
//  1.1 - vertex lighting


struct VS_OUTPUT {
	float4	pos 	: POSITION;
	float4	diffuse : COLOR;
};

VS_OUTPUT vsMain( VS_PN i ) {
	VS_OUTPUT	o;

	o.pos = mul( i.pos, mWVP );

	float3 n, light, hlf;
	gVSLightTerms( i.pos, i.normal, mWorld, n, light, hlf );

	// lighting
	o.diffuse = gPSLight( n, light, hlf, 1.0 );

	return o;
}

pixelshader psMain = asm {
	ps_1_1
	mov r0, v0
};


technique tecPS11 {
	pass P0 {
		VertexShader = compile vs_1_1 vsMain();
		PixelShader = (psMain);
	}
	pass PLast {
	}
}
