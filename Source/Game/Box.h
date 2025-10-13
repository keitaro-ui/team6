#pragma once

#include "System/Model.h"
#include "Enemy.h"

//�X���C��
class Box : public Enemy
{
public:
	Box();
	~Box() override;

	//�X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

protected:
	//���S�����Ƃ��ɌĂ΂��
	void OnDead() override;

private:
	Model* model = nullptr;

};
