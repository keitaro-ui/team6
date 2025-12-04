#pragma once
#include <DirectXMath.h>

class GoalPoint
{
public:
    DirectX::XMFLOAT3 position;
    float radius = 1.0f; // ÚG”»’è—p‚Ì”¼Œa

    GoalPoint(const DirectX::XMFLOAT3& pos)
        : position(pos) {
    }
};