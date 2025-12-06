#include "ActionDerived.h"
#include "Game/EnemySlime.h"
#include "Game/Player.h"
#include "System/Mathf.h"
#include "ProjectileManager.h"
#include "Game/PlayerManager.h"
#include "Stage.h"
#include "SearchAlgorithm.h"
#include "EnemyManager.h"
#include <random>

// 徘徊行動
#if 0
ActionBase::State WanderAction::Run(float elapsedTime)
{
	//EnemyBlueSlime05* enemy=nullptr;
	switch (step)
	{
	case 0:
		// 徘徊モーション設定
		//owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime05::EnemyAnimation::WalkFWD), true);
		step++;
		break;
	case 1:
		// 目的地点までのXZ平面での距離判定
		DirectX::XMFLOAT3 position = owner->GetPosition();
		DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
		float vx = targetPosition.x - position.x;
		float vz = targetPosition.z - position.z;
		float distSq = vx * vx + vz * vz;

		// 目的地へ着いた
		float radius = owner->GetRadius();
		if (distSq < radius * radius)
		{
			//step = 0;
			owner->SetRandomTargetPosition(); // 次の目的地を設定

			//return ActionBase::State::Complete;
			// 徘徊成功を返す
			//break; // 続行
		}

		// 目的地点へ移動
		owner->MoveToTarget(elapsedTime, 0.5f);

		owner->SetTerritory(PlayerManager::Instance().GetPlayer()->GetPosition(), 10.0f);

		// プレイヤー索敵成功したら
		if (owner->SearchPlayer())
		{
			step = 0;
			// 徘徊成功を返す
			return ActionBase::State::Complete;
		}

		//step = 0;
		//return ActionBase::State::Complete;
		break;
	}
	// 実行中を返す
	return ActionBase::State::Run;
}
#endif

// 追跡行動
ActionBase::State PursuitAction::Run(float elapsedTime)
{
	auto& path = SearchAlgorithm::Instance().GetCurrentPath();

	switch (step)
	{
	case 0:
	{
		printf("ComputePathAction\n");
		// 経路配列をリセット
		/*owner->GetCurrentPath().clear();
		owner->GetPathIndex() = 0;*/


		int i = 0;


#if 1 // ゲーム用
		// 必要な情報を取得
		int startIndex = StageManager::Instance().GetStage()->NearWayPointIndex(owner->GetPosition());
		//int goalIndex = StageManager::Instance().GetStage()->NearWayPointIndex(owner->GetTargetPosition());
		int goalIndex = StageManager::Instance().GetStage()->NearWayPointIndex(PlayerManager::Instance().GetPlayer()->GetPosition());

		

#else
		int startIndex = StageManager::Instance().GetStage()->NearWayPointIndex(owner->GetPosition());
		int goalIndex = rand() % StageManager::Instance().GetStage()->GetWayPointCount();

# endif

		/*WayPoint* start = StageManager::Instance().GetStage()->GetWayPoint(startIndex);
		WayPoint* goal = StageManager::Instance().GetStage()->GetWayPoint(goalIndex);
		if(start->area==goal->area)
		*/

		bool ok = SearchAlgorithm::Instance().DijkstraSearch(
			StageManager::Instance().GetStage(),
			startIndex,
			goalIndex,
			true
		);

		// A* を実行して path を取得
		std::vector<int> result;
		//if (SearchAlgorithm::Instance().DijkstraSearch(StageManager::Instance().GetStage(),true))
		if (ok)
		{
			//パス取得
			result = SearchAlgorithm::Instance().BuildPath(StageManager::Instance().GetStage(), startIndex, goalIndex);

			// 経路保存
			std::vector<int>currPaths = SearchAlgorithm::Instance().GetCurrentPath();
			currPaths = result;


			DirectX::XMFLOAT3 way_pos = StageManager::Instance().GetStage()->GetWayPoint(path.at(path_idx))->position;
			//DirectX::XMFLOAT3 way_pos = StageManager::Instance().GetStage()->GetWayPoint(path.at(goalIndex))->position;
			float dx = owner->GetPosition().x - way_pos.x;
			float dy = owner->GetPosition().y - way_pos.y;
			float dz = owner->GetPosition().z - way_pos.z;

			float distSq = dx * dx + dy * dy + dz * dz;

			path_idx = distSq < 0.1f ? 1 : 0;



			// 次のステップへ
			step = 1;
			return ActionBase::State::Complete;
		}
		else
		{
			return ActionBase::State::Failed;
		}
		// 次のステップへ
		step = 1;
		break;
	}

	case 1:
	{
		DirectX::XMFLOAT3 way_pos = StageManager::Instance().GetStage()->GetWayPoint(path.at(path_idx))->position;
		owner->SetTargetPosition(way_pos);

		DirectX::XMFLOAT3 target_pos = owner->GetTargetPosition();
		//DirectX::XMFLOAT3 goal_pos = StageManager::Instance().GetStage()->GetWayPoint(path.at(goalIndexX))->position;
		float dx = owner->GetPosition().x - target_pos.x;
		float dy = owner->GetPosition().y - target_pos.y;
		float dz = owner->GetPosition().z - target_pos.z;

		//float dx = owner->GetPosition().x - goal_pos.x;
		//float dy = owner->GetPosition().y - goal_pos.y;
		//float dz = owner->GetPosition().z - goal_pos.z;

		float distSq = dx * dx + dy * dy + dz * dz;

		if (distSq < 0.1f)
		{
			//パスの最後のウェイポイントに到達したか
			if (path_idx == path.size() - 1)
			{
				//
				step = 0;

			}
			else
				path_idx++;
		}

		owner->MoveToTarget(0.5f, elapsedTime);
		//return ActionBase::State::Complete;
		break;
	}
	}
	return ActionBase::State::Run;
}

