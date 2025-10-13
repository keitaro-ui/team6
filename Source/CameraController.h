#pragma once

#include <DirectXMath.h>

//�J�����R���g���[���[
class CameraController
{
public:
	void Initialize();

	//�X�V����
	void Update(float elapsedTime);

	//�^�[�Q�b�g�ʒu�ݒ�
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }
	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 dir = {};

	const DirectX::XMFLOAT3& getAngle() const{ return angle; };

	void MouseCamera(float elapsedTime);

	DirectX::XMFLOAT3		target = { 0, 0, 0 }; //�����_
	DirectX::XMFLOAT3		angle = { 0, 0, 0 }; //��]�p�x
private:
	float					rollSpeed = DirectX::XMConvertToRadians(90); //��]���x
	float					range = 10.0f; //����
	
	float					maxAngleX = DirectX::XMConvertToRadians(30);
	float					minAngleX = DirectX::XMConvertToRadians(-30);

	float					maxAngleY = DirectX::XMConvertToRadians(45);
	float					minAngleY = DirectX::XMConvertToRadians(-45);
};
