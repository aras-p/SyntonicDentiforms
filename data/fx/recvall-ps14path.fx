// just ps1.4 rendering path, not complete fx

#include "_lib.fx"

// --------------------------------------------------------------------------
//  1.4 - 2 passes

#ifdef ENABLE_PS14_PATH


struct VS_OUTPUT_SH {
	float4 pos	: POSITION;
	float4 uvShadow : TEXCOORD0;
	float4 uvCookie : TEXCOORD1;
};
struct VS_OUTPUT_L {
	float4 pos : POSITION;
	float3	hlf		: COLOR0;
	float3	normal	: TEXCOORD0;
	float3	tolight	: TEXCOORD1;
};

VS_OUTPUT_SH vsMainSH( VS_PN i ) {
	VS_OUTPUT_SH o;

	o.pos = mul( i.pos, mWVP );
	float4 uvs = mul( i.pos, mWorldView );
	uvs = mul( uvs, mShadowProj );
	o.uvShadow = o.uvCookie = uvs;

	return o;
}

VS_OUTPUT_L vsMainL( VS_PN i ) {
	VS_OUTPUT_L o;
	o.pos = mul( i.pos, mWVP );
	gVSLightTerms( i.pos, i.normal, mWorld, o.normal, o.tolight, o.hlf );
	o.hlf = o.hlf * 0.5 + 0.5;
	return o;
}


float4 psMain14SH( VS_OUTPUT_SH i ) : COLOR
{
	// shadow
	float cshadow = tex2Dproj( smpShadow, i.uvShadow ).r;
	float shadow = (cshadow>0) ? 0 : 1;
	shadow *= tex2Dproj( smpCookie, i.uvCookie ).r;
	return shadow;
}

// HLSL can't split into phases :(
pixelshader psMainAsm14L = asm {
	ps_1_4
	def c0, 0.5, 0.5, 0.5, 0
    texcrd r0.xyz, t0
    texcrd r1.xyz, t1
	dp3_sat r1, r0, r1	// r1 = N.L

	phase
    dp3_sat r2.w, v0_bx2, v0_bx2
    mad r2.xyz, 1-r2.w, v0_bias, v0_bx2
    dp3_sat r2.w, r2, r0
    mul r2.w, r2.w, r2.w
    mul r2.w, r2.w, r2.w
    mul r2.w, r2.w, r2.w
    mul r2.x, r2.w, r2.w
    mad r0.xyz, r1, c0, r2.x
  + mov r0.w, r2.x
};

technique tecPS14 {
	pass PLight {
		VertexShader = compile vs_1_1 vsMainL();
		PixelShader = (psMainAsm14L);
	}
	pass PShadow {
		VertexShader = compile vs_1_1 vsMainSH();
		PixelShader = compile ps_1_4 psMain14SH();
		AlphaBlendEnable = True;
		SrcBlend = Zero;
		DestBlend = SrcColor;
		ZWriteEnable = False;
	}
	pass PLast {
		Texture[0] = NULL;
		Texture[1] = NULL;
		AlphaBlendEnable = False;
		ZWriteEnable = True;
	}
}

#endif
