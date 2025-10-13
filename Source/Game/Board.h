#pragma once

#include "System/Model.h"
#include "Enemy.h"

//�X���C��
class Board : public Enemy
{
public:
	Board();
	~Board() override;

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