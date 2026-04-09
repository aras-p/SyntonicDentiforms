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

float sampleShadow(vec3 uv)
{
	const vec2 poisson[8] = vec2[](
		vec2(-0.326, -0.406),
		vec2(-0.840, -0.074),
		vec2(-0.696,  0.457),
		vec2(-0.203,  0.621),
		vec2( 0.962, -0.195),
		vec2( 0.473, -0.480),
		vec2( 0.519,  0.767),
		vec2( 0.185, -0.893)
	);
	vec2 scale = 1.5 / vec2(1024.0);

	float sum = 0.0;
	for (int i = 0; i < 8; ++i)
	{
		sum += texture(sampler2DShadow(texShadow, smpShadow), vec3(uv.xy + poisson[i] * scale, uv.z));
	}
	return sum * (1.0 / 8.0);
}

void main()
{
	vec3 uv;
	uv.xyz = uvShadow.xyz / uvShadow.w;

	float shadow = sampleShadow(uv);
	
	float cookie = texture(sampler2D(texCookie, smpCookie), uv.xy).r;
	shadow *= cookie;

	// lighting
	frag_color = gPSLight(normal, tolight, normalize(hlf), shadow);
}
#pragma sokol @end

#pragma sokol @program prog vs fs
