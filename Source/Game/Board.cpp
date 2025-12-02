#include "Board.h"
#include <DirectXMath.h>
#include <cmath>
#include <windows.h> // OutputDebugStringA

// --- Board.cpp 内限定の初期値テーブル ---
namespace 
{
    const DirectX::XMFLOAT3 INIT_POS[4] = 
    {
        { -26.5f, 1.0f, 17.5f },
        { -26.5f, 1.0f, -17.5f },
        { 26.5f, 1.0f, -17.5f },
        { 26.5f, 1.0f, 17.5f },
    };

    const DirectX::XMFLOAT3 INIT_ANG[4] = 
    {
        { 0, DirectX::XM_PI, 0 },
        { 0, DirectX::XM_PI * 2, 0 },
        { 0, DirectX::XM_PI * 2, 0 },
        { 0, DirectX::XM_PI, 0 },
    };
}

// コンストラクタ：indexで初期値を設定
Board::Board(int index)
{
    static const char* paths[] = {
        "Data/Model/Boad/boad_1.mdl",
        "Data/Model/Boad/boad_2.mdl",
        "Data/Model/Boad/boad_3.mdl",
        "Data/Model/Boad/boad_4.mdl"
    };

    model = std::make_unique<Model>(paths[index]);
    position = INIT_POS[index];
    angle = INIT_ANG[index];
    scale = { 0.2f, 0.2f, 0.2f };

    quizNum = index;

    radius = 0.2f;
    height = 0.0f;
    quizActive = false;
}

// デストラクタ
Board::~Board()
{
    quizActive = false;
}

// 更新処理
void Board::Update(float elapsedTime)
{
    // 必要なら速度更新など
    UpdateTransform();
    UpdateInvincibleTimer(elapsedTime);
}

// 描画処理
void Board::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    renderer->Render(rc, transform, model.get(), ShaderId::Lambert);
}

// プレイヤー接近判定
bool Board::CheckNearBoard(const Player* player)
{
    DirectX::XMVECTOR boardPosVec = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&player->GetPosition());
    DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(boardPosVec, playerPosVec);
    DirectX::XMFLOAT3 distance;
    DirectX::XMStoreFloat3(&distance, diff);

    float distXZ = std::sqrt(distance.x * distance.x + distance.z * distance.z);
    const float NEAR_DISTANCE = 1.3f; //接近時の距離の変数

    bool isNear = (distXZ <= NEAR_DISTANCE);
   
    if (isNear)
    {
    	quizStarted = true;
		quizActive = true;
        return true;
    }

    return false;
}

// クイズ開始
void Board::StartQuiz()
{
    switch (quizNum)
    {
    case 0:
        // クイズ0の処理
        break;
    case 1:
        // クイズ1の処理
        break;
    case 2:
        // クイズ2の処理
        break;
    case 3:
        // クイズ3の処理
        break;
    };
}

void Board::EndQuiz()
{
    quizStarted = false;
    quizActive = false;
}

void Board::DrawGUIValues()
{
    // Quiz番号は上に表示
    ImGui::Text("Quiz Num: %d", quizNum);
    ImGui::Separator();

    // Position を編集
    ImGui::DragFloat3("Position", &position.x, 0.1f);

    // Angle を編集（ラジアン）
    ImGui::DragFloat3("Angle", &angle.x, 0.01f);

    // Scale を編集
    ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.01f, 10.0f);

    // クイズ状態
    ImGui::Text("Quiz Active: %s", quizActive ? "true" : "false");

    // デバッグ用ボタン
    if (ImGui::Button("Start Quiz")) StartQuiz();
    ImGui::SameLine();
    if (ImGui::Button("End Quiz")) EndQuiz();
}
