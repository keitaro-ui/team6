#include "Balloon.h"

//�R���X�g���N�^
Balloon::Balloon()
{
	model = new Model("Data/Model/Target/balloon.mdl");

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.2f;

	//���A�����ݒ�
	radius = 0.2f;
	height = 0.0f;
}

//�f�X�g���N�^
Balloon::~Balloon()
{
	delete model;
}

//�X�V����
void Balloon::Update(float elapsedTime)
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
void Balloon::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}

void Balloon::OnDead()
{
	//���M��j��
	Destroy();
	box->Destroy();
}
