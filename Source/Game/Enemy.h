#pragma once

#include "System/ModelRenderer.h"
#include "../Game/Character.h"
#include <vector>

//�G�l�~�[
class Enemy : public Character
{
public:
	Enemy();
	~Enemy();

	//�j��
	void Destroy();

	//�X�V����
	virtual void Update(float elapsedTime) = 0;
	
	//�`�揈��
	virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

	int model_index = -1;
	/*int BoardModel_index = -1;*/

	// ���S����
	virtual bool IsDead() const { return health <= 0; }

private:
	Model* model = nullptr;

};
