#include "EnemySlime.h"

#include <random>
//#include <algorithm>

Model* models[4];


//�R���X�g���N�^
EnemySlime::EnemySlime()
{

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.1f;

	//���A�����ݒ�
	radius = 0.2f;
	height = 0.0f;

	CreateModel();
}

//�f�X�g���N�^
EnemySlime::~EnemySlime()
{
	
}

//�X�V����
void EnemySlime::Update(float elapsedTime)
{
	//���͍X�V����
	//UpdateVelocity(elapsedTime);

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	//���f���s��X�V
	//model->UpdateTransform();

	//���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);
}

//�`�揈��
void EnemySlime::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, models[model_index], ShaderId::Lambert);

}


void EnemySlime::CreateModel()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int>dist(0, 3);

	model_index = dist(gen);
}

void EnemySlime::OnDead()
{
	//���M��j��
	Destroy();
}
