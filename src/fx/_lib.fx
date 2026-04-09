
// Match GlobalUniforms in C++
layout(binding=0) uniform global_uniforms {
	mat4 mView;
	mat4 mProjection;
	mat4 mViewProj;
	mat4 mViewTexProj;
	mat4 mShadowProj;
	vec4 vEye;
	vec4 vLightPos;
	vec4 vLightDir;
	vec4 _pad0;
};

// Match EntityUniforms in C++
layout(binding=1) uniform entity_uniforms_vs {
	mat4 mWorld;
	mat4 mWorldView;
	mat4 mWVP;
};
layout(binding=2) uniform entity_uniforms_fs {
	vec4 vShadowID;
};

// --------------------------------------------------------------------------
// light

const float	fLightDiffuse = 0.5;
const float	fLightK = 16;


// --------------------------------------------------------------------------
//  common lighting code

vec3 gUnpackOctahedralNormal(vec2 f)
{
    vec3 n = vec3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
    float t = max(-n.z, 0.0);
	n.x += (n.x >= 0.0) ? -t : t;
    n.y += (n.y >= 0.0) ? -t : t;
    return normalize(n);
}

void gVSLightTerms(vec3 p, vec2 n_oct, mat4 w, out vec3 on, out vec3 ol, out vec3 oh )
{
	vec3 n = gUnpackOctahedralNormal(n_oct);
	vec3 pos = (w * vec4(p,1.0)).xyz;
	vec3 view = normalize( vEye.xyz - pos );
	vec3 light = normalize( vLightPos.xyz - pos );
	oh = normalize( (view+light)*0.5 );
	ol = light;
	on = mat3(w) * n;
}

vec4 gPSLight( vec3 n, vec3 l, vec3 h, float shadow )
{
	float diff = max(0.0, dot( n, l ) ) * fLightDiffuse;
	float spec = pow( max(0.0, dot( h, n )), fLightK );
	vec4 res;
	res.rgb = vec3(diff + spec);
	res.a = spec;
	return res * shadow;
}
