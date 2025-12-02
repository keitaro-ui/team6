#include "Lambert.hlsli"
#include "Skinning.hlsli"

VS_OUT main(
	float4 position : POSITION,
	float4 boneWeights : BONE_WEIGHTS,
	uint4 boneIndices : BONE_INDICES,
	float2 texcoord : TEXCOORD,
	float3 normal : NORMAL,
    float3 tangent : TANGENT,
    float3 binormal : BINORMAL)
{
    VS_OUT vout = (VS_OUT) 0;

    //position = SkinningPosition(position, boneWeights, boneIndices);
    //vout.position = position;
    //vout.vertex = mul(position, viewProjection);
    //vout.texcoord = texcoord;
    //vout.normal = SkinningVector(normal, boneWeights, boneIndices);

    //return vout;
    
    
      // スキニング
    position = SkinningPosition(position, boneWeights, boneIndices);
    
    //vout.normal = normalize(SkinningVector(normal, boneWeights, boneIndices));
    vout.normal = normalize(vout.normal);
    
    vout.tangent = normalize(SkinningVector(tangent, boneWeights, boneIndices));
    vout.bitangent = normalize(SkinningVector(binormal, boneWeights, boneIndices));

    // 出力
    vout.position = position;
    vout.vertex = mul(position, viewProjection);
    vout.texcoord = texcoord;

    return vout;
}
