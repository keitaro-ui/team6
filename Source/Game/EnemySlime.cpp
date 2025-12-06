#include "EnemySlime.h"
#include "System\Mathf.h"
#include "PlayerManager.h"

#include <random>
#include <imgui.h>


//コンストラクタ
EnemySlime::EnemySlime()
{
	model = new Model("Data/EnemySlime/BlueSlime.mdl");
	//model = new Model("Data/Model/Target/balloon.mdl");
	//モデルが大きいのでスケーリング
	//scale.x = scale.y = scale.z = 0.1f;
	scale = { 0.02f, 0.02f, 0.02f };
	//幅、高さ設定
	radius = 0.2f;
	height = 0.0f;

	CreateModel();
	SetPosition(DirectX::XMFLOAT3(0.0f, -0.05f, 5.0f));

	behaviorData = new BehaviorData();
	aiTree = new BehaviorTree(this);

	aiTree->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority, nullptr, nullptr);
	{
		// 優先順位設定（小さいほど優先）
		//aiTree->AddNode("Root", "Escape", 1, BehaviorTree05::SelectRule::Sequence, new EscapeJudgment05(this), nullptr);
		aiTree->AddNode("Root", "Battle", 1, BehaviorTree::SelectRule::Priority, new BattleJudgment(this), nullptr);
		// Battle 子ノード
		{
			//aiTree->AddNode("Battle", "Attack", 1, BehaviorTree05::SelectRule::Random, new AttackJudgment05(this), nullptr);
			aiTree->AddNode("Battle", "Pursuit", 1, BehaviorTree::SelectRule::Non, nullptr, new PursuitAction(this));
		}
		aiTree->AddNode("Root", "SearchBox", 2, BehaviorTree::SelectRule::Priority, new PutTrueJudgment(this), nullptr);
		//Bring子ノード
		{
			aiTree->AddNode("SearchBox", "Search", 1, BehaviorTree::SelectRule::Sequence, new BreakSearchJudgment(this), new BreakPursuitAction(this));
			{
				//中間地点を壊す
				aiTree->AddNode("Search", "Push", 1, BehaviorTree::SelectRule::Non, nullptr, new BringAction(this));
			}
		}
		aiTree->AddNode("Root", "Scout", 3, BehaviorTree::SelectRule::Random, nullptr, nullptr);
		// Scout 子ノード
		{
			aiTree->AddNode("Scout", "Wander", 1, BehaviorTree::SelectRule::Non, new WanderJudgment(this), new WanderAction(this));
			//aiTree->AddNode("Scout", "Idle", 2, BehaviorTree05::SelectRule::Non, nullptr, new IdleAction05(this));
		}

	}

	//ここで縄張り設定
	SetTerritory(PlayerManager::Instance().GetPlayer()->GetPosition(), 10.0f);
	SetRandomTargetPosition();
}

//デストラクタ
EnemySlime::~EnemySlime()
{
	delete model;
}

//更新処理
void EnemySlime::Update(float elapsedTime)
{
	// 現在実行されているノードが無ければ
	if (activeNode == nullptr)
	{
		// 次に実行するノードを推論する。
		activeNode = aiTree->ActiveNodeInference(behaviorData);
	}
	// 現在実行するノードがあれば
	if (activeNode != nullptr)
	{
		// ビヘイビアツリーからノードを実行。
		activeNode = aiTree->Run(activeNode, behaviorData, elapsedTime);
	}

	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	//model->UpdateTransform();

	//無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	DrawGUI();
}

//描画処理
void EnemySlime::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);
	//rendererer->RenderSphere(rc, targetPosition, radius, { 0.2f, 1.0f, 0.2f, 1.0f });
}

void EnemySlime::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	renderer->RenderSphere(rc, targetPosition, radius,  { 0.2f, 1.0f, 0.2f, 1.0f });
}

void EnemySlime::DrawGUI()
{
	if (ImGui::Begin("EnemySlime Debug"))
	{
		std::string name = activeNode ? activeNode->GetName() : "None";
		ImGui::Text("Behavior: %s", name.c_str());
	}
	ImGui::End();
}

void EnemySlime::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

// ターゲット位置をランダム設定
void EnemySlime::SetRandomTargetPosition()
{
	float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
	float range = Mathf::RandomRange(0.0f, territoryRange);
	targetPosition.x = territoryOrigin.x + sinf(theta) * range;
	targetPosition.y = territoryOrigin.y;
	targetPosition.z = territoryOrigin.z + cosf(theta) * range;
}

// 目的地点へ移動
void EnemySlime::MoveToTarget(float elapsedTime, float speedRate)
{
	// ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 移動処理
	Move(elapsedTime,vx, vz, moveSpeed * speedRate);
	Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

bool EnemySlime::SearchPlayer()
{
	// プレイヤーとの高低差を考慮して3Dで距離判定をする
	const DirectX::XMFLOAT3& playerPosition = PlayerManager::Instance().GetPlayer()->GetPosition();
	float vx = playerPosition.x - position.x;
	//float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrtf(vx * vx +  vz * vz);

	if (dist < searchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		// 単位ベクトル化
		vx /= distXZ;
		vz /= distXZ;

		// 方向ベクトル化
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);
		// 2つのベクトルの内積値で前後判定
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}
	}
	return false;
}

//敵の中間地点を壊しに行く
bool EnemySlime::SearchBox()
{
	// ものの位置を取得
	/*for (int i = 0; i < PlayerManager::Instance().GetPlayer()->GetLastSafetyAreaIndex(); ++i)
	{*/
	Player* player = PlayerManager::Instance().GetPlayer();

	if (player->GetLastSafetyAreaIndex() < 0 || player->GetSafety().size() < player->GetLastSafetyAreaIndex())
	{
		OutputDebugStringA("Error\n");
	}

		const DirectX::XMFLOAT3& BoxPosition = PlayerManager::Instance().GetPlayer()->GetSafety().at(PlayerManager::Instance().GetPlayer()->GetLastSafetyAreaIndex())->GetPosition();

		// 高低差を考慮して3Dで距離判定をする
		// 敵からものへの距離を計算。
		float vx = BoxPosition.x - position.x;
		//float vy = BoxPosition.y - position.y;
		float vz = BoxPosition.z - position.z;
		float dist = sqrtf(vx * vx + vz * vz);

		if (dist < searchRange)
		{
			// 平面上の距離を計算
			float distXZ = sqrtf(vx * vx + vz * vz);
			// 単位ベクトル化
			vx /= distXZ;
			vz /= distXZ;

			// 方向ベクトル化
			float frontX = sinf(angle.y);
			float frontZ = cosf(angle.y);
			// 2つのベクトルの内積値で前後判定
			float dot = (frontX * vx) + (frontZ * vz);
			if (dot > 0.0f)
			{
				return true;
			}
		}
	
	return false;
}

void EnemySlime::CreateModel()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int>dist(0, 3);

	model_index = dist(gen);
}

void EnemySlime::OnDead()
{
	//自信を破棄
	Destroy();
}
