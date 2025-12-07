#include "ActionDerived.h"
#include "Game/EnemySlime.h"
#include "Game/Player.h"
#include "System/Mathf.h"
#include "ProjectileManager.h"
#include "Game/PlayerManager.h"
#include "Stage.h"
#include "SearchAlgorithm.h"
#include "EnemyManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneResult.h"
#include"SoundManager.h"
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
	SoundManager::Instance().GetSound(SoundList::heartSE)->Play(false,1.0f);
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
		path_idx = 0;

		// A* を実行して path を取得
		std::vector<int> result;
		//if (SearchAlgorithm::Instance().DijkstraSearch(StageManager::Instance().GetStage(),true))
		if (ok)
		{
			//パス取得
			result = SearchAlgorithm::Instance().BuildPath(StageManager::Instance().GetStage(), startIndex, goalIndex);

			// 経路保存
			//std::vector<int>currPaths = SearchAlgorithm::Instance().GetCurrentPath();
			auto& currPaths = SearchAlgorithm::Instance().GetCurrentPath();
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
		// ?? これを必ず最初に入れる
		if (path.empty()) {
			printf("PATH EMPTY!!\n");
			step = 0;
			return ActionBase::State::Failed;
		}

		if (path_idx < 0 || path_idx >= path.size()) {
			printf("path_idx OUT OF RANGE!! idx=%d size=%d\n",
				path_idx, (int)path.size());

			// とりあえず 0 に戻す
			path_idx = 0;
		}
		float dix = owner->GetPosition().x - PlayerManager::Instance().GetPlayer()->GetPosition().x;
		float diy = owner->GetPosition().y - PlayerManager::Instance().GetPlayer()->GetPosition().y;
		float diz = owner->GetPosition().z - PlayerManager::Instance().GetPlayer()->GetPosition().z;

		float dist = dix * dix + diy * diy + diz * diz;
		float distSQ = sqrtf(dist);

		if (distSQ < 2.5f)
		{
			SceneManager::Instance().ChangeScene(new SceneResult(ResultType::GameOver));
		}

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

		


		if (distSq < 0.001f)
		{
			//パスの最後のウェイポイントに到達したか
			if (path_idx == path.size() - 1)
			{
				//
				step = 0;
				SoundManager::Instance().GetSound(SoundList::heartSE)->Stop();
				//SceneManager::Instance().ChangeScene(new SceneResult(ResultType::GameOver));

			}
			else
				path_idx++;
		}

		owner->MoveToTarget(1.5f, elapsedTime);
		//return ActionBase::State::Complete;
		break;
	}
	}
	return ActionBase::State::Run;
}


//物を壊す
ActionBase::State BringAction::Run(float elapsedTime)
{
	auto& path = SearchAlgorithm::Instance().GetCurrentPath();

	for (int i = 0; i < ProjectileManager::Instance().GetProjectileCount(); i++)
	{
		switch (step)
		{
		case 0:
		{
			printf("ComputePathAction\n");
			// 経路配列をリセット
			/*owner->GetCurrentPath().clear();
			owner->GetPathIndex() = 0;*/

#if 1 // ゲーム用
			// 必要な情報を取得
			int startIndex = StageManager::Instance().GetStage()->NearWayPointIndex(owner->GetPosition());
			//int goalIndex = StageManager::Instance().GetStage()->NearWayPointIndex(owner->GetTargetPosition());
			int goalIndex = StageManager::Instance().GetStage()->NearWayPointIndex(ProjectileManager::Instance().GetProjectile(i)->GetPosition());



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

			path_idx = 0;

			// A* を実行して path を取得
			std::vector<int> result;
			//if (SearchAlgorithm::Instance().DijkstraSearch(StageManager::Instance().GetStage(),true))
			if (ok)
			{
				//パス取得
				result = SearchAlgorithm::Instance().BuildPath(StageManager::Instance().GetStage(), startIndex, goalIndex);

				// 経路保存
				//std::vector<int>currPaths = SearchAlgorithm::Instance().GetCurrentPath();
				auto& currPaths = SearchAlgorithm::Instance().GetCurrentPath();
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

		break;



		case 1:
		{
			{
				float dix = owner->GetPosition().x - PlayerManager::Instance().GetPlayer()->GetPosition().x;
				float diy = owner->GetPosition().y - PlayerManager::Instance().GetPlayer()->GetPosition().y;
				float diz = owner->GetPosition().z - PlayerManager::Instance().GetPlayer()->GetPosition().z;

				float dist = dix * dix + diy * diy + diz * diz;

				if (dist < 10.0f)
				{
					SceneManager::Instance().ChangeScene(new SceneResult(ResultType::GameOver));
				}
			}

			{
				float dix = owner->GetPosition().x - ProjectileManager::Instance().GetProjectile(i)->GetPosition().x;
				float diy = owner->GetPosition().y - ProjectileManager::Instance().GetProjectile(i)->GetPosition().y;
				float diz = owner->GetPosition().z - ProjectileManager::Instance().GetProjectile(i)->GetPosition().z;

				float dist = dix * dix + diy * diy + diz * diz;

				if (dist < 10.0f)
				{
					ProjectileManager::Instance().GetProjectile(i)->Destroy();
				}
			}


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
					return ActionBase::State::Complete;
				}
				else
					path_idx++;
			}

			owner->MoveToTarget(0.5f, elapsedTime);
			//return ActionBase::State::Complete;
			break;
		}

		}
	}
	return ActionBase::State::Run;
}

