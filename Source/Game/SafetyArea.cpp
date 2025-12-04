#include "SafetyArea.h"
#include "../System/Graphics.h"
#include "Balloon.h"
#include "Projectile.h"
#include <DirectXMath.h>
#include <imgui.h>


using namespace DirectX;

SafetyArea::SafetyArea(ProjectileManager* manager) : Projectile(manager)
{
    /*position = { 0.0f, 0.0f, 0.0f };
    XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());
    balloonModel = new Model("Data/Model/Target/balloon.mdl");
    scale = { 0.6f, 0.6f, 0.6f };
    assert(balloonModel && "balloonModel が読み込めません");
    type = ProjectileType::Safetiy;*/
    position = { 0.0f, 0.5f, 0.0f };
    radius = 5.0f;

    XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());

    balloonModel = new Model("Data/Model/Target/balloon.mdl");

    scale = { 1.0f, 1.0f, 1.0f };
}


SafetyArea::~SafetyArea()
{
    if (balloonModel)
    {
        delete balloonModel;
        balloonModel = nullptr;
    }
}

void SafetyArea::SetPosition(const XMFLOAT3& pos)
{
    position = pos;
    UpdateTransform();
}

bool SafetyArea::IsInside(const DirectX::XMFLOAT3& p)
{
    float dx = p.x - position.x;
    float dz = p.z - position.z;

    float distSq = dx * dx + dz * dz;

    return distSq < (radius * radius);

}

//void SafetyArea::UpdateTransform()
//{
//    using namespace DirectX;
//    XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
//    XMMATRIX T = XMMatrixTranslation(position.x, position.y, position.z);
//    XMStoreFloat4x4(&transform, S * T);
//}


void SafetyArea::Update(float deltaTime)
{
    // 風船は少し上下にふわふわ
    position.y = 0.5f + 0.1f * sinf(static_cast<float>(GetTickCount64()) * 0.005f);

    UpdateTransform();
    balloonModel->UpdateTransform();
}

void SafetyArea::Render(const RenderContext& rc, ModelRenderer* renderer)
{
   
    if (!balloonModel || !renderer) return;
    renderer->Render(rc, transform, balloonModel, ShaderId::Lambert);
    //RenderDebugPrimitive(rc, renderer);
}

void SafetyArea::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
    // 地面に円を描く（安全エリア範囲）
    renderer->RenderCircle(rc,
        { position.x, 0.01f, position.z },
        radius,
        { 1.0f, 1.0f, 0.0f, 1.0f }
    );
}