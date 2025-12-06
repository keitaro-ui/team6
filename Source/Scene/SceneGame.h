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
#include "../Game/Object.h"
#include "../Source/System/Sprite.h"
#include "../Game/StartPoint.h"
#include "../Game/GoalPoint.h"

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

	StartPoint* startPoint = nullptr;
	GoalPoint* goalPoint = nullptr;

	Sprite hpBarTex;
	Sprite hpBarFrameTex;


private:
	//ポインタ宣言
	std::unique_ptr<Stage> stage = nullptr;
	
	std::unique_ptr<Player> player = nullptr;

	std::unique_ptr<EnemySlime> enemyslime = nullptr;


	std::unique_ptr<Start>start = nullptr;

	std::unique_ptr<Goal>goal = nullptr;

	std::unique_ptr<StartPoint> startpoint = nullptr;

	std::unique_ptr<GoalPoint> goalpoint = nullptr;



	CameraController* cameraController = nullptr;

	Balloon* balloon = nullptr;

	ModelRenderer* renderer = nullptr;
	

	Board* boards[4];

	std::unique_ptr<Sprite> spr = nullptr;

	//変数宣言
	PhysicsSystem2d physics;
	Board* activeBoard = nullptr;
	float playerSize;

	float xDis;
	float zDis;
	float debugDoorSize;
	DirectX::XMFLOAT3 blockSize;
	bool playerIsInside = false;
public:
	std::vector<std::unique_ptr<Sprite>> sprites;
};
