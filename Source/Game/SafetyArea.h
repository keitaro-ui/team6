#pragma once
#include "System/Graphics.h"
#include "Projectile.h"
#include <DirectXMath.h>

class SafetyArea : public Projectile
{
public:
    SafetyArea(ProjectileManager* manager)
        : Projectile(manager)
        /*, position{0.0f, 0.0f, 0.0f}
        , transform{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}*/
    {
        position = { 0.0f, 0.0f, 0.0f };
        XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());
        balloonModel = new Model("Data/Model/Target/balloon.mdl");
        assert(balloonModel && "balloonModel が読み込めません");
    }
    
    ~SafetyArea();
    // 既存の仮想関数
    virtual void Update(float elapsedTime) override;
    virtual void Render(const RenderContext& rc, ModelRenderer* renderer) override;
    virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override; // ← 追加

    // SetPosition メソッドを追加
    void SetPosition(const DirectX::XMFLOAT3& pos);


private:
    struct SafetyAreaData
    {
        DirectX::XMFLOAT3 pos;
        float radius;
    };

    Model* balloonModel = nullptr;
    float radius = 2.0f;   // 安全エリアの半径
    /*DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 scale = {1.0f, 1.0f, 1.0f};
    DirectX::XMFLOAT4X4 transform;
    void UpdateTransform();
    void Destroy();*/
};