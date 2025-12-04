#pragma once
#include "System/Graphics.h"
#include "Projectile.h"
#include <DirectXMath.h>

class SafetyArea : public Projectile
{
public:
    SafetyArea(ProjectileManager* manager);
  
    
    ~SafetyArea();

    // 既存の仮想関数
    virtual void Update(float elapsedTime) override;
    virtual void Render(const RenderContext& rc, ModelRenderer* renderer) override;
    virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override; // ← 追加

    // SetPosition メソッドを追加
    void SetPosition(const DirectX::XMFLOAT3& pos);

    bool IsInside(const DirectX::XMFLOAT3& p);

private:
    struct SafetyAreaData
    {
        DirectX::XMFLOAT3 pos;
        float radius;
    };

    Model* balloonModel = nullptr;
    float radius = 2.0f;   // 安全エリアの半径
    std::vector<SafetyArea*> safetyAreas;

    /*DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 scale = {1.0f, 1.0f, 1.0f};
    DirectX::XMFLOAT4X4 transform;
    void UpdateTransform();
    void Destroy();*/
};