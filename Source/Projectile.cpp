#include "Projectile.h"
#include "ProjectileManager.h"

//コンストラクタ
Projectile::Projectile(ProjectileManager* manager) : manager(manager)
{
	position.y = 1.4f;
	position.z = 0.15f;
	manager->Register(this);
}

//デバッグプリミティブ描画
void Projectile::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//衝突判定用のデバッグ球を描画
	renderer->RenderSphere(rc, position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
}

//行列更新処理
void Projectile::UpdateTransform()
{
	DirectX::XMVECTOR Front, Up, Right;

	//前ベクトルを算出
	Front = DirectX::XMLoadFloat3(&direction);
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
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//回転行列を作成
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x,angle.y,angle.z);
	DirectX::XMMATRIX Rx = DirectX::XMMatrixRotationX(0);
	DirectX::XMMATRIX Ry = DirectX::XMMatrixRotationY(0);
	DirectX::XMMATRIX Rz = DirectX::XMMatrixRotationZ(0);
	DirectX::XMMATRIX R = Rz * Ry * Rx;
	//DirectX::XMMATRIX R = Rx;

	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;

	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);

	//発射方向
	this->direction = front;
}

//破棄
void Projectile::Destroy()
{
	manager->Remove(this);
}
