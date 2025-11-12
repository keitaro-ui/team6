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


//コンストラクタ
Player::Player()
{
	model = new Model("Data/Model/Player/pasted__pCube2.mdl");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.21f;

	
}

//デストラクタ
Player::~Player()
{
	delete model;
}

//マウス操作用の変数
POINT cursorPos;
DirectX::XMFLOAT3 ndc = {};

//更新処理
void Player::Update(float elapsedTime)
{
	shottimer++;
	//Mouse& mouse = Input::Instance().GetMouse();
	////移動入力処理
	InputMove(elapsedTime);

	////ジャンプ入力処理
	//InputJump();

	angle.x = -cameraController->getAngle().x;
	angle.y = cameraController->getAngle().y - DirectX::XM_PIDIV2;

	//弾の間隔
	coolgun(elapsedTime);

	//弾丸入力処理
	InputProjectile();

	//セーフティエリア処理
	InputSafetrSrea();

	if (!canPlaceSafeArea)
	{
		safeCooldown -= elapsedTime;
		if (safeCooldown <= 0.0f)
		{
			safeCooldown = 0.0f;
			canPlaceSafeArea = true;
		}
	}

	//速力処理更新
	UpdateVelocity(elapsedTime);

	//弾丸更新処理
	//projectileManager.Update(elapsedTime);

	//プレイヤーとエネミーとの衝突処理
	CollisionPlayerVsEnemies();

	//弾丸と敵の衝突処理
	CollisionProjectilesVsEnemies();

	//マウス操作
	SStws();

	//オブジェクト行列を更新
	UpdateTransform();
	model->UpdateTransform();

	if (vibe_interval == false)
	{
		float w = 5.0f; // 角速度

		v_angle += w * elapsedTime;

		float amp = 0.4f; // 振幅
		//cameraController->angle.x = amp * sinf(v_angle);
		cameraController->angle.x = amp * ((sinf(v_angle) + 1.0) * 0.5);
		//takeSE->Play(false);
	}

	//mouse.Update();
}

//移動入力処理
void Player::InputMove(float elapsedTime)
{
	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	//移動処理
	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

	//旋回処理
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

}

//弾丸入力処理
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	Mouse& mouse = Input::Instance().GetMouse();
	//if (finish == false)
	{
		//if (interval == true)
		{

			//if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
		
		}
	}


}

void Player::coolgun(float elpasedTime)
{
	Camera& camera = Camera::Instance();
	guntime -= elpasedTime;
	v_guntime -= elpasedTime;
	if (guntime <= 0)interval = true;
	if (v_guntime <= 0)vibe_interval = true;
}

//デバッグ用GUI描画
void Player::DrawDebugGUI()
{
	ImGui::DragFloat3("pos", &position.x,0.01f);
	
	// 最大数を調整
	ImGui::DragInt("Max Safety Area Count", &maxSafetyAreaCount, 1, 1, 20);

	// 現在の設置数を表示
	ImGui::Text("Current Count: %d / %d", (int)safetyAreas.size(), maxSafetyAreaCount);

	if (ImGui::CollapsingHeader("Spawned SafetyAreas"))
	{
			// 半径
		ImGui::DragFloat(("Radius"), &radius, 0.01f, 0.1f, 50.0f);

			// 高さ
		ImGui::DragFloat(("Height"), &height, 0.01f, 0.1f, 20.0f);

		for (int i = 0; i < safetyAreas.size(); ++i)
		{
			SafetyArea* area = safetyAreas[i];
			if (!area) continue;

			std::string label = "SpawnPos " + std::to_string(i);
			ImGui::DragFloat3(label.c_str(), &area->position.x, 0.01f);
		}
	}
}

