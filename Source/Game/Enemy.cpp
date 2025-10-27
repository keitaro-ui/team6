#include "Enemy.h"
#include "EnemyManager.h"

Enemy::Enemy()
{
	//�G�l�~�[���f���ǂݍ���
	model = new Model("Data/Model/Target/balloon.mdl");
}

Enemy::~Enemy()
{
	//�G�l�~�[���f���j��
	delete model;
}

void Enemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	//�����_�����f���ɕ`�悵�Ă��炤
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}

//�j��
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}
