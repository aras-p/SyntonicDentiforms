// just ps1.1 rendering path, not complete fx

// --------------------------------------------------------------------------
//  1.1 - 1 sample shadows, vertex lighting

struct VS_OUTPUT {
	float4	pos 	: POSITION;
	float4	uvShadow	: TEXCOORD0;
	float4	uvCookie	: TEXCOORD1;
	float4	diffuse : COLOR; // diffuse+specular in RGB, specular in A
};

VS_OUTPUT vsMain( VS_PN i ) {
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

pixelshader psMain = asm {
	ps_1_1
	tex t0		// shadowmap
	tex t1		// cookie
	// largen the difference, absolute value
	sub_x4 r0, c0, t0
	dp3_x4_sat r0, r0, r0
	add_x4_sat r0, r0, r0
	// modulate by cookie
	mul r0, t1, 1-r0
	// modulate by diffuse
	mul r0, r0, v0
};


technique tecPS11 {
	pass P0 {
		VertexShader = compile vs_2_0 vsMain();
		PixelShader = (psMain);
		PixelShaderConstant[0] = (vShadowID);
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
