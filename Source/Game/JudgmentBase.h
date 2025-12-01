#pragma once

class EnemySlime;

// é¿çsîªíË
class JudgmentBase
{
public:
	JudgmentBase(EnemySlime* enemy) :owner(enemy) {}
	virtual bool Judgment() = 0;
protected:
	EnemySlime* owner;
};