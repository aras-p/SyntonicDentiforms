#include "_lib.fx"
  
texture		tBase;
texture		tLUT;

// x=0.5/texwidth, y=0.5/texheight
float4	vFixUV;



sampler smpBase = sampler_state {
    Texture   = (tBase);
    MipFilter = None;	MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};
sampler smpLUT = sampler_state {
    Texture   = (tLUT);
    MipFilter = Point;	MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};

struct VS_OUTPUT {
	float4 pos	: POSITION;
	float2 uv	: TEXCOORD0;
	float2 uvE[4] : TEXCOORD1;
};


// --------------------------------------------------------------------------
//  2.0

#ifdef ENABLE_PS20_PATH


VS_OUTPUT vsMain( VS_PUV i )
{
    VS_OUTPUT o;
	o.pos = billboardpos( i.pos );
	float2 uv = i.uv + vFixUV.xy;
	o.uv = uv;

	float dx = vFixUV.x*2;
	float dy = vFixUV.y*2;
	o.uvE[0] = uv + float2(0,-dy);
	o.uvE[1] = uv + float2(0, dy);
	o.uvE[2] = uv + float2(-dx,0);
	o.uvE[3] = uv + float2( dx,0);
	return o;
}

half4 psMain20( VS_OUTPUT i ) : COLOR
{
	half4 c = tex2D( smpBase, i.uv );
	half4 clut = tex2D( smpLUT, c.ar );
	clut.a = 0;

	half cU = tex2D( smpBase, i.uvE[0] ).r;
	half cD = tex2D( smpBase, i.uvE[1] ).r;
	half cL = tex2D( smpBase, i.uvE[2] ).r;
	half cR = tex2D( smpBase, i.uvE[3] ).r;
	half dUD = (cU - cD)*4;
	half dLR = (cL - cR)*4;
	dUD = dUD * dUD;
	dLR = dLR * dLR;
	half edge = saturate( 1 - dUD - dLR );

	return clut * edge;
}

technique tec20 {
	pass P0 {
		VertexShader = compile vs_1_1 vsMain();
		PixelShader = compile ps_2_0 psMain20();

		ZEnable = False;
		ZWriteEnable = False;
	}
	pass PLast {
		ZEnable = True;
		ZWriteEnable = True;
		Texture[0] = NULL;
	}
}

#endif


// --------------------------------------------------------------------------
//  1.4 - bugs on Radeon 9000?

// disable
#if 0
#ifdef ENABLE_PS14_PATH


float4 psMain14( VS_OUTPUT i ) : COLOR
{
	float4 c = tex2D( smpBase, i.uv );
	float4 clut = tex2D( smpLUT, c.ar );
	clut.a = 0;

	float cU = tex2D( smpBase, i.uvE[0] ).r;
	float cD = tex2D( smpBase, i.uvE[1] ).r;
	float cL = tex2D( smpBase, i.uvE[2] ).r;
	float cR = tex2D( smpBase, i.uvE[3] ).r;
	float dUD = (cU - cD)*4;
	float dLR = (cL - cR)*4;
	dUD = dUD * dUD;
	dLR = dLR * dLR;
	float edge = saturate( 1 - dUD - dLR );

	return clut * edge;
}

//   smpBase      s0       1
//   smpBase      s1       1
//   smpBase      s2       1
//   smpLUT       s3       1
pixelshader psMainAsm14 = asm {
    ps_1_4
    def c0, 0, 0, 0, 0
    texld r0, t0	// r0 = base
    texld r1, t4	// r1 = right
    texld r2, t3	// r2 = left
    add_x4 r1.w, r2.x, -r1.x	// r1.w = dUD
  + mov r2.xy, r0.r	// r2.xy = base.ar
    mov r2.z, r1.w	// r2.z = r1.w = dUD
    phase
    texld r0, t2	// r0 = down
    texld r1, t1	// r1 = up
    texld r3, r2	// r3 = clut
    mov r2.w, r2.z	// r2.w = dUD
    add_x4 r0.w, r1.x, -r0.x	// r0.w = dLR
    mul_sat r0.w, r0.w, r0.w	// r0.w = dLR*dLR
    mad_sat r0.w, r2.w, -r2.w, 1-r0.w	// r0.w = 1-dUD*dUD-dLR*dLR
    mul r0.xyz, r3, r0.w		// 
  + mul r0.w, r0.w, c0.w
};

technique tec14 {
	pass P0 {
		VertexShader = compile vs_1_1 vsMain();
		//PixelShader = compile ps_1_4 psMain14();
		Sampler[0] = (smpBase);
		Sampler[1] = (smpBase);
		Sampler[2] = (smpBase);
		Sampler[3] = (smpLUT);
		PixelShader = (psMainAsm14);

		ZEnable = False;
		ZWriteEnable = False;
	}
	pass PLast {
		ZEnable = True;
		ZWriteEnable = True;
		Texture[0] = NULL;
	}
}

#endif
#endif


// --------------------------------------------------------------------------
//  1.1 - two passes


struct VS_OUTPUT_11edge {
	float4 pos	: POSITION;
	float2 uv[4] : TEXCOORD0;
};

VS_OUTPUT_11edge vsMain11edge( VS_PUV i )
{
    VS_OUTPUT_11edge o;
	o.pos = i.pos * float4(2,2,1,1);
	float2 uv = i.uv + vFixUV.xy;
	float dx = vFixUV.x*2;
	float dy = vFixUV.y*2;
	o.uv[0] = uv + float2(0,-dy);
	o.uv[1] = uv + float2(0, dy);
	o.uv[2] = uv + float2(-dx,0);
	o.uv[3] = uv + float2( dx,0);
	return o;
}

pixelshader psMainAsm11toon = asm {
	ps_1_1
	def c0, 0, 0, 0, 0
	tex t0 	// t0 = base
	texreg2ar t1, t0 	// t1 = clut
	mov r0.rgb, t1
	+mov r0.a, c0
};
pixelshader psMainAsm11edge = asm {
	ps_1_1
	def c0, 1, 1, 1, 1
	// taps
	tex t0
	tex t1
	tex t2
	tex t3
	// dUD, dLR
	add_x4 r0.w, t2.z, -t3.z
	add_x4 r1.w, t0.z, -t1.z
	mul_sat r0.w, r0, r0
	mad_sat r0, r1.w, -r1.w, 1-r0.w
};


technique tec11 {
	pass P0 {
		VertexShader = compile vs_1_1 vsMain11edge();
		PixelShader = (psMainAsm11edge);
		Sampler[0] = (smpBase);
		Sampler[1] = (smpBase);
		Sampler[2] = (smpBase);
		Sampler[3] = (smpBase);

		ZEnable = False;
		ZWriteEnable = False;
	}
	pass P1 {
		VertexShader = compile vs_1_1 vsBillboard(vFixUV);
		PixelShader = (psMainAsm11toon);
		Sampler[0] = (smpBase);
		Sampler[1] = (smpLUT);

		AlphaBlendEnable = True;
		SrcBlend = Zero;
		DestBlend = SrcColor;
	}
	pass PLast {
		FillMode = Solid;
		ZEnable = True;
		ZWriteEnable = True;
		AlphaBlendEnable = False;
		Texture[0] = NULL;
	}
}