// 待機行動
ActionBase::State IdleAction::Run(float elapsedTime)
{
	float runTimer = owner->GetRunTimer();
	switch (step)
	{
	case 0:
		owner->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));
		//owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime05::EnemyAnimation::IdleNormal), true);
		step++;
		break;
	case 1:
		runTimer -= elapsedTime;
		// タイマー更新
		owner->SetRunTimer(runTimer);

		// 待機時間が過ぎた時
		if (runTimer <= 0.0f)
		{
			owner->SetRandomTargetPosition();
			step = 0;
			return ActionBase::State::Complete;
		}

		// プレイヤーを見つけた時
		if (owner->SearchPlayer())
		{
			step = 0;
			return ActionBase::State::Complete;
		}
		break;
	}
	// 実行中を返す
	return ActionBase::State::Run;
}

// 逃走行動
ActionBase::State LeaveAction05::Run(float elapsedTime)
{
	DirectX::XMFLOAT3 targetPosition;
	switch (step)
	{
	case 0:
		// 目標地点をプレイヤーと正反対のベクトル×5の位置に指定
		DirectX::XMVECTOR startPosition = DirectX::XMLoadFloat3(&PlayerManager::Instance().GetPlayer()->GetPosition());
		DirectX::XMVECTOR endPosition = DirectX::XMLoadFloat3(&owner->GetPosition());

		DirectX::XMVECTOR TargetPosition = DirectX::XMVectorSubtract(endPosition, startPosition);
		TargetPosition = DirectX::XMVector3Normalize(TargetPosition);
		TargetPosition = DirectX::XMVectorScale(TargetPosition, 5.0f);


		DirectX::XMStoreFloat3(&targetPosition, TargetPosition);
		targetPosition.x += owner->GetPosition().x;
		targetPosition.y += owner->GetPosition().y;
		targetPosition.z += owner->GetPosition().z;
		owner->SetTargetPosition(targetPosition);

		//owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime05::EnemyAnimation::RunFWD), true);
		step++;
		break;
	case 1:

		targetPosition = owner->GetTargetPosition();
		// 目的地点へ移動
		owner->MoveToward(elapsedTime, 1.0);

		DirectX::XMFLOAT3 position = owner->GetPosition();
		targetPosition = owner->GetTargetPosition();

		float vx = targetPosition.x - position.x;
		float vz = targetPosition.z - position.z;
		float distSq = vx * vx + vz * vz;

		// 目的地へ着いた
		float radius = owner->GetRadius();
		if (distSq < radius * radius)
		{
			step = 0;
			return ActionBase::State::Complete;
		}

		break;
	}
	// 実行中を返す
	return ActionBase::State::Run;
}
// 回復行動
ActionBase::State RecoverAction::Run(float elapsedTime)
{
	switch (step)
	{
	case 0:
		//owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime05::EnemyAnimation::Taunt), false);
		step++;
		break;
	case 1:
		/*if (!owner->GetModel()->IsPlayAnimation())
		{
			owner->SetHealth(owner->GetMaxHealth());
			step = 0;
			return ActionBase::State::Complete;
		}*/
		break;
	}
	// 実行中を返す
	return ActionBase::State::Run;
}

