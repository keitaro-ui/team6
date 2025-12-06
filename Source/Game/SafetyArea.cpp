#include "SafetyArea.h"
#include "../System/Graphics.h"
#include "Balloon.h"
#include "Projectile.h"
#include <DirectXMath.h>
#include <imgui.h>


using namespace DirectX;

SafetyArea::SafetyArea(ProjectileManager* manager) : Projectile(manager)
{
    //balloon
	{
        balloonModel = new Model("Data/Model/Target/flag.mdl");
	    position = { 0.0f, 3.0f, 0.0f };
	    radius = 5.0f;
        scale = { 0.02f,0.02f,0.02f };
	}
    XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());

    //circle
	{
		circle = std::make_unique<Model>("Data/Model/circle.mdl");
        circlePosition = {0.0f, 1.1f, 0.0f};
        sc = 0.05f;
        circleScale = {sc, 0.0001f, sc};
	}
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
    circlePosition.x = position.x;
    circlePosition.z = position.z;
    UpdateTransform();
}


void SafetyArea::SetAngle(const XMFLOAT3& angle)
{
    direction = angle;
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
    //position.y = 0.5f + 0.1f * sinf(static_cast<float>(GetTickCount64()) * 0.005f);

    UpdateTransform();
    circleUpdateTransform();
    balloonModel->UpdateTransform();
}

void SafetyArea::Render(const RenderContext& rc, ModelRenderer* renderer)
{
   
    if (!balloonModel || !renderer) return;
    renderer->Render(rc, transform, balloonModel, ShaderId::Lambert);
    renderer->Render(rc, circleTransform, circle.get(), ShaderId::Lambert);
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

//行列更新処理
void SafetyArea::circleUpdateTransform()
{
    DirectX::XMVECTOR Front, Up, Right;

    //前ベクトルを算出
    Front = DirectX::XMLoadFloat3(&circleDirection);
    Front = DirectX::XMVector3Normalize(Front);

    //仮の上ベクトルを算出
    Up = DirectX::XMVectorSet(0.001f, 1, 0, 0);

    //右ベクトルを算出
    Right = DirectX::XMVector3Cross(Up, Front);
    Right = DirectX::XMVector3Normalize(Right);

    //上ベクトルを算出
    Up = DirectX::XMVector3Cross(Front, Right);
    Up = DirectX::XMVector3Normalize(Up);

    //計算結果を取り出し
    DirectX::XMFLOAT3 right, up, front;
    DirectX::XMStoreFloat3(&right, Right);
    DirectX::XMStoreFloat3(&up, Up);
    DirectX::XMStoreFloat3(&front, Front);

    //スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(circleScale.x, circleScale.y, circleScale.z);

    //回転行列を作成
    //DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x,angle.y,angle.z);
    DirectX::XMMATRIX Rx = DirectX::XMMatrixRotationX(0);
    DirectX::XMMATRIX Ry = DirectX::XMMatrixRotationY(0);
    DirectX::XMMATRIX Rz = DirectX::XMMatrixRotationZ(0);
    DirectX::XMMATRIX R = Rz * Ry * Rx;
    //DirectX::XMMATRIX R = Rx;

    //位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(circlePosition.x, circlePosition.y, circlePosition.z);

    //3つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;

    //計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&circleTransform, W);

    //発射方向
    this->circleDirection = front;
}