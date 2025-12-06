#pragma once

#include "System/Model.h"
#include "Enemy.h"
#include "Player.h"
#include "Game/EnemySlime.h"
#include "Game/BehaviorTree.h"
#include "Game/BehaviorData.h"
#include "Game/NodeBase.h"
#include "Game/JudgmentDerived.h"
#include "Game/ActionDerived.h"
#include "Game/SafetyArea.h"

//スライム
class EnemySlime : public Enemy
{
public:
	EnemySlime();
	~EnemySlime() override;

	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	void DrawGUI();

	//デバッグプリミティブ描画
	//void DrawDebugPrimitive()override;

	//縄張り設定
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

	//ターゲット位置をランダム設定
	void SetRandomTargetPosition();

	//目標地点へ移動
	void MoveToTarget(float speedRate,float elapsedTime);

	void MoveToward(float elapsedTime, float speedRate);

	void WallRayCast(float elapsedTime);

	//プレイヤー索敵
	bool SearchPlayer();

	//ボックス見つける
	bool SearchBox();

	//ターゲットポジション設定
	void SetTargetPosition(DirectX::XMFLOAT3 position) { targetPosition = position; }

	//ターゲットポジション取得
	DirectX::XMFLOAT3 GetTargetPosition() { return targetPosition; }

	//ポジション取得
	DirectX::XMFLOAT3 GetPosition() { return position; }

	//攻撃範囲取得
	float GetAttackRange() { return attackRange; }

	// 実行タイマー設定
	void SetRunTimer(float timer) { runTimer = timer; }

	// 実行タイマー取得
	float GetRunTimer() { return runTimer; }


	void SetRenderModel(bool t) { renderModel = t; }
	float GetRenderModel() const { return renderModel; }

	void CreateModel();

	Model* GetModel() { return model; }



	// ----- currentPath -----
	/*std::vector<int>& GetCurrentPath() {return currentPath;}

	void SetCurrentPath(const std::vector<int>& path){currentPath = path;}*/

	// currentPath をクリアする関数（よく使うので）
	/*void ClearPath()
	{
		currentPath.clear();
		PathIndex = 0;
	}*/

protected:
	//死亡したときに呼ばれる
	void OnDead() override;

public:

	int GetO() { return o; }

private:
	Model* model = nullptr;
	

	DirectX::XMFLOAT3	territoryOrigin = { 0.0f,0.0f,0.0f };
	float				territoryRange = 100.0f;

	DirectX::XMFLOAT3	targetPosition = { 0.0f,0.0f,0.0f };

	float				attackRange = 1.5f;
	float				moveSpeed = 3.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(360);
	float				searchRange = 25.0f;
	float				runTimer = 0.0f;

	bool renderModel = true;

	bool search_player = false;

	BehaviorTree* aiTree = nullptr;
	BehaviorData* behaviorData = nullptr;
	NodeBase* activeNode = nullptr;

	int o = rand() % 42;

public:
	bool GetSPlayer() { return search_player; }
	void SetSPlayer(bool i) { search_player = i; }
	float GetSearchRange() const { return searchRange; }
	DirectX::XMFLOAT3 GetAngle() { return angle; }
};
