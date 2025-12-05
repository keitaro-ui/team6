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


    void SetAngle(const DirectX::XMFLOAT3& angle);


    bool IsInside(const DirectX::XMFLOAT3& p);

<<<<<<< HEAD
    void SafetyArea::circleUpdateTransform();
=======
>>>>>>> 02765a96f2fda4e84d8a43890c7703940332ad51

private:
    struct SafetyAreaData
    {
        DirectX::XMFLOAT3 pos;
        float radius;
    };

    Model* balloonModel = nullptr;
    float radius = 2.0f;   // 安全エリアの半径
    std::vector<SafetyArea*> safetyAreas;

    std::unique_ptr<Model>  circle;
    DirectX::XMFLOAT3		circlePosition = { 0, 0, 0 };
    DirectX::XMFLOAT3		circleDirection = { 0, 0, 0 };
    DirectX::XMFLOAT3		circleScale = { 1, 1, 1 };
    DirectX::XMFLOAT4X4		circleTransform =
    {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1 
    };
    float sc;

    /*DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 scale = {1.0f, 1.0f, 1.0f};
    DirectX::XMFLOAT4X4 transform;
    void UpdateTransform();
    void Destroy();*/
};