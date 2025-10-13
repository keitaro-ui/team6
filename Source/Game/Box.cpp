#include "Box.h"

//�R���X�g���N�^
Box::Box()
{
	model = new Model("Data/Model/Boad/Box.mdl");

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.8f;

	//���A�����ݒ�
	radius = 0.2f;
	height = 0.0f;
	

}

//�f�X�g���N�^
Box::~Box()
{
	delete model;
}

//�X�V����
void Box::Update(float elapsedTime)
{
	//���͍X�V����
	//UpdateVelocity(elapsedTime);

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	//���f���s��X�V
	model->UpdateTransform();

	//���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);
}

//�`�揈��
void Box::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}

void Box::OnDead()
{
	//���M��j��
	/*if (getdes() == true)
	{
		Destroy();
	}*/
}
