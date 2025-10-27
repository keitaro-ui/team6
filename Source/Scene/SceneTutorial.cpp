#include "System/Graphics.h"
#include"System/Input.h"
#include "SceneTutorial.h"
#include "Camera.h"
#include "../Game/EnemyManager.h"
#include "time.h"
#include "random"
#include "algorithm"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneLoading.h"


// ������
void SceneTutorial::Initialize()
{
	//�X�e�[�W������
	stage = std::make_unique<Stage>();

	//�v���C���[������
	player = std::make_unique<Player>();

	//���e�B�N���֐�
	sprites[0] = std::make_unique<Sprite>("Data/Sprite/���e�B�N��.png");
	sprites[1] = std::make_unique<Sprite>("Data/Sprite/�`���[�g���A�����.png");
	sprites[2] = std::make_unique<Sprite>("Data/Sprite/�`���[�g���A����.png");

	//���e�B�N���֐�
	SceneGame::RoadModel();
	
	
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
	int num = 0;
	int hei = 0;
	EnemyManager& enemyManager = EnemyManager::Instance();
	for (int i = 0; i < 20; i++)
	{
		//EnemySlime* target = new EnemySlime();
		balloon = new Balloon();
		/*target->setobjnum(i);*/

		//Board* board = new Board();
		board = new Board();

		//Box* box = new Box();
		box = new Box();
		/*box->setobjnum(i);*/


		//�G�l�~�[�ʒu
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float>dist(-4.0f, 4.0f);
		std::uniform_real_distribution<float>dist2(0.5f, 4.0f);
		std::uniform_real_distribution<float>dist3(5.0f, 10.0f);

		balloon->SetPosition(DirectX::XMFLOAT3(dist(gen), dist2(gen), dist3(gen)));
		balloon->SetAngle(DirectX::XMFLOAT3(0, DirectX::XM_PI, 0));


		board->SetPosition(DirectX::XMFLOAT3(0, 4.0f, 14));
		board->SetAngle(DirectX::XMFLOAT3(0, DirectX::XM_PI, 0));
		

		if (i % 5 == 0) { num = 0; hei++; }

		box->SetPosition(DirectX::XMFLOAT3(-4.6f + 2.3f * num, 1.45f * hei, 12));

		num++;

		enemyManager.Register(balloon);
		enemyManager.Register(board);
		enemyManager.Register(box);
		balloon->box = box;
	}

	

	//�}�E�X�ʒu�̎擾�ƃ��b�N
	Input::Instance().GetMouse().Lock();
	//Input::Instance().GetMouse();
}

// �I����
void SceneTutorial::Finalize()
{
	//mouse�̈ʒu���b�N��Ԃ�����
	Input::Instance().GetMouse().Unlock();

	//�J�����R���g���[���[�I����
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	//box�Ȃǂ�enemy���p�����Ă���new��delete���Ă͂����Ȃ��BEnemyManager���Ə���
	//�G�l�~�[�I����
	EnemyManager::Instance().Clear();
}

// �X�V����
void SceneTutorial::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	const GamePadButton anyButton =
		GamePad::BTN_START;
	

	switch (change)
	{
	case 0:

		if (gamePad.GetButtonDown() & anyButton)
		{
			change++;
		}

		break;

	case 1:
		//�J�����R���g���[���[�X�V����
		DirectX::XMFLOAT3 target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->Update(elapsedTime);

		player->Update(elapsedTime);

		//�X�e�[�W�X�V����
		stage->Update(elapsedTime);

		//�v���C���[�X�V����

		//�G�l�~�[�X�V����
		EnemyManager::Instance().Update(elapsedTime);

		if (gamePad.GetButtonDown() & anyButton)
		{
			change++;
		}

		break;

	case 2:

		//�V�[���J��
		if (gamePad.GetButtonDown() & anyButton)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
		}
		break;
	};
}

// �`�揈��
void SceneTutorial::Render()
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
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());

		if (change == 1)
		{
			sprites[0]->Render(rc,
				610, 335, 0, 64.0f, 64.0f,
				0,
				1, 1, 1, 1);
		}

		if (change == 0)
		{
			sprites[1]->Render(rc,
				0, 0, 0, screenWidth, screenHeight,
				0,
				1, 1, 1, 1);
		}

		if (change == 2)
		{
			sprites[2]->Render(rc,
				0, 0, 0, screenWidth, screenHeight,
				0,
				1, 1, 1, 1);
		}

	}
}

// GUI�`��
void SceneTutorial::DrawGUI()
{
	//�v���[���[�f�o�b�O����
	//player->DrawDebugGUI();
}
