#include "Board.h"

#include <random>
#include <ctime>
#include <cstdlib>

Board::Board()
{
	//デフォルト（指定なしの場合）
	model = std::make_unique<Model>("Data/Model/Boad/boad_1.mdl");
	position = { 0, 0, 0 };
	angle = { 0, DirectX::XM_PI, 0 };
	scale = { 0.2f, 0.2f, 0.2f };

	//boardPos[0] = { 0.0f, 0.0f, 15.0f };

	radius = 0.2f;
	height = 0.0f;
	quizFlag = false;
}

//デストラクタ
Board::~Board()
{
	quizFlag = false;
}

//modelのセッター
void Board::SetModel(const char* modelPath)
{
	model = std::make_unique<Model>(modelPath);
}

//更新処理
void Board::Update(float elapsedTime)
{
	//速力更新処理
	//UpdateVelocity(elapsedTime);

	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	//model->UpdateTransform();

	//無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	//if ()
	{

	}
}

//描画処理
void Board::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	//renderer->Render(rc, transform, model_boad, ShaderId::Lambert);
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);
}

//playerと対面でクイズ開始させる
bool Board::CheckPlayerOnBoard(const Player* player)
{
	// 現在の距離を計算
	DirectX::XMVECTOR boardPos_vec = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR playerPos_vec = DirectX::XMLoadFloat3(&player->GetPosition());
	DirectX::XMVECTOR distVec = DirectX::XMVectorSubtract(boardPos_vec, playerPos_vec);
	DirectX::XMFLOAT3 distance;
	DirectX::XMStoreFloat3(&distance, distVec);

	// プレイヤーとボードの水平距離（XZ平面）
	float distXZ = sqrtf(distance.x * distance.x + distance.z * distance.z);

	// 極端に近い距離（例：30cm以内）を接近判定に使う
	const float NEAR_DISTANCE = 0.3f;

	bool isNear = (distXZ <= NEAR_DISTANCE);

	// 「前フレームは離れていて、今フレームで近づいた」→ その瞬間だけ true を返す
	bool justEntered = (!playerNear && isNear);

	// 状態を次フレームに持ち越す
	playerNear = isNear;

	// クイズ開始判定
	if (justEntered) {
		quizStarted = true;
		return true;
	}

	return false;
}

//クイズの中身
void Board::StartQuiz()
{
	// モデルに書かれたクイズ内容を参照する想定
   // ここではデバッグ出力のみ
	OutputDebugStringA("Board::StartQuiz() called\n");
}
