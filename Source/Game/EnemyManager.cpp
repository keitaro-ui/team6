#include "EnemyManager.h"
#include "Collision.h"


//�G�l�~�[�o�^
void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

//�G�l�~�[�폜
void EnemyManager::Remove(Enemy* enemy)
{
	//�j�����X�g�ɒǉ�
	removes.insert(enemy);
}

//�X�V����
void EnemyManager::Update(float elapsedTime)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	//�j������
	//�@enemies�͈̔�for������erase()����ƕs����������Ă��܂����߁A
	//�@�X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	for (Enemy* enemy : removes)
	{
		//std::vector����v�f���폜����ꍇ�̓C�e���[�^�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
		if (it != enemies.end())
		{
			enemies.erase(it);
		}

		//�폜
		delete enemy;
	}
	//�j�����X�g�̃N���A
	removes.clear();

	//�G���m�̏Փˏ���
	CollisionEnemyVsEnemies();
}

//�`�揈��
void EnemyManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render(rc, renderer);
	}
}

//�f�o�b�O�v���~�e�B�u�`��
void EnemyManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (Enemy* enemy : enemies)
	{
		renderer->RenderSphere(rc, enemy->GetPosition(), enemy->GetRadius(), DirectX::XMFLOAT4(0, 0, 0, 1));
	}
}

//�G�l�~�[���m�̏Փˏ���
void EnemyManager::CollisionEnemyVsEnemies()
{
	size_t enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; i++)
	{
		Enemy* enemyA = enemies.at(i);
		for (int j = i + 1; j < enemyCount; j++)
		{
			Enemy* enemyB = enemies.at(j);

			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsSphere(
				enemyA->GetPosition(),
				enemyA->GetRadius(),
				enemyB->GetPosition(),
				enemyB->GetRadius(),
				outPosition
			))
			{
				enemyB->SetPosition(outPosition);
			}
		}
	}
}

//�G�l�~�[�S�폜
void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}
	enemies.clear();
}
