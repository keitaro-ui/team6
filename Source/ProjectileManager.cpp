#include "ProjectileManager.h"

//コンストラクタ
ProjectileManager::ProjectileManager()
{

}

//デストラクタ
ProjectileManager::~ProjectileManager()
{
	
	Clear();
}

//弾丸削除
void ProjectileManager::Remove(Projectile* projectile)
{
	removes.insert(projectile);
}

//更新処理
void ProjectileManager::Update(float elapsedTime)
{
	//更新処理
	for (Projectile* projectile : projectiles)
	{
		//if (projectile && !projectile->IsDead())
		{
			
			projectile->Update(elapsedTime);
		}
	}
	for (Projectile* p : removes)
	{
		// projectiles から取り除く
		auto it = std::find(projectiles.begin(), projectiles.end(), p);
		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}

		// メモリ開放
		delete p;
	}

	// removes を空に
	removes.clear();

}

//描画処理
void ProjectileManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (auto projectile : projectiles)
	{
		//if (projectile && !projectile->IsDead())
		projectile->Render(rc, renderer);
	}
	
}

//デバッグプリミティブ描画
void ProjectileManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->RenderDebugPrimitive(rc, renderer);
	}
}

//弾丸登録
void ProjectileManager::Register(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}

//弾丸全削除
void ProjectileManager::Clear()
{

	for (Projectile* projectile : projectiles)
	{
		delete projectile;
	}
	projectiles.clear();
}

