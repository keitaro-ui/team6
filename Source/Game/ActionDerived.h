#pragma once
#include "ActionBase.h"
#include "EnemySlime.h"
#include "SafetyArea.h"
#include "System/Raycast.h"




//std::vector<SafetyArea*> safetyAreas;

// 通常攻撃
class AttackAction : public ActionBase
{
public:
	AttackAction(EnemySlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};



// 追跡行動
class PursuitAction : public ActionBase
{
public:
	PursuitAction(EnemySlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);

private:
	int path_idx = 0;
};

// 徘徊行動
class WanderAction : public ActionBase
{
public:
	WanderAction(EnemySlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};


//物を動かす行動
class BringAction : public ActionBase
{
public:
	BringAction(EnemySlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);

private:
	int step = 0;
	int path_idx = -1;
};



class ComputePathAction :public ActionBase
{
public:
	ComputePathAction(EnemySlime*enemy):ActionBase(enemy){}
	ActionBase::State Run(float elpasedTime);

	
	//std::vector<int>GetCP() { return currPaths; }
private:
	//std::vector<int>& currPaths;

	// 現在目標にしているパスのインデックス
	int path_idx = 0;
	int startIndex = 0;
	int goalIndex = 0;
};

