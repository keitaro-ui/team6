#include"System/Input.h"
#include"CameraController.h"
#include"Camera.h"
#include<System/Graphics.h>
#include "imgui.h"

void CameraController::Initialize()
{

}

//�X�V����
void CameraController::Update(float elapsedTime)
{
	/*GamePad& gamePad = Input::Instance().GetGamePad();*/
	Mouse& mouse = Input::Instance().GetMouse();

	CameraController::MouseCamera(elapsedTime);

	//float mouseDeltaPosX = mouse.GetPositionX() - mouse.GetOldPositionX();
	//float mouseDeltaPosY = mouse.GetPositionY() - mouse.GetOldPositionY();
	float mouseDeltaPosX = mouse.GetDeltaPositionX();
	float mouseDeltaPosY = mouse.GetDeltaPositionY();

	float movePower = 0.01f;

	float ax = mouseDeltaPosX * movePower;
	float ay = mouseDeltaPosY * movePower;
	
	angle.x -= ay * 0.2f;
	angle.y += ax * 0.2f;


	//X���̃J������]�𐧌�
	if (angle.x < minAngleX)
	{
		angle.x = minAngleX;
	}
	if (angle.x > maxAngleX)
	{
		angle.x = maxAngleX;
	}

	if (angle.y < minAngleY)
	{
		angle.y = minAngleY;
	}
	if (angle.y > maxAngleY)
	{
		angle.y = maxAngleY;
	}


	//Y���̉�]�l��-3.14�`3.14�Ɏ��܂�悤�ɂ���
	if (angle.y < -DirectX::XM_PI)
	{
		angle.y += DirectX::XM_2PI;
	}
	if (angle.y > DirectX::XM_PI)
	{
		angle.y -= DirectX::XM_2PI;
	}

	//�J������]�l����]�s��ɕϊ�
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	//��]�s�񂩂�O�����x�N�g�������o��
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	//�����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
	
	eye.x = target.x - front.x;// *range;
	eye.y = target.y + front.y;// *range;
	eye.z = target.z - front.z;// *range;

	//�����Ă�������̃x�N�g�����v�Z�ŏo��
	dir.x = target.x - eye.x;
	dir.y = target.y - eye.y;
	dir.z = target.z - eye.z;
	float len = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
	dir.x /= len;
	dir.y /= len;
	dir.z /= len;

	//�J�����̎��_�ƒ����_��ݒ�
	Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));

	//SetCursorPos(Graphics::Instance().GetScreenWidth() / 2.0f, Graphics::Instance().GetScreenHeight() / 2.0f);
	//mouse.Update();
}

void CameraController::MouseCamera(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	Mouse& mouse = Input::Instance().GetMouse();

	// IMGUI�̃}�E�X���͒l���g���ăJ�������삷��
	ImGuiIO io = ImGui::GetIO();

	// �}�E�X�J�[�\���̈ړ��ʂ����߂�
	float moveX = io.MouseDelta.x * 0.02f;
	float moveY = io.MouseDelta.y * 0.02f;

	// �J�������s�ړ�
	if (io.MouseDown[ImGuiMouseButton_Middle])
	{
		float mousePosX = io.MousePos.x;
		float mousePosY = io.MousePos.y;

		DirectX::XMFLOAT2 mousePos = { mousePosX,mousePosY };
		//target += {mousePos.x, 0, mousePos.y};
	}

	// �}�E�X���{�^��������
	if (io.MouseDown[ImGuiMouseButton_Left])
	{
		// Y����]
		angle.y += moveX * 0.5f;
		if (angle.y > DirectX::XM_PI)
		{
			angle.y -= DirectX::XM_2PI;
		}
		else if (angle.y < -DirectX::XM_PI)
		{
			angle.y += DirectX::XM_2PI;
		}
		// X����]
		angle.x += moveY * 0.5f;
		if (angle.x > DirectX::XM_PI)
		{
			angle.x -= DirectX::XM_2PI;
		}
		else if (angle.x < -DirectX::XM_PI)
		{
			angle.x += DirectX::XM_2PI;
		}
	}

	// �}�E�X�z�C�[��
	else if (io.MouseWheel != 0)
	{
		// �Y�[��
		range -= io.MouseWheel * range * 0.1f;
	}
}

