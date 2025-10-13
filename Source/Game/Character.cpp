#include "Character.h"
#include "System/ShapeRenderer.h"

//�s��X�V����
void Character::UpdateTransform()
{
	//�X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//��]�s����쐬
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x,angle.y,angle.z);
	DirectX::XMMATRIX Rx = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Ry = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Rz = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Rz * Ry * Rx;
	//DirectX::XMMATRIX R = Rx;

	//�ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//3�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;

	//�v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

//�f�o�b�O�v���~�e�B�u�`��
void Character::RenderDebugPrimitive(const RenderContext & rc, ShapeRenderer * renderer)
{
	//�Փ˔���p�̃f�o�b�O�~����`��
	renderer->RenderCylinder(rc, position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

//�_���[�W��^����
bool Character::ApplyDamage(int damage, float invincibleTime)
{
	//�_���[�W0�Ȃ猒�N��Ԃ�ύX����K�v���Ȃ�
	if (damage == 0) return false;
	
	//���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
	if (health <= 0) return false;

	//���G���Ԓ��̓_���[�W��^���Ȃ�
	if (invincibleTimer >= 0.0f) return false;

	//���G���Ԑݒ�
	invincibleTimer = invincibleTime;

	//�_���[�W����
	health -= damage;

	//���S�ʒm
	if (health <= 0)
	{
		OnDead();
	}

	//�_���[�W�ʒm
	else
	{
		OnDamaged();
	}

	//���N��Ԃ�ύX������true��Ԃ�
	return true;
}

//�ړ�����
void Character::Move(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;
	position.x += vx * speed;
	position.z += vz * speed;
}

void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	//�i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f) return;

	//�i�s�x�N�g����P�ʃx�N�g����
	vx /= length;
	vz /= length;

	//���g�̉�]�l����O���������߂�
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//��]�p�����߂邽�߁A2�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = (frontX * vx) + (frontZ * vz);

	//���ϒl��-1.0�`1.0�ŕ\������Ă��āA2�̒P�ʃx�N�g���̊p�x��
	//�������قǂ�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐߂���
	float rot = 1.0f - dot;
	if (rot > speed) rot = speed;

	//���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = (frontZ * vx) - (frontX * vz);

	//2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E������s����
	//���E������s�����Ƃɂ���č��E��]��I������
	if (cross < 0.0f)
	{
		angle.y -= rot;
	}
	else
	{
		angle.y += rot;
	}
}

//�W�����v����
void Character::Jump(float speed)
{
	//������̗͂�ݒ�
	velocity.y = speed;
}

//���͍X�V����
void Character::UpdateVelocity(float elapsedTime)
{
	//�d�͏���
	velocity.y += gravity * elapsedTime;

	//�ړ�����
	position.y += velocity.y * elapsedTime;

	//�n�ʔ���
	if (position.y < 0.0f)
	{
		position.y = 0.0f;
		velocity.y = 0.0f;

		//���n����
		if (!isGround)
		{
			OnLanding();
		}
		isGround = true;
	}
	else
	{
		isGround = false;
	}
}

//���G���ԍX�V
void Character::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}
