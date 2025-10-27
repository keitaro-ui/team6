#pragma once

#include "System/ModelRenderer.h"
#include "../Game/Character.h"
#include <vector>

//エネミー
class Enemy : public Character
{
public:
	Enemy();
	~Enemy();

	//破棄
	void Destroy();

	//更新処理
	virtual void Update(float elapsedTime) = 0;
	
	//描画処理
	virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

	int model_index = -1;
	/*int BoardModel_index = -1;*/

	// 死亡判定
	virtual bool IsDead() const { return health <= 0; }

private:
	Model* model = nullptr;

};