//物を壊す
ActionBase::State BringAction::Run(float elapsedTime)
{
	//float moveTimer = owner->GetMoveTimer();

	switch (step)
	{
	case 0:
	{
		// アニメーション再生（押す動作）
		//owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime05::EnemyAnimation::Attack02), true);

		step++;
		break;
	}

	case 1:
	{
		owner->SetRenderModel(false);
		//delete EnemyBlueSlime05().
		break;
	}
	// 実行中を返す
	return ActionBase::State::Run;
	}
}

// 箱見つけたら動き出す
ActionBase::State BreakPursuitAction::Run(float elapsedTime)
{
	float runTimer = owner->GetRunTimer();
	/*for (int i = 0; i < safetyAreas.size(); ++i)
	{
		SafetyArea* area = safetyAreas[i];
	}*/
	for (int i = 0; i < 4; i++)
	{
		switch (step)
		{
		case 0:
			// 目標地点を箱の位置に設定
			owner->SetTargetPosition(ProjectileManager::Instance().GetProjectile(i)->GetPosition());
			owner->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));
			//owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime05::EnemyAnimation::RunFWD), true);
			step++;
			break;
		case 1:
			runTimer -= elapsedTime;
			// タイマー更新
			owner->SetRunTimer(runTimer);
			// 目標地点を箱の位置に設定
			owner->SetTargetPosition(ProjectileManager::Instance().GetProjectile(i)->GetPosition());
			// 目的地点へ移動
			//owner->MoveToTarget(elapsedTime, 1.0);

			// プレイヤーとの距離を計算
			DirectX::XMFLOAT3 position = owner->GetPosition();
			DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();

			float vx = targetPosition.x - position.x;
			float vy = targetPosition.y - position.y;
			float vz = targetPosition.z - position.z;
			float dist = sqrtf(vx * vx + vy * vy + vz * vz);
			// 攻撃範囲にいるとき
			if (dist < owner->GetAttackRange())
			{
				step = 0;
				// 追跡成功を返す
				return ActionBase::State::Complete;
			}
			// 行動時間が過ぎた時
			if (runTimer <= 0.0f)
			{
				step = 0;
				// 追跡失敗を返す
				return ActionBase::State::Failed;
			}
			break;
		}
	}
	// 実行中を返す
	return ActionBase::State::Run;
}


