#pragma once

#include "../Game/Stage.h"
#include "../Game/Player.h"
#include "CameraController.h"
#include "DirectXMath.h"
#include "PhysicsSystem2d.h"
#include "System/Sprite.h"
#include "Scene.h"
#include "../Game/Balloon.h"
#include "../Game/Board.h"
#include "../Game/Box.h"
#include "../Game/EnemySlime.h"

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

	std::unique_ptr<EnemySlime> enemyslime = nullptr;

	CameraController* cameraController = nullptr;

	Balloon* balloon = nullptr;
	

	Board* boards[4];

	std::unique_ptr<Sprite> spr = nullptr;

	//変数宣言
	bool quizFlag = false;
	bool fRenFlag = false;
	PhysicsSystem2d physics;
	Board* activeBoard = nullptr;
	float playerSize;

	float xDis;
	float zDis;
	DirectX::XMFLOAT3 blockSize;
public:
	
};
