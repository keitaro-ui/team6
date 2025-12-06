#pragma once
#include <DirectXMath.h>
#include "Edge.h"
#include "System/ModelRenderer.h"
#include "System/ShapeRenderer.h"
#include <vector>

#define MAX_WAY_POINT 42


enum class AreaAttribute
{
	AreaA,
	AreaB,
	AreaC,
	AreaD,
};

// ウェイポイント用クラス(いわゆるノード)
class WayPoint
{
public:
	DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	WayPoint();

	~WayPoint();
	void Render(const RenderContext& rc, ModelRenderer* renderer);
	bool searchFg = false;
	
	float costFromStart = -1.0f;

	AreaAttribute area;

private:
	int EdgeNo = 4;
	std::vector<WayPoint*>		wayPoints;
	std::unique_ptr<ShapeRenderer> shape = nullptr;


public:
	Edge* edge[4]; // 各ノードをつなげるエッジ
	int GetEdgeNo() { return EdgeNo; }
};
