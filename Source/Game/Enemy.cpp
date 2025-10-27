#include "Enemy.h"
#include "EnemyManager.h"

Enemy::Enemy()
{
	//エネミーモデル読み込み
	model = new Model("Data/Model/Target/balloon.mdl");
}

Enemy::~Enemy()
{
	//エネミーモデル破棄
	delete model;
}

void Enemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	//レンダラモデルに描画してもらう
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}

//破棄
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}
