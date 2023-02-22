cbuffer lightBuffer : register(b0)
{
	float3 ambientLightColor;
	float ambientLightStrength;

	float3 LightColor;
	float LightStrength;
	float3 LightPosition;

	float LightAttenuation_a;
	float LightAttenuation_b;
	float LightAttenuation_c;
}

struct PS_INPUT
{
	float4 inPos : SV_POSITION;
	float2 inTexCoord : TEXCOORD;
	float3 inNormal : NORMAL;
	float3 inWorldPos : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE: register(t0);
SamplerState objSamplerState : SAMPLER: register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 objectColor = objTexture.Sample(objSamplerState, input.inTexCoord);

	float3 ambientLight = ambientLightColor * ambientLightStrength;
	float3 light = ambientLight;
	float3 vectorToLight = normalize(LightPosition - input.inWorldPos);
	float3 diffuseLightIntensity = max(dot(vectorToLight, input.inNormal), 0);
	float distanceToLight = distance(LightPosition, input.inWorldPos);
	float attenuationFactor = 1 / (LightAttenuation_a + LightAttenuation_b * distanceToLight + LightAttenuation_c * pow(distanceToLight, 2));
	diffuseLightIntensity *= attenuationFactor;
	float3 diffuseLight = diffuseLightIntensity * LightStrength * LightColor;
	light += diffuseLight;

	float3 color = objectColor * light;
	return float4(color, 1.0f);
}