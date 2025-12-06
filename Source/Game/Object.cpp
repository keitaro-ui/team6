#include <imgui.h>
#include "Object.h"
#include "System\Graphics.h"
#include "Collision.h"
#include "Stage.h"

void ObjectBase::UpdateTransform()
{
	// スケール行列を作成
	DirectX::XMMATRIX s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列を作成
	DirectX::XMMATRIX r = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	// 位置行列を作成
	DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	// 3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX w = s * r * t;
	// 計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, w);
}

static Start* startInstance = nullptr;
// インスタンス取得
Start& Start::Instance()
{
	if (startInstance == nullptr)
	{
		assert(false && "Start instance is null! You must create a Start object before using Start::Instance()");
	}
	return *startInstance;
}

// コンストラクタ
Start::Start()
{
	// インスタンスポイント設定
	startInstance = this;
	position = { StageManager::Instance().GetStage()->GetWayPoint(2)->position.x, StageManager::Instance().GetStage()->GetWayPoint(2)->position.y, StageManager::Instance().GetStage()->GetWayPoint(2)->position.z };
}

// デストラクタ
Start::~Start()
{
}

// プレイヤー更新処理
void Start::Update(float elapsedTime)
{
	// オブジェクト行列を更新
	UpdateTransform();
}

// 描画
void Start::Render(RenderContext* dc, Shader* shader)
{
	// 描画
	ShapeRenderer* ShapeRenderer = Graphics::Instance().GetShapeRenderer();
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&transform, S * R * T);

	//ShapeRenderer->RenderSphere(dc,position, radius, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
}

static Goal* goalInstance = nullptr;

// インスタンス取得
Goal& Goal::Instance()
{
	return *goalInstance;
}


Goal::Goal()
{
	// インスタンスポイント設定
	goalInstance = this;
	position = DirectX::XMFLOAT3(19.5f, 0.0f, 19.5f);
}

Goal::~Goal()
{
}

void Goal::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// 描画
	ShapeRenderer* ShapeRenderer = Graphics::Instance().GetShapeRenderer();
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&transform, S * R * T);
}