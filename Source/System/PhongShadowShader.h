#pragma once
#include "Shader.h"

class PhongShadowshader : public Shader
{
public:
	PhongShadowshader(ID3D11Device* device);
	~PhongShadowshader() override = default;

	void Begin(const RenderContext& rc) override;
	void Update(const RenderContext& rc, const ModelResource::Material& material) override;
	void End(const RenderContext& rc) override;

private:
	struct CbMaterial
	{
		DirectX::XMFLOAT4 materialColor;
		DirectX::XMFLOAT4X4 LightViewProjection;
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixselShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr <ID3D11Buffer> constantBuffer;
};