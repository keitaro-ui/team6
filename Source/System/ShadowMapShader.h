#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "Shader.h"
#include "ModelResource.h"
#include "RenderState.h"

class ShadowMapShader : public Shader
{
public:
    explicit ShadowMapShader(ID3D11Device* device);
    void Begin(const RenderContext& rc) override;
    void Update(const RenderContext& rc, const ModelResource::Material& material) override;
    void End(const RenderContext& rc) override;

private:
    struct CbMaterial { DirectX::XMFLOAT4 materialColor; };
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
};

