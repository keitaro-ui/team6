#pragma once
#include "ActionBase.h"
#include "EnemySlime.h"
#include "SafetyArea.h"

//std::vector<SafetyArea*> safetyAreas;

// 通常攻撃
class AttackAction : public ActionBase
{
public:
	AttackAction(EnemySlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// スキル攻撃行動
class SkillAction : public ActionBase
{
public:
	SkillAction(EnemySlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// 追跡行動
class PursuitAction : public ActionBase
{
public:
	PursuitAction(EnemySlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// 徘徊行動
class WanderAction : public ActionBase
{
public:
	WanderAction(EnemySlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// 待機行動
class IdleAction : public ActionBase
{
public:
	IdleAction(EnemySlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// 逃走行動
class LeaveAction05 : public ActionBase
{
public:
	LeaveAction05(EnemySlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// 回復行動
class RecoverAction : public ActionBase
{
public:
	RecoverAction(EnemySlime* enemy) :ActionBase(enemy) {}
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
};

class BreakPursuitAction : public ActionBase
{
public:
	BreakPursuitAction(EnemySlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};
