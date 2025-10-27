#include"Player.h"
#include"System/Input.h"
#include<imgui.h>
#include "Camera.h"

#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectHoming.h"
#include "../System/Graphics.h"
#include "Camera.h"
#include "System/Audio.h"


int answer = -1, count_1, count_2, count_3, count_4;


//�R���X�g���N�^
Player::Player()
{
	model = new Model("Data/Model/Player/pasted__pCube2.mdl");

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.21f;

	//�q�b�gSE�ǂݍ���
	/*hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");
	shotSE = Audio::Instance().LoadAudioSource("Data/Sound/revolver.wav");
	takeSE = Audio::Instance().LoadAudioSource("Data/Sound/take revolver.wav");*/
}

//�f�X�g���N�^
Player::~Player()
{
	delete model;
}

//�}�E�X����p�̕ϐ�
POINT cursorPos;
DirectX::XMFLOAT3 ndc = {};

//�X�V����
void Player::Update(float elapsedTime)
{
	shottimer++;
	//Mouse& mouse = Input::Instance().GetMouse();
	////�ړ����͏���
	InputMove(elapsedTime);

	////�W�����v���͏���
	//InputJump();

	angle.x = -cameraController->getAngle().x;
	angle.y = cameraController->getAngle().y - DirectX::XM_PIDIV2;

	//�e�̊Ԋu
	coolgun(elapsedTime);

	//�e�ۓ��͏���
	InputProjectile();

	//���͏����X�V
	UpdateVelocity(elapsedTime);

	//�e�ۍX�V����
	projectileManager.Update(elapsedTime);

	//�v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
	CollisionPlayerVsEnemies();

	//�e�ۂƓG�̏Փˏ���
	CollisionProjectilesVsEnemies();

	//�}�E�X����
	SStws();

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();
	model->UpdateTransform();

	if (vibe_interval == false)
	{
		float w = 5.0f; // �p���x

		v_angle += w * elapsedTime;

		float amp = 0.4f; // �U��
		//cameraController->angle.x = amp * sinf(v_angle);
		cameraController->angle.x = amp * ((sinf(v_angle) + 1.0) * 0.5);
		takeSE->Play(false);
	}

	//mouse.Update();
}

//�ړ����͏���
void Player::InputMove(float elapsedTime)
{
	//�i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	//�ړ�����
	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

	//���񏈗�
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

}

//�e�ۓ��͏���
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	Mouse& mouse = Input::Instance().GetMouse();
	if (finish == false)
	{
		if (interval == true)
		{
			//���i�e�۔���
			if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
			{
				shotSE->Play(false);
				guntime = 1.3f;		//�e�̔��ˊԊu
				v_guntime = 0.7f;
				v_angle = 0;
				//�O����
				DirectX::XMFLOAT3 dir;
				dir = cameraController->dir;

				//���ˈʒu�i�v���C���[�̍�������j
				DirectX::XMFLOAT3 pos;
				pos.x = position.x + dir.x * 0.20f;
				pos.y = position.y + height * 0.25f - 0.05;
				pos.z = position.z + dir.z * 0.20f;

				//����
				ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);

				DirectX::XMFLOAT4X4 projectileTransform = model->GetNode("tipofPistol")->globalTransform;
				DirectX::XMMATRIX projectileMATRIX = DirectX::XMLoadFloat4x4(&projectileTransform);
				DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&transform);

				// X���𔽓]����X�P�[�����O�s����쐬
				//DirectX::XMMATRIX flipMatrix = DirectX::XMMatrixScaling(-1.0f, 1.0f, 1.0f);
				//projectileMATRIX = DirectX::XMMatrixMultiply(projectileMATRIX * worldMatrix, flipMatrix);
				DirectX::XMStoreFloat4x4(&projectileTransform, projectileMATRIX * worldMatrix);

				projectile->Launch(dir, { projectileTransform._41,projectileTransform._42 + 0.1f,projectileTransform._43 });
				//projectile->Launch(dir, pos);
				//projectileManager.Register(projectile);

				interval = false;
				vibe_interval = false;
			}
		}
	}

#if 0
	//�ǔ��e����
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		//�O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0;
		dir.z = cosf(angle.y);

		//���ˈʒu�i�v���C���[�̍�������j
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;


		//�^�[�Q�b�g
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		//��ԋ߂��̓G���^�[�Q�b�g����
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; ++i)
		{
			//�G�Ƃ̋�������
			Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);

			float d;
			DirectX::XMStoreFloat(&d, D);
			if (d < dist)
			{
				dist = d;
				target = enemy->GetPosition();
				target.y += enemy->GetHeight() * 0.5f;
			}

		}

		//����
		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);

	}
#endif
}

void Player::coolgun(float elpasedTime)
{
	Camera& camera = Camera::Instance();
	guntime -= elpasedTime;
	v_guntime -= elpasedTime;
	if (guntime <= 0)interval = true;
	if (v_guntime <= 0)vibe_interval = true;
}

