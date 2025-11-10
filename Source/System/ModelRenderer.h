#pragma once

#include <memory>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Model.h"
#include "Shader.h"

enum class ShaderId
{
	Basic,
	Lambert,
	Phong,
	Shadow,
	EnumCount
};

class ModelRenderer
{
public:
	ModelRenderer(ID3D11Device* device);
	~ModelRenderer() {}

	// ï`âÊé¿çs
	void Render(const RenderContext& rc, const DirectX::XMFLOAT4X4& worldTransform, const Model* model, ShaderId shaderId);

private:
	struct CbScene
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		lightDirection;
		DirectX::XMFLOAT4 cameraPosition;
		DirectX::XMFLOAT4X4 lightViewProjection;
	};

	struct CbSkeleton
	{
		DirectX::XMFLOAT4X4		boneTransforms[256];
	};

	struct LightConstants
	{
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT4 directionalLightColor;
		DirectX::XMFLOAT4 directionalLightDirection;
	};

	struct ShadowMapConstants
	{
		DirectX::XMFLOAT4 shadowColor;
		float shadowBias;
		DirectX::XMFLOAT3 padding;
	};

	DirectX::XMFLOAT4 ambientColor = { 0.2f,0.2f,0.2f,1.0f };
	DirectX::XMFLOAT4 directionalLightColor = { 1,1,1,1 };
	DirectX::XMFLOAT4 directionalLightDirection = { -0.5f,-1,-0.3f,0 };

	DirectX::XMFLOAT4 shadowColor = { 0,0,0,1 };
	float shadowBias = 0.005f;


	std::unique_ptr<Shader>					shaders[static_cast<int>(ShaderId::EnumCount)];

	Microsoft::WRL::ComPtr<ID3D11Buffer>	sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	skeletonConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowMapConstantBuffer;
};
