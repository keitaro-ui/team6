#pragma once

#include<DirectXMath.h>
#include<DirectXCollision.h>
#include <System/ModelResource.h>
#include <System/Model.h>

class Raycast
{
public:
	// レイキャスト
	static bool Raycast::RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const DirectX::XMFLOAT4X4& worldTransform,
		const Model* model,
		DirectX::XMFLOAT3& hitPosition,
		DirectX::XMFLOAT3& hitNormal);

};