// AStarを実行して経路を作る
ActionBase::State ComputePathAction::Run(float elapsedTime)
{

	// プレイヤーとの高低差を考慮して3Dで距離判定をする
	const DirectX::XMFLOAT3& playerPosition = PlayerManager::Instance().GetPlayer()->GetPosition();
	float vx = playerPosition.x - owner->GetPosition().x;
	//float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - owner->GetPosition().z;
	float dist = sqrtf(vx * vx + vz * vz);
	//int goalIndexX = rand() % 40;
	auto& path = SearchAlgorithm::Instance().GetCurrentPath();

	switch (step)
	{
	case 0:
	{
		printf("ComputePathAction\n");
		// 経路配列をリセット
		/*owner->GetCurrentPath().clear();
		owner->GetPathIndex() = 0;*/
		
	
		int i = 0;


#if 1 // ゲーム用
		// 必要な情報を取得
		int startIndex = StageManager::Instance().GetStage()->NearWayPointIndex(owner->GetPosition());
		//int goalIndex = StageManager::Instance().GetStage()->NearWayPointIndex(owner->GetTargetPosition());
		int goalIndex = rand() % StageManager::Instance().GetStage()->GetWayPointCount();

		while (startIndex == goalIndex) {
			goalIndex = rand() % StageManager::Instance().GetStage()->GetWayPointCount();
		}
		
#else
		int startIndex = StageManager::Instance().GetStage()->NearWayPointIndex(owner->GetPosition());
		int goalIndex = rand() % StageManager::Instance().GetStage()->GetWayPointCount();

# endif
		
		/*WayPoint* start = StageManager::Instance().GetStage()->GetWayPoint(startIndex);
		WayPoint* goal = StageManager::Instance().GetStage()->GetWayPoint(goalIndex);
		if(start->area==goal->area)
		*/	

		bool ok = SearchAlgorithm::Instance().DijkstraSearch(
			StageManager::Instance().GetStage(),
			startIndex,
			goalIndex,
			true
		);

		// A* を実行して path を取得
		std::vector<int> result;
		//if (SearchAlgorithm::Instance().DijkstraSearch(StageManager::Instance().GetStage(),true
	
		
		if(ok)
		{
			//パス取得
			result = SearchAlgorithm::Instance().BuildPath(StageManager::Instance().GetStage(),startIndex,goalIndex);

			// 経路保存
			std::vector<int>currPaths = SearchAlgorithm::Instance().GetCurrentPath();
			currPaths = result;


			DirectX::XMFLOAT3 way_pos = StageManager::Instance().GetStage()->GetWayPoint(path.at(path_idx))->position;
			//DirectX::XMFLOAT3 way_pos = StageManager::Instance().GetStage()->GetWayPoint(path.at(goalIndex))->position;
			float dx = owner->GetPosition().x - way_pos.x;
			float dy = owner->GetPosition().y - way_pos.y;
			float dz = owner->GetPosition().z - way_pos.z;

			float distSq = dx * dx + dy * dy + dz * dz;

			path_idx = distSq < 0.1f ? 1 : 0;
			

			
			// 次のステップへ
			step = 1;
			return ActionBase::State::Complete;
		}
		else
		{
			return ActionBase::State::Failed;
		}
		// 次のステップへ
		step = 1;
		break;
	}

	case 1:
	{
		DirectX::XMFLOAT3 way_pos = StageManager::Instance().GetStage()->GetWayPoint(path.at(path_idx))->position;
		owner->SetTargetPosition(way_pos);

		DirectX::XMFLOAT3 target_pos = owner->GetTargetPosition();
		//DirectX::XMFLOAT3 goal_pos = StageManager::Instance().GetStage()->GetWayPoint(path.at(goalIndexX))->position;
		float dx = owner->GetPosition().x - target_pos.x;
		float dy = owner->GetPosition().y - target_pos.y;
		float dz = owner->GetPosition().z - target_pos.z;

		//float dx = owner->GetPosition().x - goal_pos.x;
		//float dy = owner->GetPosition().y - goal_pos.y;
		//float dz = owner->GetPosition().z - goal_pos.z;

		float distSq = dx * dx + dy * dy + dz * dz;

		if (dist < owner->GetSearchRange())
		{
			float distXZ = sqrtf(vx * vx + vz * vz);
			// 単位ベクトル化
			vx /= distXZ;
			vz /= distXZ;

			// 方向ベクトル化
			float frontX = sinf(owner->GetAngle().y);
			float frontZ = cosf(owner->GetAngle().y);
			// 2つのベクトルの内積値で前後判定
			float dot = (frontX * vx) + (frontZ * vz);
			if (dot > 0.0f)
			{
				owner->SetSPlayer(true);
				return ActionBase::State::Complete;
			}
		}

		if (owner->GetSPlayer() == false)
		{
			if (distSq < 0.1f)
			{
				//パスの最後のウェイポイントに到達したか
				if (path_idx == path.size() - 1)
				{
					//
					step = 0;
					return ActionBase::State::Complete;

				}
				else
					path_idx++;
			}
		}

		owner->MoveToTarget(0.5f, elapsedTime);
		//return ActionBase::State::Complete;
		break;
	}
	}
	return ActionBase::State::Run;
}

