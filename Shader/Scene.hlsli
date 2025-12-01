
cbuffer CbScene : register(b0)
{
	row_major float4x4	viewProjection;
	float4				lightDirection;
    float4				camera_position;
    float lightingMultiplier;
    float useLighting;
    float padding[2];
};

// 点光源
struct point_lights
{
    float4 position;
    float4 color;
    float range;
    float3 dummy;
};

// スポットライト
struct spot_lights
{
    float4 position;
    float4 direction;
    float4 color;
    float range;
    float innerCorn;
    float outerCorn;
    float dummy;
};

cbuffer LIGHT_CONSTANT_BUFFER : register(b2)
{
    float4 ambient_color;
    float4 directional_light_direction;
    float4 directional_light_color;
    point_lights point_light[16];
    spot_lights spot_light[8];
};



#include "LightingFunctions.hlsli"