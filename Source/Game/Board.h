#pragma once
#include "System/Model.h"
#include "Enemy.h"
#include "Player.h"
#include <DirectXMath.h>
#include <memory>
#include "imgui.h"

class Board : public Enemy
{
public:
    // indexで初期位置・角度を設定
    Board(int index);
    ~Board() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render(const RenderContext& rc, ModelRenderer* renderer) override;

    // プレイヤーが近づいたか判定
    bool CheckNearBoard(const Player* player);

    // セッター / ゲッター
    void SetPosition(const DirectX::XMFLOAT3& pos) { position = pos; }
    void SetAngle(const DirectX::XMFLOAT3& ang) { angle = ang; }
    void SetScale(const DirectX::XMFLOAT3& scl) { scale = scl; }
    void SetModel(const char* modelPath) { model = std::make_unique<Model>(modelPath); }

    const DirectX::XMFLOAT3& GetPosition() const { return position; }
    const DirectX::XMFLOAT3& GetAngle() const { return angle; }
    const DirectX::XMFLOAT3& GetScale() const { return scale; }
    const Model* GetModel() const { return model.get(); }

    // クイズ
    void StartQuiz();
    void EndQuiz();
    bool IsQuizActive() const { return quizActive; }
    int GetQuizNum() const { return quizNum; }
    void DrawGUIValues();

private:
    std::unique_ptr<Model> model;
    bool quizActive;
    int quizNum;

public:
    bool quizStarted = false;
    bool playerNear = false;
};
