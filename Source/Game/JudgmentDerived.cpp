#include "Game/JudgmentDerived.h"
#include "Game/Player.h"
#include "Game/PlayerManager.h"
#include "System/Mathf.h"
#include "Camera.h"


inline DirectX::XMFLOAT3 subtract(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
{
	return { a.x - b.x,a.y - b.y,a.z - b.z };
}


// BattleNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
bool BattleJudgment::Judgment()
{
	// ƒvƒŒƒCƒ„[‚ðŒ©‚Â‚¯‚Ä‚¢‚é‚©
	if (owner->SearchPlayer())
	{
		return true;
	}
	return false;
}

// AttackNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
bool AttackJudgment::Judgment()
{
	// ‘ÎÛ‚Æ‚Ì‹——£‚ðŽZo
	DirectX::XMFLOAT3 position = owner->GetPosition();
	DirectX::XMFLOAT3 targetPosition = Camera::Instance().GetFront();

	float vx = targetPosition.x - position.x;
	float vy = targetPosition.y - position.y;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < owner->GetAttackRange())
	{
		// AttackNode‚Ö‘JˆÚ‚Å‚«‚é
		return true;
	}
	return false;
}


// WanderNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
bool WanderJudgment::Judgment()
{
	// –Ú“I’n“_‚Ü‚Å‚ÌXZ•½–Ê‚Å‚Ì‹——£”»’è
	DirectX::XMFLOAT3 position = owner->GetPosition();
	DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	// –Ú“I’n‚©‚ç—£‚ê‚Ä‚¢‚éê‡
	float radius = owner->GetRadius();
	if (distSq > radius * radius)
	{
		return true;
	}

	return false;
}


bool BringJudgment::Judgment()
{

	DirectX::XMFLOAT3 jud = ProjectileManager::Instance().GetSafety(PlayerManager::Instance().GetPlayer()->GetMaxSafeAreaCount())->GetPosition();
	if (jud.x < 5.0f && jud.y < 5.0f && jud.z < 5.0f)
	{
		return true;
	}
	return false;
}

// BattleNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
bool BreakSearchJudgment::Judgment()
{
	// ” ‚ðŒ©‚Â‚¯‚Ä‚¢‚é‚©
	if (owner->SearchBox())
	{
		return true;
	}
	return false;
}

//Search‚·‚é‚½‚ß‚Éput‚ðtrue‚É‚·‚é
bool PutTrueJudgment::Judgment()
{
	//” ‚ª‚¨‚©‚ê‚Ä‚¢‚é‚©‚Ç‚¤‚©
	if (PlayerManager::Instance().GetPlayer()->GetputTrue() == true)
	{
		return true;
	}
	return false;
}