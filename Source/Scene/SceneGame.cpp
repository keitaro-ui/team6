#include "System/Graphics.h"
#include"System/Input.h"
#include "Camera.h"
#include "../Game/EnemyManager.h"
#include "time.h"
#include "random"
#include "algorithm"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneLoading.h"

//float game_timer;

// ������
void SceneGame::Initialize()
{
	//�X�e�[�W������
	stage = std::make_unique<Stage>();

	//�v���C���[������
	player = std::make_unique<Player>();

	//����
	game_timer = 30;

	//�X�v���C�g�����ݒ�
	sprite = std::make_unique<Sprite>("Data/Sprite/���e�B�N��.png");
	sprite_number = std::make_unique<Sprite>("Data/Sprite/number.png");
	sprite_text = std::make_unique<Sprite>("Data/Sprite/�c�莞��.png");

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
		std::uniform_real_distribution<float>dist (-4.0f, 4.0f);
		std::uniform_real_distribution<float>dist2(0.5f, 4.0f);
		std::uniform_real_distribution<float>dist3(5.0f, 10.0f);

		balloon->SetPosition(DirectX::XMFLOAT3(dist(gen), dist2(gen), dist3(gen)));
		balloon->SetAngle(DirectX::XMFLOAT3(0, DirectX::XM_PI, 0));
		

		board->SetPosition(DirectX::XMFLOAT3(0, 4.0f, 14));
		board->SetAngle(DirectX::XMFLOAT3(0, DirectX::XM_PI, 0));

		if (i % 5 == 0) { num = 0; hei++; }
		
		box->SetPosition(DirectX::XMFLOAT3(-4.6f+2.3f*num, 1.45f*hei, 12));

		num++;
		
		enemyManager.Register(balloon);
		enemyManager.Register(board);
		enemyManager.Register(box);
		balloon->box = box;
	}

	//�}�E�X�ʒu�̎擾�ƃ��b�N
	Input::Instance().GetMouse().Lock();
}

// �I����
void SceneGame::Finalize()
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
void SceneGame::Update(float elapsedTime)
{

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

	//�V�[���J��
	GamePad& gamePad = Input::Instance().GetGamePad();

	const GamePadButton anyButton =
		GamePad::BTN_START;

	game_timer -= elapsedTime;

	//if (gamePad.GetButtonDown() & anyButton)
	if(game_timer < 0)
	{
		//SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
		player->finish = true;
	}

	if (gamePad.GetButtonDown() & anyButton&&player->finish==true)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
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
		//EnemyManager::Instance().Render(rc, modelRenderer);
		//player->Render(rc, modelRenderer);

		//EnemyManager::Instance().Render(rc, modelRenderer);

		//player->RenderDebugPrimitive(rc, shapeRenderer);
	}

	// 3D�f�o�b�O�`��
	{
		//�v���C���[�f�o�b�O�v���~�e�B�u�`��
		//player->RenderDebugPrimitive(rc, shapeRenderer);

		//�G�l�~�[�f�o�b�O�v���~�e�B�u�`��
		//EnemyManager::Instance(); 
		//.RenderDebugPrimitive(rc, shapeRenderer);
	}

	// 2D�X�v���C�g�`��
	{
		
	}
}

// GUI�`��
void SceneGame::DrawGUI()
{
	//�v���[���[�f�o�b�O����
	player->DrawDebugGUI();
}

void SceneGame::RoadModel()
{
	//�I�̃��[�h
	extern Model* models[4];

	models[0] = new Model("Data/Model/Target/target_1.mdl");
	models[1] = new Model("Data/Model/Target/target_2.mdl");
	models[2] = new Model("Data/Model/Target/target_3.mdl");
	models[3] = new Model("Data/Model/Target/target_4.mdl");

	//��蕶�̃��[�h
	extern Model* model_boad[21];

	model_boad[0] = new Model("Data/Model/Boad/boad_tutorial.mdl");
	model_boad[1] = new Model("Data/Model/Boad/boad_1.mdl");
	model_boad[2] = new Model("Data/Model/Boad/boad_2.mdl");
	model_boad[3] = new Model("Data/Model/Boad/boad_3.mdl");
	model_boad[4] = new Model("Data/Model/Boad/boad_4.mdl");
	model_boad[5] = new Model("Data/Model/Boad/boad_5.mdl");
	model_boad[6] = new Model("Data/Model/Boad/boad_6.mdl");
	model_boad[7] = new Model("Data/Model/Boad/boad_7.mdl");
	model_boad[8] = new Model("Data/Model/Boad/boad_8.mdl");
	model_boad[9] = new Model("Data/Model/Boad/boad_9.mdl");
	model_boad[10] = new Model("Data/Model/Boad/boad_10.mdl");
	model_boad[11] = new Model("Data/Model/Boad/boad_11.mdl");
	model_boad[12] = new Model("Data/Model/Boad/boad_12.mdl");
	model_boad[13] = new Model("Data/Model/Boad/boad_13.mdl");
	model_boad[14] = new Model("Data/Model/Boad/boad_14.mdl");
	model_boad[15] = new Model("Data/Model/Boad/boad_15.mdl");
	model_boad[16] = new Model("Data/Model/Boad/boad_16.mdl");
	model_boad[17] = new Model("Data/Model/Boad/boad_17.mdl");
	model_boad[18] = new Model("Data/Model/Boad/boad_18.mdl");
	model_boad[19] = new Model("Data/Model/Boad/boad_19.mdl");
	model_boad[20] = new Model("Data/Model/Boad/boad_20.mdl");
}
