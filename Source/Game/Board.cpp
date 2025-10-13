#include "Board.h"

#include <random>
#include <ctime>
#include <cstdlib>

Model* model_boad[20];
int board_answer = -1;
int BoardModel_index = -1;
bool CreateSelect = true;

//�R���X�g���N�^
Board::Board()
{

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.6f;

	//���A�����ݒ�
	radius = 0.2f;
	height = 0.0f;
	if (CreateSelect == true)
	{
		CreateModel();
	}
	CreateSelect = false;
}

//�f�X�g���N�^
Board::~Board()
{
	
}

//�X�V����
void Board::Update(float elapsedTime)
{
	//���͍X�V����
	//UpdateVelocity(elapsedTime);

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	//���f���s��X�V
	//model_->UpdateTransform();

	//���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);
}

//�`�揈��
void Board::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model_boad[BoardModel_index], ShaderId::Lambert);
}

void Board::CreateModel()
{
	srand((unsigned int)time(NULL));

	/*std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int>dist(1, 20);
	
	BoardModel_index = dist(gen);
	board_answer = BoardModel_index;*/

	std::srand(static_cast<unsigned int>(std::time(0)));
	BoardModel_index = std::rand() % 20 + 1;
	board_answer = BoardModel_index;
}

void Board::OnDead()
{
	//���M��j��
	/*if (scale.x > 0.0f)
	{
		Destroy();
	}*/
}