// AStarを実行して経路を作る 徘徊
ActionBase::State ComputePathAction::Run(float elapsedTime)
{

	

	// プレイヤーとの高低差を考慮して3Dで距離判定をする
	//const DirectX::XMFLOAT3& playerPosition = PlayerManager::Instance().GetPlayer()->GetPosition();
	//float vx = playerPosition.x - owner->GetPosition().x;
	////float vy = playerPosition.y - position.y;
	//float vz = playerPosition.z - owner->GetPosition().z;
	//float dist = sqrtf(vx * vx + vz * vz);
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
		//int startIndex = StageManager::Instance().GetStage()->NearWayPointIndex(owner->GetPosition());
		////int goalIndex = StageManager::Instance().GetStage()->NearWayPointIndex(owner->GetTargetPosition());
		//int goalIndex = rand() % StageManager::Instance().GetStage()->GetWayPointCount();

		//if (startIndex == goalIndex)
		//{
		//	goalIndex = rand() % StageManager::Instance().GetStage()->GetWayPointCount();
		//}

	/*	int count = StageManager::Instance().GetStage()->GetWayPointCount();
		int startIndex = StageManager::Instance().GetStage()->NearWayPointIndex(owner->GetPosition());

		int goalIndex = startIndex;
		while (goalIndex == startIndex)
		{
			goalIndex = rand() % count;
		}*/

		int startIndex = StageManager::Instance().GetStage()->NearWayPointIndex(owner->GetPosition());

		int goalIndex;
		int wpCount = StageManager::Instance().GetStage()->GetWayPointCount();

		do {
			goalIndex = rand() % wpCount;
		} while (goalIndex == startIndex);
		

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

		path_idx = 0;

		// A* を実行して path を取得
		std::vector<int> result;
		//if (SearchAlgorithm::Instance().DijkstraSearch(StageManager::Instance().GetStage(),true
	
		
		if(ok)
		{
			//パス取得
			result = SearchAlgorithm::Instance().BuildPath(StageManager::Instance().GetStage(),startIndex,goalIndex);

			// 経路保存
			//std::vector<int>currPaths = SearchAlgorithm::Instance().GetCurrentPath();
			auto& currPaths = SearchAlgorithm::Instance().GetCurrentPath();
			currPaths = result;

			auto& path = SearchAlgorithm::Instance().GetCurrentPath();


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

		// ?? これを必ず最初に入れる
		if (path.empty()) {
			printf("PATH EMPTY!!\n");
			step = 0;
			return ActionBase::State::Failed;
		}

		if (path_idx < 0 || path_idx >= path.size()) {
			printf("path_idx OUT OF RANGE!! idx=%d size=%d\n",
				path_idx, (int)path.size());

			// とりあえず 0 に戻す
			path_idx = 0;
		}
		float dix = owner->GetPosition().x - PlayerManager::Instance().GetPlayer()->GetPosition().x;
		float diy = owner->GetPosition().y - PlayerManager::Instance().GetPlayer()->GetPosition().y;
		float diz = owner->GetPosition().z - PlayerManager::Instance().GetPlayer()->GetPosition().z;

		float dist = dix * dix + diy * diy + diz * diz;
		float distSQ = sqrtf(dist);

		if (distSQ < 2.5f)
		{
			SceneManager::Instance().ChangeScene(new SceneResult(ResultType::GameOver));
		}
	/*	if (dist < 0.1f)
		{
			SceneManager::Instance().ChangeScene(new SceneResult(ResultType::GameOver));
		}*/

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

		//if (dist < owner->GetSearchRange())
		//{
		//	float distXZ = sqrtf(vx * vx + vz * vz);
		//	// 単位ベクトル化
		//	vx /= distXZ;
		//	vz /= distXZ;

		//	// 方向ベクトル化
		//	float frontX = sinf(owner->GetAngle().y);
		//	float frontZ = cosf(owner->GetAngle().y);
		//	// 2つのベクトルの内積値で前後判定
		//	float dot = (frontX * vx) + (frontZ * vz);
		//	if (dot > 0.0f)
		//	{
		//		owner->SetSPlayer(true);
		//		return ActionBase::State::Complete;
		//	}
		//}

		

		if (owner->SearchPlayer())
		{
			return ActionBase::State::Complete;
		}

		if (owner->GetSPlayer() == false)
		{
			if (distSq < 0.01f)
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



