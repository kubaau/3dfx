#ifndef States_hlsl
#define States_hlsl

SamplerState linear_sampler : register(s0);
SamplerState anisotropic_sampler : register(s1);
SamplerState point_sampler : register(s2);
SamplerState point_linear_sampler : register(s3);
SamplerState shadow_sampler : register(s4);

#endif
