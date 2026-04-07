// 1 sample shadows, vertex lighting

#pragma sokol @module receiverLo

#pragma sokol @block commonlib
#pragma sokol @include _lib.fx
#pragma sokol @end

#pragma sokol @vs vs
#pragma sokol @include_block commonlib
layout(location=0) in vec3 va_position;
layout(location=1) in vec3 va_normal;

out vec4 uvTex;
out vec4 diffuse; // diffuse+specular in RGB, specular in A

void main()
{
	gl_Position = mWVP * vec4(va_position, 1.0);
	vec4 uvs = mWorldView * vec4(va_position, 1.0);
	uvTex = mShadowProj * uvs;

	// lighting
	vec3 n, light, hlf;
	gVSLightTerms(va_position, va_normal, mWorld, n, light, hlf );
	diffuse = gPSLight(n, light, hlf, 1.0);
}
#pragma sokol @end

#pragma sokol @fs fs
#pragma sokol @include_block commonlib

layout(binding=0) uniform texture2D texShadow;
layout(binding=1) uniform texture2D texCookie;
layout(binding=0) uniform sampler smpShadow;
layout(binding=1) uniform sampler smpCookie;

in vec4 uvTex;
in vec4 diffuse;
out vec4 frag_color;
void main()
{
	float shadow = textureProj(sampler2D(texShadow, smpShadow), uvTex).r == vShadowID.r ? 1.0 : 0.0;
	float cookie = textureProj(sampler2D(texCookie, smpCookie), uvTex).r;

	frag_color = diffuse * (shadow * cookie);
}
#pragma sokol @end

#pragma sokol @program prog vs fs
