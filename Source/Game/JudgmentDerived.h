#pragma once
#include "Game/JudgmentBase.h"
#include "Game/EnemySlime.h"


// BattleNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class BattleJudgment : public JudgmentBase
{
public:
	BattleJudgment(EnemySlime* enemy) :JudgmentBase(enemy) {};
	// ”»’è
	bool Judgment();
};

// AttackNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class AttackJudgment : public JudgmentBase
{
public:
	AttackJudgment(EnemySlime* enemy) :JudgmentBase(enemy) {};
	// ”»’è
	bool Judgment();
};

// WanderNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WanderJudgment : public JudgmentBase
{
public:
	WanderJudgment(EnemySlime* enemy) :JudgmentBase(enemy) {};
	// ”»’è
	bool Judgment();
};


//Bring‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class BringJudgment :public JudgmentBase
{
public:
	BringJudgment(EnemySlime* enemy) :JudgmentBase(enemy) {};
	//”»’è
	bool Judgment();
};

//Break‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class BreakSearchJudgment :public JudgmentBase
{
public:
	BreakSearchJudgment(EnemySlime* enemy) :JudgmentBase(enemy) {};
	//”»’è
	bool Judgment();
};

//Search‚·‚é‚½‚ß‚Éput‚ðtrue‚É‚·‚é
class PutTrueJudgment :public JudgmentBase
{
public:
	PutTrueJudgment(EnemySlime* enemy) :JudgmentBase(enemy) {};
	bool Judgment();
};
