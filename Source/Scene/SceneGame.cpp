#include "System/Graphics.h"
#include"System/Input.h"
#include "SceneGame.h"
#include "Camera.h"
#include "../Game/EnemyManager.h"
#include "../Game/EnemySlime.h"
#include "../Game/Board.h"
#include "time.h"
#include "random"
#include "algorithm"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneLoading.h"
#include "SceneResult.h"

//float game_timer;
extern int answer, count_1, count_2, count_3, count_4;

// ������
void SceneGame::Initialize()
{
	//�X�e�[�W������
	stage = new Stage();
	game_timer = 15;

	//�v���C���[������
	player = new Player();

	//���f���ǂݍ���
	sprite = new Sprite("Data/Sprite/���e�B�N��.png");
	sprite_number = new Sprite("Data/Sprite/number.png");
	sprite_text = new Sprite("Data/Sprite/�c�莞��.png");
	RoadModel();
	

	//�J���������ݒ�
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),//���_
		DirectX::XMFLOAT3(0, 0, 0),//�����_
		DirectX::XMFLOAT3(0, 1, 0)//�����
		);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),//����p
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),//��ʃA�X�y�N�g��
		0.1f,//�N���b�v�����i�߁j
		1000.0f//�N���b�v�����i���j
	);
	//�J�����R���g���[���[������
	cameraController = new CameraController();
	player->cameraController = cameraController;

	//�G�l�~�[������
	EnemyManager& enemyManager = EnemyManager::Instance();
	for (int i = 0; i < 35; i++)
	{
		EnemySlime* target = new EnemySlime();

		Board* board = new Board();

		//�G�l�~�[�ʒu
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float>dist (-4.0f, 4.0f);
		std::uniform_real_distribution<float>dist2(0.5f, 4.0f);
		std::uniform_real_distribution<float>dist3(5.0f, 10.0f);

		target->SetPosition(DirectX::XMFLOAT3(dist(gen), dist2(gen), dist3(gen)));
		target->SetAngle(DirectX::XMFLOAT3(0, DirectX::XM_PI, 0));
		

		board->SetPosition(DirectX::XMFLOAT3(0, 1.0f, 3));
		board->SetAngle(DirectX::XMFLOAT3(0, DirectX::XM_PI, 0));
		
		enemyManager.Register(target);
		//enemyManager.Register(board);
	}

	//�}�E�X�ʒu�̎擾�ƃ��b�N
	Input::Instance().GetMouse().Lock();
}

// �I����
void SceneGame::Finalize()
{
	Input::Instance().GetMouse().Unlock();

	//�J�����R���g���[���[�I����
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	//�X�e�[�W�I����
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}

	//�v���C���[�I����
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	//�G�l�~�[�I����
	EnemyManager::Instance().Clear();

	game_timer = 0;
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	//�J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	player->Update(elapsedTime);
	targetCheck();

	//�X�e�[�W�X�V����
	stage->Update(elapsedTime);

	//�v���C���[�X�V����

	//�G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);

	//�V�[���J��
	GamePad& gamePad = Input::Instance().GetGamePad();

	const GamePadButton anyButton =
		GamePad::BTN_B;

	//game_timer++;

	game_timer -= elapsedTime;

	if (game_timer < 0)
	//if (gamePad.GetButtonDown() & anyButton)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneResult));
	}

}

// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	// �`�揀��
	RenderContext rc;
	rc.deviceContext = dc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f };	// ���C�g�����i�������j
	rc.renderState = graphics.GetRenderState();

	//�`�揈��
	//RenderContext rc;
	rc.renderState = graphics.GetRenderState();
	rc.lightDirection = { 0.0f , -1.0f , 0.0f };	//���C�g�����i�������j

	//�J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3D���f���`��
	{
		//�X�e�[�W�`��
		stage->Render(rc, modelRenderer);

		player->Render(rc, modelRenderer);

		EnemyManager::Instance().Render(rc, modelRenderer);

		player->RenderDebugPrimitive(rc, shapeRenderer);
	}

	// 3D�f�o�b�O�`��
	{
		//�v���C���[�f�o�b�O�v���~�e�B�u�`��
		//player->RenderDebugPrimitive(rc, shapeRenderer);

		//�G�l�~�[�f�o�b�O�v���~�e�B�u�`��
		EnemyManager::Instance();
			//.RenderDebugPrimitive(rc, shapeRenderer);
	}

	// 2D�X�v���C�g�`��
	{
		sprite->Render(rc,
			610, 335, 0, 64.0f, 64.0f,
			0,
			1, 1, 1, 1);

		sprite_text->Render(rc,
			950, 0, 0, 120, 80, 0, 1, 1, 1, 1);

		int n[2]{};
		//n[0] = static_cast<int>(game_timer) / 100 % 10;
		n[0] = static_cast<int>(game_timer) / 10 % 10;
		n[1] = static_cast<int>(game_timer) % 10;

		for (int i = 0; i < 2; i++)
		{
			sprite_number->Render(rc,
				32 * 2 * i + 1100, 00,
				0,
				32 * 2, 32 * 2,
				372.5 * n[i], 0,
				372.5, 514,
				0,
				1, 1, 1, 1);
		}
	}
}

// GUI�`��
void SceneGame::DrawGUI()
{
	//�v���[���[�f�o�b�O����
	//player->DrawDebugGUI();

	
}
void SceneGame::targetCheck()
{
	if (answer == 0)
		count_1++;
	else if (answer == 1)
		count_2++;
	else if (answer == 2)
		count_3++;
	else if (answer == 3)
		count_4++;

	answer = -1;

}
