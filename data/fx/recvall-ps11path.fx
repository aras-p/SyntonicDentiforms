// just ps1.1 rendering path, not complete fx
#include "_lib.fx"

// --------------------------------------------------------------------------
//  1.1 - vertex lighting

struct VS_OUTPUT {
	float4	pos 	: POSITION;
	float4	uvShadow	: TEXCOORD0;
	float4	uvCookie	: TEXCOORD1;
	float4	diffuse : COLOR; // diffuse+specular in RGB, specular in A
};

VS_OUTPUT vsMain11( VS_PN i ) {
	VS_OUTPUT	o;

	o.pos = mul( i.pos, mWVP );
	float4 uvs = mul( i.pos, mWorldView );
	uvs = mul( uvs, mShadowProj );
	o.uvShadow = o.uvCookie = uvs;

	float3 n, light, hlf;
	gVSLightTerms( i.pos, i.normal, mWorld, n, light, hlf );

	// lighting
	o.diffuse = gPSLight( n, light, hlf, 1.0 );

	return o;
}

float4 psMain11( VS_OUTPUT i ) : COLOR {
	float shadow = tex2D( smpShadow, i.uvShadow ).a;
	float cookie = tex2D( smpCookie, i.uvCookie ).a;
	return i.diffuse * (cookie * (shadow > 0 ? 0 : 1));
}

technique tecPS11 {
	pass P0 {
		VertexShader = compile vs_2_0 vsMain11();
		PixelShader = compile ps_2_0 psMain11();
		Sampler[0] = (smpShadow);
		Sampler[1] = (smpCookie);

		TextureTransformFlags[0] = Projected;
		TextureTransformFlags[1] = Projected;
	}
	pass PLast {
		TextureTransformFlags[0] = Disable;
		TextureTransformFlags[1] = Disable;
		Texture[0] = NULL;
		Texture[1] = NULL;
	}
}
