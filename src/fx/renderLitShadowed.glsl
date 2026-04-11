#pragma sokol @module renderLitShadowed

#pragma sokol @block commonlib
#pragma sokol @include lib_lighting.glsl
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
    vec4 worldPos = mWorld * vec4(va_position, 1.0);
    gl_Position = mViewProj * worldPos;
	vec4 uvs = mView * worldPos;
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

vec3 finalShadowCoord(vec2 baseUV, vec2 deltaUV, float depth, vec3 receiverPlaneDepthBias)
{
    vec3 uv = vec3(baseUV + deltaUV, depth + receiverPlaneDepthBias.z);
    uv.z += dot(deltaUV, receiverPlaneDepthBias.xy);
    return uv;
}

float sampleShadow(vec3 coord)
{
	vec3 receiverPlaneDepthBias = vec3(0.0); // just don't use receiver plane bias for now

	// optimized 5x5 PCF filter by Ignacio Castano:
	// https://www.ludicon.com/castano/blog/articles/shadow-mapping-summary-part-1/

	float shadowMapSize = 1024.0;
	vec2 texelSize = 1.0 / vec2(shadowMapSize);
    vec2 offset = vec2(0.5, 0.5);
    vec2 uv = coord.xy * shadowMapSize + offset;
    vec2 base_uv = (floor(uv) - offset) * texelSize.xy;
    vec2 st = fract(uv);

    vec3 uw = vec3(4 - 3 * st.x, 7, 1 + 3 * st.x);
    vec3 u = vec3((3 - 2 * st.x) / uw.x - 2, (3 + st.x) / uw.y, st.x / uw.z + 2);
    u *= texelSize.x;

    vec3 vw = vec3(4 - 3 * st.y, 7, 1 + 3 * st.y);
    vec3 v = vec3((3 - 2 * st.y) / vw.x - 2, (3 + st.y) / vw.y, st.y / vw.z + 2);
    v *= texelSize.y;

    float sum = 0.0f;

    vec3 accum = uw * vw.x;
    sum += accum.x * texture(sampler2DShadow(texShadow, smpShadow), finalShadowCoord(base_uv, vec2(u.x, v.x), coord.z, receiverPlaneDepthBias));
    sum += accum.y * texture(sampler2DShadow(texShadow, smpShadow), finalShadowCoord(base_uv, vec2(u.y, v.x), coord.z, receiverPlaneDepthBias));
    sum += accum.z * texture(sampler2DShadow(texShadow, smpShadow), finalShadowCoord(base_uv, vec2(u.z, v.x), coord.z, receiverPlaneDepthBias));

    accum = uw * vw.y;
    sum += accum.x * texture(sampler2DShadow(texShadow, smpShadow), finalShadowCoord(base_uv, vec2(u.x, v.y), coord.z, receiverPlaneDepthBias));
    sum += accum.y * texture(sampler2DShadow(texShadow, smpShadow), finalShadowCoord(base_uv, vec2(u.y, v.y), coord.z, receiverPlaneDepthBias));
    sum += accum.z * texture(sampler2DShadow(texShadow, smpShadow), finalShadowCoord(base_uv, vec2(u.z, v.y), coord.z, receiverPlaneDepthBias));

    accum = uw * vw.z;
    sum += accum.x * texture(sampler2DShadow(texShadow, smpShadow), finalShadowCoord(base_uv, vec2(u.x, v.z), coord.z, receiverPlaneDepthBias));
    sum += accum.y * texture(sampler2DShadow(texShadow, smpShadow), finalShadowCoord(base_uv, vec2(u.y, v.z), coord.z, receiverPlaneDepthBias));
    sum += accum.z * texture(sampler2DShadow(texShadow, smpShadow), finalShadowCoord(base_uv, vec2(u.z, v.z), coord.z, receiverPlaneDepthBias));
    return sum / 144.0f;
}

void main()
{
	vec3 uv;
	uv.xyz = uvShadow.xyz / uvShadow.w;
	#if SOKOL_GLSL
	uv.y = 1.0 - uv.y;
	#endif

    float shadow = 0.0;
    if (uv.z > 0.0)
    {
        float cookie = texture(sampler2D(texCookie, smpCookie), uv.xy).r;
        if (cookie > 0.0)
        {
            shadow = cookie * sampleShadow(uv);
        }
    }
	// lighting
	frag_color = gPSLight(normalize(normal), normalize(tolight), normalize(hlf), shadow);
}
#pragma sokol @end

#pragma sokol @program prog vs fs
