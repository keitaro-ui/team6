#pragma once


#include <DirectXMath.h>
#include "System/ShapeRenderer.h"

//�L�����N�^�[
class Character
{
public:
	Character(){}
	virtual ~Character(){}

	//�s��X�V����
	void UpdateTransform();

	//�ʒu�擾
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	//�ʒu�ݒ�
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	
	//��]�擾
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }

	//��]�ݒ�
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	//�X�P�[���擾
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	//�X�P�[���ݒ�
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	//���a�擾
	float GetRadius() const { return radius; }

	//�����擾
	float GetHeight() const { return height; }

	//�f�o�b�O�v���~�e�B�u�`��
	virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	//�ڒn����
	bool IsGround() const { return isGround; }

	//�_���[�W��^����
	bool ApplyDamage(int damage, float invincibleTime);

protected:
	DirectX::XMFLOAT3		position = { 0,1,0 };
	DirectX::XMFLOAT3		angle = { 0,0,0 };
	DirectX::XMFLOAT3		scale = { 1,1,1 };
	DirectX::XMFLOAT4X4		transform =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	float radius = 0.5f;
	float height = 2.0f;

	//�ړ�����
	void Move(float elapsedTime, float vx, float vz, float speed);

	//���񏈗�
	void Turn(float elapsedTime, float vx, float vz, float speed);

	//�W�����v����
	void Jump(float speed);

	//���͏����X�V
	void UpdateVelocity(float elapsedTime);

	//���G���ԍX�V
	void UpdateInvincibleTimer(float elapsedTime);

	//���n�������ɌĂ΂��
	virtual void OnLanding() {}

	//�_���[�W�󂯂���Ă΂��
	virtual void OnDamaged() {}

	//���S������Ă΂��
	virtual void OnDead() {}

protected:
	float				gravity = 0.0f;
	DirectX::XMFLOAT3	velocity = { 0, 0, 0 };

	bool				isGround = false;

	int					health = 1;
	float				invincibleTimer = 1.0f;
};
