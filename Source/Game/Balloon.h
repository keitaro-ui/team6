#pragma once

#include "System/Model.h"
#include "Enemy.h"
#include "Box.h"

//�X���C��
class Balloon : public Enemy
{
public:
	Balloon();
	~Balloon() override;

	//�X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	Box* box=nullptr;

protected:
	//���S�����Ƃ��ɌĂ΂��
	void OnDead() override;

private:
	Model* model = nullptr;
	
	

};
