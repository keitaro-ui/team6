#include "Lambert.hlsli"


cbuffer CbMesh : register(b1)
{
    float4 materialColor;

};

Texture2D DiffuseMap : register(t0);
SamplerState LinearSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    // material
    const float4 ka = { 1.0f, 1.0f, 1.0f, 1.0f }; // 環境光の反射率（ambient reflectivity）
    const float4 kd = { 1.0f, 1.0f, 1.0f, 1.0f }; // 拡散反射率（diffuse reflectivity）
    const float4 ks = { 0.5f,0.5f,0.5f, 1.0f }; //スペキュラ反射率（specular reflectivity）

    // texture
    float4 texColor = DiffuseMap.Sample(LinearSampler, pin.texcoord) * materialColor;
    float3 baseColor = texColor.rgb;
   
    float3 N = normalize(pin.normal);
    float3 E = normalize(pin.position.xyz - camera_position.xyz);
    float3 L = normalize(lightDirection.xyz);
    
    // ライティングOFFなら簡易Lambert
    if (useLighting <= 0.5f)
    {
        float power = saturate(dot(N, -L));
        power = power * 0.5f + 0.5f;
        texColor.rgb *= power;
        return texColor;
    }

     // ambient
    float3 ambient = ambient_color.rgb * ka.rgb;
    
    float3 phong_diffuse = 0;
    float3 phong_specular = 0;
    {
        float diffPower = saturate(dot(N, L));
        phong_diffuse = directional_light_color.rgb * diffPower * kd.rgb;

        float3 R = reflect(-L, N);
        float specPower = max(dot(E, R), 0);
        specPower = pow(specPower, 16);
        phong_specular = directional_light_color.rgb * specPower * ks.rgb;
      
    }

    // Directional
    float3 directional_diffuse = CalcLambert(N, L, directional_light_color.rgb, kd.rgb) * lightingMultiplier;
    float3 directional_specular = CalcPhongSpecular(N, L, E, directional_light_color.rgb, ks.rgb) * lightingMultiplier;
    
      
    //点光源の処理
    float3 point_diffuse = 0;
    float3 point_specular = 0;
    for (int i = 0; i < 16; ++i)
    {
        float3 LP = pin.position.xyz - point_light[i].position.xyz; 
        float len = length(LP); if (len >= point_light[i].range) continue;
        float attenuationLength = saturate(1.0f - len / point_light[i].range); 
        float attenuation = attenuationLength * attenuationLength; LP /= len; 
        point_diffuse += CalcLambert(N, LP, point_light[i].color.rgb, kd.rgb) * attenuation; 
        point_specular += CalcPhongSpecular(N, LP, E, point_light[i].color.rgb, ks.rgb) * attenuation;
    }
    
    // スポットライトの処理
    float3 spot_diffuse = 0;
    float3 spot_specular = 0;
    for (int j = 0; j < 8; ++j)
    {
        float3 LP = pin.position.xyz - spot_light[j].position.xyz; 
        float len = length(LP); 
        if (len >= spot_light[j].range) 
            continue; 
        float attenuateLength = saturate(1.0f - len / spot_light[j].range);
        float attenuation = attenuateLength * attenuateLength; LP /= len; 
        float3 spotDirection = normalize(spot_light[j].direction.xyz); 
        float angle = dot(spotDirection, LP); float area = spot_light[j].innerCorn - spot_light[j].outerCorn;
        attenuation *= saturate(1.0f - (spot_light[j].innerCorn - angle) / area);
        spot_diffuse += CalcLambert(N, LP, spot_light[j].color.rgb, kd.rgb) * attenuation;
        spot_specular += CalcPhongSpecular(N, LP, E, spot_light[j].color.rgb, ks.rgb) * attenuation;
    }

    ambient = kd.rgb * ka.rgb * 0.1f;
    
    texColor.rgb = saturate(
    baseColor.rgb *(ambient + point_diffuse + spot_diffuse) + (point_specular + spot_specular));

    
    texColor.a = 1.0f;
    
    return texColor;
}