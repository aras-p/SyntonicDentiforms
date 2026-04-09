#pragma sokol @module receiverHi

#pragma sokol @block commonlib
#pragma sokol @include _lib.fx
#pragma sokol @end

#pragma sokol @vs vs
#pragma sokol @include_block commonlib
layout(location=0) in vec3 va_position;
layout(location=1) in vec2 va_normal;

out vec4 uvShadow;
out vec3 hlf;
out vec3 normal;
out vec3 tolight;

void main()
{
	gl_Position = mWVP * vec4(va_position, 1.0);
	vec4 uvs = mWorldView * vec4(va_position, 1.0);
	uvShadow = mShadowProj * uvs;

	gVSLightTerms(va_position, va_normal, mWorld, normal, tolight, hlf);
}
#pragma sokol @end

#pragma sokol @fs fs
#pragma sokol @include_block commonlib

layout(binding=0) uniform texture2D texShadow;
layout(binding=1) uniform texture2D texCookie;
layout(binding=0) uniform sampler smpShadow;
layout(binding=1) uniform sampler smpCookie;

in vec4 uvShadow;
in vec3 hlf;
in vec3 normal;
in vec3 tolight;
out vec4 frag_color;
void main()
{
	vec3 uv;
	uv.xyz = uvShadow.xyz / uvShadow.w;

	float shadow = 0.0;
	shadow += texture(sampler2DShadow(texShadow, smpShadow), uv);
	//shadow /= 9.0;
	
	float cookie = texture(sampler2D(texCookie, smpCookie), uv.xy).r;
	shadow *= cookie;

	// lighting
	frag_color = gPSLight(normal, tolight, normalize(hlf), shadow);
}
#pragma sokol @end

#pragma sokol @program prog vs fs
