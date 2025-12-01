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
	void MoveToTarget(float elapsedTime, float speedRate);

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

protected:
	//死亡したときに呼ばれる
	void OnDead() override;

public:


private:
	Model* model = nullptr;
	

	DirectX::XMFLOAT3	territoryOrigin = { 0.0f,0.0f,0.0f };
	float				territoryRange = 100.0f;

	DirectX::XMFLOAT3	targetPosition = { 0.0f,0.0f,0.0f };

	float				attackRange = 1.5f;
	float				moveSpeed = 3.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(360);
	float				searchRange = 5.0f;
	float				runTimer = 0.0f;

	bool renderModel = true;

	BehaviorTree* aiTree = nullptr;
	BehaviorData* behaviorData = nullptr;
	NodeBase* activeNode = nullptr;

	//押す（到達）目標
	/*DirectX::XMFLOAT3 moveTargetPosition = { 10.0f,0.0f,-5.0f };*/
};
