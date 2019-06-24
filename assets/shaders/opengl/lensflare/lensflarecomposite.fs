//#include "shaders/def.glsl"

#version 410 core

noperspective in vec2 vTexCoord;

uniform sampler2D uColorTexture;
uniform sampler2D txFeatures;
uniform sampler2D txLensDirt;
uniform sampler2D txStarburst;

uniform float uGlobalBrightness;
uniform float uStarburstOffset;

layout(location=0) out vec4 fResult;

struct DrawArraysIndirectCmd
{
	uint m_count;
	uint m_primCount;
	uint m_first;
	uint m_baseInstance;
};
struct DrawElementsIndirectCmd
{
	uint m_count;
	uint m_primCount;
	uint m_firstIndex;
	uint m_baseVertex;
	uint m_baseInstance;
};
struct DispatchIndirectCmd
{
	uint m_groupsX;
	uint m_groupsY;
	uint m_groupsZ;
};

#define CONCATENATE_TOKENS(_a, _b) _a ## _b

// Use for compile-time branching based on memory qualifiers - useful for buffers defined in common files, e.g.
// #define FooMemQualifier readonly
// layout(std430) FooMemQualifier buffer _bfFoo {};
// #if (MemoryQualifier(FooMemQualifier) == MemoryQualifier(readonly))
#define MemoryQualifier_            0
#define MemoryQualifier_readonly    1
#define MemoryQualifier_writeonly   2
#define MemoryQualifier(_qualifier) CONCATENATE_TOKENS(MemoryQualifier_, _qualifier)


#define Gamma_kExponent 2.2
float Gamma_Apply(in float _x)
{
	return pow(_x, Gamma_kExponent);
}
vec2 Gamma_Apply(in vec2 _v)
{
	return vec2(Gamma_Apply(_v.x), Gamma_Apply(_v.y));
}
vec3 Gamma_Apply(in vec3 _v)
{
	return vec3(Gamma_Apply(_v.x), Gamma_Apply(_v.y), Gamma_Apply(_v.z));
}
vec4 Gamma_Apply(in vec4 _v)
{
	return vec4(Gamma_Apply(_v.x), Gamma_Apply(_v.y), Gamma_Apply(_v.z), Gamma_Apply(_v.w));
}
float Gamma_ApplyInverse(in float _x)
{
	return pow(_x, 1.0/Gamma_kExponent);
}
vec2 Gamma_ApplyInverse(in vec2 _v)
{
	return vec2(Gamma_ApplyInverse(_v.x), Gamma_ApplyInverse(_v.y));
}
vec3 Gamma_ApplyInverse(in vec3 _v)
{
	return vec3(Gamma_ApplyInverse(_v.x), Gamma_ApplyInverse(_v.y), Gamma_ApplyInverse(_v.z));
}
vec4 Gamma_ApplyInverse(in vec4 _v)
{
	return vec4(Gamma_ApplyInverse(_v.x), Gamma_ApplyInverse(_v.y), Gamma_ApplyInverse(_v.z), Gamma_ApplyInverse(_v.w));
}

// Constants
#define kPi                          (3.14159265359)
#define k2Pi                         (6.28318530718)
#define kHalfPi                      (1.57079632679)

#define Color_Black                  vec3(0.0)
#define Color_White                  vec3(1.0)
#define Color_Red                    vec3(1.0, 0.0, 0.0)
#define Color_Green                  vec3(0.0, 1.0, 0.0)
#define Color_Blue                   vec3(0.0, 0.0, 1.0)
#define Color_Magenta                vec3(1.0, 0.0, 1.0)
#define Color_Yellow                 vec3(1.0, 1.0, 0.0)
#define Color_Cyan                   vec3(0.0, 1.0, 1.0)

// Functions
#define saturate(_x)                 clamp((_x), 0.0, 1.0)
#define length2(_v)                  dot(_v, _v)
#define sqrt_safe(_x)                sqrt(max(_x, 0.0))
#define length_safe(_v)              sqrt_safe(dot(_v, _v))
#define log10(x)                     (log2(x) / log2(10.0))
#define linearstep(_e0, _e1, _x)     saturate((_x) * (1.0 / ((_e1) - (_e0))) + (-(_e0) / ((_e1) - (_e0))))
#define max3(_a, _b, _c)             max(_a, max(_b, _c))
#define min3(_a, _b, _c)             min(_a, min(_b, _c))
#define max4(_a, _b, _c, _d)         max(_a, max(_b, max(_c, _d)))
#define min4(_a, _b, _c, _d)         min(_a, min(_b, min(_c, _d)))

// Recover view space depth from a depth buffer value given a perspective projection.
// This may return INF for infinite perspective projections.
float GetDepthV_Perspective(in float _depth, in mat4 _proj)
{
	#if FRM_NDC_Z_ZERO_TO_ONE
		float zndc = _depth;
	#else
		float zndc = _depth * 2.0 - 1.0;
	#endif
	return _proj[3][2] / (_proj[2][3] * zndc - _proj[2][2]);
}
// Recover view space depth from a depth buffer value given an orthographic projection.
// This may return INF for infinite perspective projections.
float GetDepthV_Orthographic(in float _depth, in mat4 _proj)
{
	#if FRM_NDC_Z_ZERO_TO_ONE
		float zndc = _depth;
	#else
		float zndc = _depth * 2.0 - 1.0;
	#endif
	return (zndc - _proj[3][2]) / _proj[2][2];
}

vec3 TransformPosition(in mat4 _m, in vec3 _v)
{
	return (_m * vec4(_v, 1.0)).xyz;
}
vec2 TransformPosition(in mat3 _m, in vec2 _v)
{
	return (_m * vec3(_v, 1.0)).xy;
}
vec3 TransformDirection(in mat4 _m, in vec3 _v)
{
	return mat3(_m) * _v;
}
vec2 TransformDirection(in mat3 _m, in vec2 _v)
{
	return (_m * vec3(_v, 0.0)).xy;
}

void main() 
{
 // starburst
	vec2 centerVec = vTexCoord - vec2(0.5);
	float d = length(centerVec);
	float radial = acos(centerVec.x / d);
	float mask = 
		  texture(txStarburst, vec2(radial + uStarburstOffset * 1.0, 0.0)).r
		* texture(txStarburst, vec2(radial - uStarburstOffset * 0.5, 0.0)).r
		;
	mask = saturate(mask + (1.0 - smoothstep(0.0, 0.3, d)));
	
 // lens dirt
	mask *= textureLod(txLensDirt, vTexCoord, 0.0).r;
	
	//fResult = vec4(textureLod(txFeatures, vTexCoord, 0.0).rgb, 1.0);
	//fResult = vec4(textureLod(txFeatures, vTexCoord, 0.0).rgb * mask * uGlobalBrightness, 1.0);
	fResult = texture(uColorTexture, vTexCoord) + vec4(textureLod(txFeatures, vTexCoord, 0.0).rgb * mask * uGlobalBrightness, 1.0);
	//fResult = texture(uColorTexture, vTexCoord) / 2.0f + vec4(textureLod(txFeatures, vTexCoord, 0.0).rgb * uGlobalBrightness, 1.0);
	//fResult = vec4(1,0,0,1);
}