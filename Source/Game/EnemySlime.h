#pragma once

#include "System/Model.h"
#include "Enemy.h"

//�X���C��
class EnemySlime : public Enemy
{
public:
	EnemySlime();
	~EnemySlime() override;

	//�X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	void CreateModel();

protected:
	//���S�����Ƃ��ɌĂ΂��
	void OnDead() override;

private:

};
