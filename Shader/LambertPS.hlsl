#include "Lambert.hlsli"

#define BASECOLOR_TEXTURE 0
#define NORMAL_TEXTURE 1
#define ROUGHNESS_TEXTURE 2
#define METALNESS_TEXTURE 3
#define EMISIVE_TEXTURE 4
#define OCCLUSION_TEXTURE 5

cbuffer CbMesh : register(b1)
{
    float4 materialColor;

};

//Texture2D DiffuseMap : register(t0);
Texture2D Textures[6] : register(t0);
SamplerState LinearSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{  
	// ガンマ補正係数
    static const float GammaFactor = 2.2f;

    // BaseColor ---------------------------------------------------------    
    float4 color = Textures[BASECOLOR_TEXTURE].Sample(LinearSampler, pin.texcoord) * materialColor;
    color.rgb = pow(color.rgb, GammaFactor);
    
    // Normal ------------------------------------------------------------
    // ノーマルマップから取得し、 ワールド空間へ変換
    float3x3 mat = { normalize(pin.tangent), normalize(pin.bitangent), normalize(pin.normal) };
    float3 N = Textures[NORMAL_TEXTURE].Sample(LinearSampler, pin.texcoord).rgb;
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    
    // emisive -----------------------------------------------------------
    float3 emisive = Textures[EMISIVE_TEXTURE].Sample(LinearSampler, pin.texcoord).rgb;
    
    // Roughness, metalness ----------------------------------------------
    float roughness = Textures[ROUGHNESS_TEXTURE].Sample(LinearSampler, pin.texcoord).r;
    roughness = max(0, roughness);
    
    roughness = max(roughness, 0.3f);

    float metalness = Textures[METALNESS_TEXTURE].Sample(LinearSampler, pin.texcoord).r;
    metalness = max(0, metalness);

    // occlusion ---------------------------------------------------------
    float3 occlusion = Textures[OCCLUSION_TEXTURE].Sample(LinearSampler, pin.texcoord).rrr;
    const float occlusionStrength = 1.0f;
    
    // フレネル反射率の初期値（非金属は最低4%）
    float3 albedo = color.rgb;
    float3 F0 = lerp(0.04f, albedo.rgb, metalness);

    
    // material
    const float4 ka = { 0.5f,0.5f,0.5f,1.0f }; // 環境光の反射率（ambient reflectivity）
    const float4 kd = { 1.0f, 1.0f, 1.0f, 1.0f }; // 拡散反射率（diffuse reflectivity）
    const float4 ks = { 0.3f,0.3f,0.3f, 1.0f }; //スペキュラ反射率（specular reflectivity）

    // texture
    float4 texColor = color;
    float3 baseColor = texColor.rgb;

   
    float3 E = normalize(pin.position.xyz - camera_position.xyz);
    float3 L = normalize(-lightDirection.xyz);
    float3 V = normalize(camera_position.xyz - pin.position.xyz);
    
    float3 H = normalize(V + L);
    float VdotH = saturate(dot(V, H));
    
    // ライティングOFFなら簡易Lambert
    if (useLighting <= 0.5f)
    {
        float power = saturate(dot(N, -L));
        power = power * 0.5f + 0.5f;
        texColor.rgb *= power;
        return texColor;
    }
    
    if (LightSwitch <= 0.5f)
    {
        float power = saturate(dot(N, -L));
        power = power * 0.5f + 0.5f;
        texColor.rgb *= power;
        return texColor;
    }
    
    float ambientStrength = 0.02f;
    float3 ambient = float3(0.1, 0.2f, 0.2f) * ka.rgb;
    //float3 ambient = float3(0.5f,0.5f,0.5f) * ka.rgb;
    
    float3 phong_diffuse = 0;
    float3 phong_specular = 0;
    {
        float3 diffPower = saturate(dot(N, L));
        phong_diffuse = directional_light_color.rgb * diffPower * kd.rgb;

        float3 R = reflect(-L, N);
        float3 specPower = max(dot(E, R), 0);
        specPower = pow(specPower, 16);
        phong_specular = directional_light_color.rgb * specPower * ks.rgb;
      
    }

    // Directional
    float3 directional_diffuse = CalcLambert(N, L, directional_light_color.rgb, kd.rgb) * lightingMultiplier;
    float3 directional_specular = CalcPhongSpecular(N, L, E, directional_light_color.rgb, ks.rgb) * lightingMultiplier;
    
    //Direcvtional PBR
    float3 diffuse_total = 0;
    float3 specular_total = 0;
    {
        float3 L = normalize(-lightDirection.xyz);
        float3 H = normalize(V + L);
        
        float NdotL = saturate(dot(N, L));
        float NdotV = saturate(dot(N, V));
        float NdotH = saturate(dot(N, H));
        float VdotH = saturate(dot(V, H));
        
        float3 diff = DiffuseBRDF(VdotH, F0, albedo) * NdotL;
        float3 spec = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness);
        
        //diffuse_total += diff;
        //specular_total += spec;
    }
    
      
    //点光源の処理
    float3 point_diffuse = 0;
    float3 point_specular = 0;

    for (int i = 0; i < 16; ++i)
    {
        float3 LP = point_light[i].position.xyz - pin.position.xyz;
        float len = length(LP); 
        if (len >= point_light[i].range) continue;
        
        float attenuationLength = saturate(1.0f - len / point_light[i].range); 
        float attenuation = attenuationLength * attenuationLength;
        
        float fadeRange = point_light[i].range - 5.0f;
        fadeRange = max(0, fadeRange);
        float fade = saturate(1.0f - (len - fadeRange) / point_light[i].range);
        attenuation *= fade;
        
        LP /= len;
        
        float3 H = normalize(V + LP);
        float NdotL = saturate(dot(N, LP));
        
        float NdotV = saturate(dot(N, V));
        float NdotH = saturate(dot(N, H));
        float VdotH = saturate(dot(V, H));
        
        float3 diff = DiffuseBRDF(VdotH, F0, albedo) * attenuation;
        float3 spec = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness)*attenuation;
        
        float3 LightColor = point_light[i].color.rgb * attenuation;
        
        diffuse_total += diff * LightColor;
        specular_total += spec * LightColor;
        
        
        point_diffuse += CalcLambert(N, LP, point_light[i].color.rgb, kd.rgb) * attenuation; 
        point_specular += CalcPhongSpecular(N, LP, E, point_light[i].color.rgb, ks.rgb) * attenuation;
    }
    
    // スポットライトの処理
    float3 spot_diffuse = 0;
    float3 spot_specular = 0;
    float aaa = 0;
   
    
    float3 SpotColor = 0;
    if (SpotLightSwitch <= 0.3f)
    {
        for (int j = 0; j < 8; ++j)
        {
            float3 LP = spot_light[j].position.xyz - pin.position.xyz;
            float len = length(LP);
            
            float range = spot_light[j].range;
            
            if(SpotLightSwitch>=0.5f&&SpotLightSwitch<=0.9f)
                range = 0.5f;
        
            float3 L = normalize(-LP);
            float attenuateLength = saturate(1.0f - len / range);
            float attenuation = attenuateLength * attenuateLength;
        
            float3 spotDirection = normalize(spot_light[j].direction.xyz);
            float angle = dot(spotDirection, L);
        
            float spotEffect = smoothstep(spot_light[j].outerCorn, spot_light[j].innerCorn, angle);
            attenuation *= spotEffect;
        
            float3 H = normalize(V + L);
            float NdotL = saturate(dot(N, L));
            float NdotV = saturate(dot(N, V));
            float NdotH = saturate(dot(N, H));
            float VdotH = saturate(dot(V, H));
        
            float3 LightColor = spot_light[j].color.rgb * attenuation * spot_light[0].color.w;
        
            float3 diff = DiffuseBRDF(VdotH, F0, kd.rgb);
            float3 spec = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness);
            
        
        
     
            diffuse_total += diff * LightColor;
            specular_total += spec * LightColor;
        
        }
    }

    
    texColor.rgb = saturate(
    baseColor.rgb *(ambient + point_diffuse + spot_diffuse) + (point_specular + spot_specular));
        
    texColor.a = 1.0f;
    
    float3 finalColor=0;
    
    float Boost = 2.0f;

    
    finalColor.rgb += color.rgb * diffuse_total*Boost;
    finalColor.rgb += specular_total;
    
    finalColor.rgb += emisive; 

    // ガンマ補正を戻す（リニア → sRGB）
    finalColor = pow(finalColor, 1.0f / GammaFactor);


    //return texColor;
    return float4(finalColor.rgb+ambient, color.a);
    
   
}