//�f�o�b�O�pGUI�`��
void Player::DrawDebugGUI()
{
	//ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	//ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	//ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	//if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	//{
	//	//�}�E�X�̈ʒu
	//	ImGui::Text("x = %f", ndc.x);
	//	ImGui::Text("y = %f", ndc.y);

	//	//�I
	//	//extern int count_1, count_2, count_3, count_4;
	//	ImGui::Text("count_1 = %d", count_1);
	//	ImGui::Text("count_2 = %d", count_2);
	//	ImGui::Text("count_3 = %d", count_3);
	//	ImGui::Text("count_4 = %d", count_4);

	//	//�g�����X�t�H�[��
	//	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		//�ʒu
	//		ImGui::InputFloat3("Position", &position.x);
	//		//��]
	//		DirectX::XMFLOAT3 a;
	//		a.x = DirectX::XMConvertToDegrees(angle.x);
	//		a.y = DirectX::XMConvertToDegrees(angle.y);
	//		a.z = DirectX::XMConvertToDegrees(angle.z);
	//		ImGui::InputFloat3("Angle", &a.x);
	//		angle.x = DirectX::XMConvertToRadians(a.x);
	//		angle.y = DirectX::XMConvertToRadians(a.y);
	//		angle.z = DirectX::XMConvertToRadians(a.z);
	//		//�X�P�[��
	//		ImGui::InputFloat3("Scale", &scale.x);

	//		//�J����
	//		//ImGui::InputFloat3("eye",&)
	//	}
	//}
	//ImGui::End();
}

//�`�揈��
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);

	//�e�ە`�揈��
	projectileManager.Render(rc, renderer);
	DirectX::XMFLOAT4X4 projectileTransform = model->GetNode("tipofPistol")->globalTransform;
	DirectX::XMMATRIX projectileMATRIX = DirectX::XMLoadFloat4x4(&projectileTransform);
	DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&transform);
	DirectX::XMStoreFloat4x4(&projectileTransform, projectileMATRIX * worldMatrix);

	//�e�ێˏo�ӏ��̉���
	/*Graphics::Instance().GetShapeRenderer()->RenderSphere(rc,
		{ projectileTransform._41,projectileTransform._42,projectileTransform._43 },
		0.1f,
		{ 1.0f,1.0f,1.0f,1.0f });*/
}

//�f�o�b�O�v���~�e�B�u�`��
void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//�v���C���[�f�o�b�O�v���~�e�B�u�`��
	//Character::RenderDebugPrimitive(rc, renderer);

	//�e�ۃf�o�b�O�v���~�e�B�u�`��
	//projectileManager.RenderDebugPrimitive(rc, renderer);
}

//�X�e�B�b�N���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	//���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//�ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

	//�J�����E�����x�N�g����XZ���ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);

	if (cameraRightLength > 0.0f)
	{
		//�P�ʃx�N�g����
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//�J�����O������XZ�P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);

	if (cameraFrontLength > 0.0f)
	{
		//�P�ʃx�N�g����
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//�X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
	//�X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
	//�i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	//Y�������ɂ͈ړ����Ȃ�
	vec.y = 0.0f;

	return vec;
}

//�v���C���[�ƓG�̏Փ˔���
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//�S�Ă̓G�Ƒ�������ŏՓˏ���
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyManager.GetEnemyCount(); i++)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//�Փˏ���
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsSphere(
			position, radius,
			enemy->GetPosition(),
			enemy->GetRadius(),
			outPosition))
		{
			//�����o����̈ʒu�ݒ�
			enemy->SetPosition(outPosition);
		}
	}
}


//�e�ۂƓG�̏Փˏ���
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//�S�Ă̒e�ۂƑS�Ă̓G�𑍓�����ŏՓˏ���
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);


			//�Փˏ���
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsSphere(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				outPosition))
			{
				answer = enemy->model_index;

				//�_���[�W��^����
				if (enemy->ApplyDamage(1, 0.5f))
				{
					//�e�۔j��
					projectile->Destroy();
					hitSE->Play(false);
				}
				break;
			}
		}
	}
}

//�}�E�X����
void Player::SStws()
{
	// �}�E�X�J�[�\���̈ʒu���i�[����\����
	GetCursorPos(&cursorPos);
	// �E�B���h�E�̃n���h���i���݃A�N�e�B�u�ȃE�B���h�E�j
	HWND hwnd = GetForegroundWindow();
	// ScreenToClient�ŉ�ʍ��W���E�B���h�E�̃N���C�A���g�̈���W�ɕϊ�
	ScreenToClient(hwnd, &cursorPos);


	ndc.x = (cursorPos.x / (1280.0f / 2)) - 1.0f;
	ndc.y = ((cursorPos.y / (720.0f / 2)) - 1.0f) * -1;

	using namespace DirectX;
	XMMATRIX View = XMLoadFloat4x4(&Camera::Instance().GetView());
	XMMATRIX Projection = XMLoadFloat4x4(&Camera::Instance().GetProjection());

	XMMATRIX VP = View * Projection;
	XMMATRIX InvVP = XMMatrixInverse(nullptr, VP);

	XMVECTOR Ndc = XMLoadFloat3(&ndc);

	XMVECTOR World_pos = XMVector3TransformCoord(Ndc, InvVP);
	XMFLOAT4 STORE;
	DirectX::XMStoreFloat4(&STORE, World_pos);

	XMFLOAT3 w_pos;

	STORE.x /= STORE.w;
	STORE.y /= STORE.w;
	STORE.z /= STORE.w;

	w_pos = { STORE.x,STORE.y,STORE.z };
}

//���n�����Ƃ��ɌĂ΂��
void Player::OnLanding()
{
	jumpCount = 0;
}

//�W�����v���͏���
void Player::InputJump()
{
	//�{�^�����͂ŃW�����v�i�񐔐������j
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		//�W�����v�񐔐���
		if (jumpCount < jumpLimit)
		{
			Jump(jumpSpeed);
			jumpCount++;
		}
	}
}

