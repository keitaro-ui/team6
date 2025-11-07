#pragma once

#include "../Game/Stage.h"
#include "../Game/Player.h"
#include "CameraController.h"
#include "DirectXMath.h"
#include "System/Sprite.h"
#include "Scene.h"
#include "../Game/Balloon.h"
#include "../Game/Board.h"
#include "../Game/Box.h"

// ゲームシーン
class SceneGame : public Scene
{
public:
	SceneGame() {};
	~SceneGame() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

	// GUI描画
	void DrawGUI() override;

private:
	//ポインタ宣言
	std::unique_ptr<Stage> stage = nullptr;
	
	std::unique_ptr<Player> player = nullptr;

	CameraController* cameraController = nullptr;

	Balloon* balloon = nullptr;

	//複数のBoardを管理する配列
	std::vector<Board*> boards;

	//変数宣言
	bool quizFlag = false;

public:
	
};