////経路に沿って移動
//ActionBase::State FollowPathAction::Run(float elapsedTime)
//{
//	switch (step)
//	{
//	case 0:
//	{
//		// 経路が空なら失敗
//		if (SearchAlgorithm::Instance().GetCurrentPath().empty())
//			return ActionBase::State::Failed;
//
//
//		// 次のステップへ
//		step = 1;
//		break;
//	}
//
//	case 1:
//	{
//		
//
//		DirectX::XMFLOAT3 target = owner->GetTargetPosition();
//
//		owner->MoveToTarget(0.5f, elapsedTime);
//
//		float dx = owner->GetPosition().x - target.x;
//		float dy = owner->GetPosition().y - target.y;
//		float dz = owner->GetPosition().z - target.z;
//
//		float distSq = dx * dx + dy * dy + dz * dz;
//
//		if (distSq < 0.1f)  // ある程度近づいたら到達扱い
//		{
//			return ActionBase::State::Complete;
//		}
//
//
//
//		/*if (owner->GetPosition().x == target.x && owner->GetPosition().y == target.y && owner->GetPosition().z == target.z)
//		{
//			return ActionBase::State::Complete;
//		}*/
//
//		
//
//		// 到着判定
//		/*if (owner->Reached(target))
//		{
//			owner->pathIndex++;
//		}*/
//
//		break;
//	}
//	}
//
//	return ActionBase::State::Run;
//}
//

//経路に沿って移動
ActionBase::State FollowPathAction::Run(float elapsedTime)
{
	switch (step)
	{
	case 0:
	{
		// 経路が空なら失敗
		if (SearchAlgorithm::Instance().GetCurrentPath().empty())
			return ActionBase::State::Failed;


		auto& path = SearchAlgorithm::Instance().GetCurrentPath();
 

		//path.at(0) = currPaths.at(0);

		// 次のステップへ
		step = 1;
		break;
	}
	case 1:
	{

		
		//owner->GetTargetPosition() = StageManager::Instance().GetStage()->GetWayPoint(path_idx).position;
		owner->GetTargetPosition() = StageManager::Instance().GetStage()->GetWayPoint(path_idx)->position;

		float dx = owner->GetPosition().x - owner->GetTargetPosition().x;
		float dy = owner->GetPosition().y - owner->GetTargetPosition().y;
		float dz = owner->GetPosition().z - owner->GetTargetPosition().z;

		float distSq = dx * dx + dy * dy + dz * dz;

		if (distSq < 0.1f)
			path_idx++;

		owner->MoveToTarget(0.5f, elapsedTime);


		//float distSq = dx * dx + dy * dy + dz * dz;

		//if (distSq < 0.1f)  // ある程度近づいたら到達扱い
		//{
		//	return ActionBase::State::Complete;
		//}



		/*if (owner->GetPosition().x == target.x && owner->GetPosition().y == target.y && owner->GetPosition().z == target.z)
		{
			return ActionBase::State::Complete;
		}*/



		// 到着判定
		/*if (owner->Reached(target))
		{
			owner->pathIndex++;
		}*/

		break;
	}
	}

	return ActionBase::State::Run;
}


