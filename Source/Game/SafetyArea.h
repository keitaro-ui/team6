#pragma once
#include "System/Graphics.h"
#include "Projectile.h"
#include <DirectXMath.h>

class SafetyArea : public Projectile
{
public:
<<<<<<< HEAD
    SafetyArea(ProjectileManager* manager)
        : Projectile(manager)
        /*, position{0.0f, 0.0f, 0.0f}
        , transform{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}*/
    {
        position = { 0.0f, 0.0f, 0.0f };
        XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());
        balloonModel = new Model("Data/Model/Target/flag.mdl");
        scale={0.02f, 0.02f, 0.02f};
        assert(balloonModel && "balloonModel が読み込めません");
        type = ProjectileType::Safetiy;
    }
=======
    SafetyArea(ProjectileManager* manager);
  
>>>>>>> master
    
    ~SafetyArea();

    // 既存の仮想関数
    virtual void Update(float elapsedTime) override;
    virtual void Render(const RenderContext& rc, ModelRenderer* renderer) override;
    virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override; // ← 追加

    // SetPosition メソッドを追加
    void SetPosition(const DirectX::XMFLOAT3& pos);

<<<<<<< HEAD
    void SetAngle(const DirectX::XMFLOAT3& angle);

=======
    bool IsInside(const DirectX::XMFLOAT3& p);
>>>>>>> master

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