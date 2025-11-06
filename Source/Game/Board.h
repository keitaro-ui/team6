#pragma once
#include "System/Model.h"
#include "Enemy.h"
#include "Player.h"

class Board : public Enemy
{
public:
	Board();
	~Board() override;

	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	//距離によるイベントを検出する
	bool CheckPlayerOnBoard(const Player* player);

	//クイズ開始処理
	void StartQuiz();

protected:
	//死亡したときに呼ばれる
	//void OnDead() override;

//変数
public:
	bool quizStarted = false;
	bool playerNear = false;

private:
	Model* model_boad;
	bool quizFlag;
};