//描画処理
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);

	//弾丸描画処理
	//projectileManager.Render(rc, renderer);
	DirectX::XMFLOAT4X4 projectileTransform = model->GetNode("tipofPistol")->globalTransform;
	DirectX::XMMATRIX projectileMATRIX = DirectX::XMLoadFloat4x4(&projectileTransform);
	DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&transform);
	DirectX::XMStoreFloat4x4(&projectileTransform, projectileMATRIX * worldMatrix);

	//弾丸射出箇所の可視化
	/*Graphics::Instance().GetShapeRenderer()->RenderSphere(rc,
		{ projectileTransform._41,projectileTransform._42,projectileTransform._43 },
		0.1f,
		{ 1.0f,1.0f,1.0f,1.0f });*/
}

//デバッグプリミティブ描画
void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//プレイヤーデバッグプリミティブ描画
	//Character::RenderDebugPrimitive(rc, renderer);

	//弾丸デバッグプリミティブ描画
	//projectileManager.RenderDebugPrimitive(rc, renderer);

	for (auto* area : safetyAreas)
	{
		if (!area) continue;

		const DirectX::XMFLOAT3& center = {
			area->GetPosition().x,
			0.5f,
			area->GetPosition().z
		};

		renderer->RenderCylinder(rc, center, radius,-height, { 0.2f, 1.0f, 0.2f, 1.0f });
	}
}

//スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//移動ベクトルはXZ平面に水平なベクトルになるようにする

	//カメラ右方向ベクトルをXZ平面ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);

	if (cameraRightLength > 0.0f)
	{
		//単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//カメラ前方向をXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);

	if (cameraFrontLength > 0.0f)
	{
		//単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//スティックの水平入力値をカメラ右方向に反映し、
	//スティックの垂直入力値をカメラ前方向に反映し、
	//進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	//Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}

//プレイヤーと敵の衝突判定
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyManager.GetEnemyCount(); i++)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//衝突処理
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsSphere(
			position, radius,
			enemy->GetPosition(),
			enemy->GetRadius(),
			outPosition))
		{
			//押し出し後の位置設定
			enemy->SetPosition(outPosition);
		}
	}
}


//弾丸と敵の衝突処理
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての弾丸と全ての敵を総当たりで衝突処理
	int projectileCount = ProjectileManager::Instance().GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = ProjectileManager::Instance().GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);


			//衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsSphere(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				outPosition))
			{
				answer = enemy->model_index;

				//ダメージを与える
				if (enemy->ApplyDamage(1, 0.5f))
				{
					//弾丸破棄
					projectile->Destroy();
					hitSE->Play(false);
				}
				break;
			}
		}
	}
}

//マウス操作
void Player::SStws()
{
	// マウスカーソルの位置を格納する構造体
	GetCursorPos(&cursorPos);
	// ウィンドウのハンドル（現在アクティブなウィンドウ）
	HWND hwnd = GetForegroundWindow();
	// ScreenToClientで画面座標をウィンドウのクライアント領域座標に変換
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

void Player::InputSafetrSrea()
{
	if (GetAsyncKeyState('R') & 1 && canPlaceSafeArea && maxSafetyAreaCount>0)
	{
		DirectX::XMFLOAT3 forward = Camera::Instance().GetFront();

		DirectX::XMFLOAT3 spawnPos = {
		  position.x + forward.x,
		  position.y + 1.0f,
		  position.z + forward.z 
		};

		// ===== SafetyArea生成 =====
		SafetyArea* area = new SafetyArea(&ProjectileManager::Instance());
		area->SetPosition(spawnPos);
		maxSafetyAreaCount -= 1;
		safetyAreas.push_back(area);

		canPlaceSafeArea = false;
		safeCooldown = safeInterval;
	}


}

//着地したときに呼ばれる
void Player::OnLanding()
{
	jumpCount = 0;
}

//ジャンプ入力処理
void Player::InputJump()
{
	//ボタン入力でジャンプ（回数制限つき）
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		//ジャンプ回数制限
		if (jumpCount < jumpLimit)
		{
			Jump(jumpSpeed);
			jumpCount++;
		}
	}
}

