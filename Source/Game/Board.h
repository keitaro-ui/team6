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

	//クイズそれぞれを設定するセッターゲッター
	/*void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetAngle(const DirectX::XMFLOAT3& ang);
	void SetScale(const DirectX::XMFLOAT3& scl);*/
	void SetModel(const char* modelPath);

	/*const DirectX::XMFLOAT3& GetPosition() const { return position; }
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	const Model* GetModel() const { return model.get(); }*/

	//クイズ開始処理
	void StartQuiz();


public:
	bool quizStarted = false;
	bool playerNear = false;

private:
	std::unique_ptr<Model> model;
	bool quizFlag;

	/*static DirectX::XMFLOAT3 boardPos[4];
	static DirectX::XMFLOAT3 boardAng[4];*/
};