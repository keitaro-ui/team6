#pragma once
#include <DirectXMath.h>

class StartPoint
{
public:
    DirectX::XMFLOAT3 position;

    StartPoint(const DirectX::XMFLOAT3& pos)
        : position(pos) {
    }
};
