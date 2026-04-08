// 4 sample robust objectid shadows

#pragma sokol @module receiverHi

#pragma sokol @block commonlib
#pragma sokol @include _lib.fx
#pragma sokol @end

#pragma sokol @vs vs
#pragma sokol @include_block commonlib
layout(location=0) in vec3 va_position;
layout(location=1) in vec3 va_normal;

out vec3 uvShadow;
out vec3 hlf;
out vec3 normal;
out vec3 tolight;

void main()
{
	gl_Position = mWVP * vec4(va_position, 1.0);
	vec4 uvs = mWorldView * vec4(va_position, 1.0);
	uvShadow = (mShadowProj * uvs).xyz;

	gVSLightTerms(va_position, va_normal, mWorld, normal, tolight, hlf);
}
#pragma sokol @end

#pragma sokol @fs fs
#pragma sokol @include_block commonlib

layout(binding=0) uniform texture2D texShadow;
layout(binding=1) uniform texture2D texCookie;
layout(binding=0) uniform sampler smpShadow;
layout(binding=1) uniform sampler smpCookie;

in vec3 uvShadow;
in vec3 hlf;
in vec3 normal;
in vec3 tolight;
out vec4 frag_color;
void main()
{
	float offset = 1.0 / 1024.0;
	//@TODO: gather4
	vec2 uv = uvShadow.xy / uvShadow.z;

	// Robust ObjectID shadows (ShaderX^2)
	vec2 uv00 = uv + vec2(      0,      0 );
	vec2 uv01 = uv + vec2(      0, offset );
	vec2 uv10 = uv + vec2( offset,      0 );
	vec2 uv11 = uv + vec2( offset, offset );
	vec4 ids = vec4(
		texture(sampler2D(texShadow, smpShadow), uv00).r,
		texture(sampler2D(texShadow, smpShadow), uv01).r,
		texture(sampler2D(texShadow, smpShadow), uv10).r,
		texture(sampler2D(texShadow, smpShadow), uv11).r);
	bvec4 compare = equal(ids, vShadowID.rrrr);	
	float shadow = any(compare) ? 1.0 : 0.0;
	
	float cookie = texture(sampler2D(texCookie, smpCookie), uv).r;
	shadow *= cookie;

	// lighting
	frag_color = gPSLight(normal, tolight, normalize(hlf), shadow);
}
#pragma sokol @end

#pragma sokol @program prog vs fs
