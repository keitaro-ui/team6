#pragma once
#include <DirectXMath.h>
#include "Edge.h"
#define ROW_COUNT 20
#define COLUM_COUNT 20
#define MAX_WAY_POINT 400

const int EdgeNo = 8;

// ウェイポイント用クラス(いわゆるノード)
class WayPoint
{
public:
	DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	WayPoint();

	~WayPoint();
	bool searchFg = false;
	Edge* edge[8]; // 各ノードをつなげるエッジ
	float costFromStart = -1.0f;
};
