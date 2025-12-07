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
#include "SceneResult.h"
#include "../Game/PlayerManager.h"
#include "../Game/SafetyArea.h"
#include "../Source/System/Sprite.h"
#include "../Source/Game/Player.h"
#include "../Game/StartPoint.h"
#include "../Game/GoalPoint.h"
#include "../Source/Scene/ScenePassword.h"


// 初期化
void SceneGame::Initialize()
{
	//ステージ初期化
	stage = std::make_unique<Stage>();
	StageManager::Instance().Regist(stage.get());

	//プレイヤー初期化
	player = std::make_unique<Player>();
	

	PlayerManager::Instance().Register(player.get());

	enemyslime = std::make_unique<EnemySlime>();
	//enemyslime->player = player.get();


	start = std::make_unique<Start>();

	goal = std::make_unique<Goal>();

	//スタート位置初期化
	player->SetPosition({ 0.0f, 3.0f, -16.0f });
	Graphics& graphics = Graphics::Instance();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();
	player->SetRenderer(modelRenderer);

	renderer = modelRenderer;
	renderer->InitLights();
	

	//ゴール位置初期化
	goalPoint = new GoalPoint({ 0.0f, 3.0f, 17.0f });



	//スプライト初期設定
	{
		spr = std::make_unique<Sprite>("Data/Sprite/LoadingIcon.png");
		sprSpace = std::make_unique<Sprite>("Data/Sprite/space.png");
		sprR = std::make_unique<Sprite>("Data/Sprite/R.png");
		Graphics& graphics = Graphics::Instance();
		//スプライト読み込み
		hpBarTex = Sprite("Data/Sprite/Oxygen_gauge_Single.png");
		hpBarFrameTex = Sprite("Data/Sprite/Oxygen_gauge_frame.png");
		SafetyIconTex = Sprite("Data/Sprite/flag_UI.png");

	}
	passwordScene = new ScenePassword("3132");
	//カメラ初期設定
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),//視点
		DirectX::XMFLOAT3(0, 0, 0),//注視点
		DirectX::XMFLOAT3(0, 1, 0)//上方向
		);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),//視野角
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),//画面アスペクト比
		0.1f,//クリップ距離（近）
		1000.0f//クリップ距離（遠）
	);
	

	//カメラコントローラー初期化
	cameraController = new CameraController();
	player->cameraController = cameraController;

	//エネミー初期化
	EnemyManager& enemyManager = EnemyManager::Instance();
	//for (int i = 0; i < 20; i++)
	{
		//ランダムを出力した変数3つ
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float>dist (-4.0f, 4.0f);
		std::uniform_real_distribution<float>dist2(0.5f, 4.0f);
		std::uniform_real_distribution<float>dist3(5.0f, 10.0f);
	
		//エネミー位置
		balloon = new Balloon();
		balloon->SetPosition(DirectX::XMFLOAT3(dist(gen), dist2(gen), dist3(gen)));
		balloon->SetAngle(DirectX::XMFLOAT3(0, DirectX::XM_PI, 0));
		
		//クイズ板の初期設定
		for (int i = 0; i < 4; i++) 
		{
			boards[i] = new Board(i);
			EnemyManager::Instance().Register(boards[i]);
		}
	}

	//当たり判定
	{
		xDis = 9.0f;
		zDis = 6.6f;
		debugDoorSize = 2.5f;
		blockSize = { 3.0f, 0.0f, 1.6f };

		//外枠
		physics.AddObb({ 0, 0, 0 }, { 28.5f, 0, 17.7f }, 0);
		
		int map[5][6] =
		{
			// 1列目
			{ 1, 1, 0, 1, 1, 1 },

			// 2列目
			{ 1, -1, 1, 0, 0, -1 },

			// 3列目
			{ 1, 1, 1, 0, 1, 1 },

			// 4列目
			{ -1, 0, -1, 1, 1, 0 },

			// 5列目
			{ 0, 1, 0, -1, 0, 1 }
		};

		for (int z = 0; z < 5; z++)
		{
			for (int x = 0; x < 6; x++)
			{
				DirectX::XMFLOAT3 pos = { -22.5f + x * xDis, 0.0f, zDis * (2 - z) };
				
				int v = map[z][x];

				if (v == 0)
				{
					// 壁
					physics.AddObb(pos, blockSize, 0.0f);
				}
				else
				{
					// ドアの方向
					DirectX::XMFLOAT3 dir = { 0, 0, (float)v };

					physics.AddDoorObb(pos,blockSize,0.0f,dir,debugDoorSize,3.05f
					);
				}
			}
		}

		//通路塞ぐ壁
		{
			physics.AddObb({ -22.5f + 3 * xDis, 0.0f, zDis * 1.5f }, blockSize, 0.0f);
			physics.AddObb({ -22.5f + 3 * xDis, 0.0f, zDis * 0.5f }, blockSize, 0.0f);

			physics.AddObb({ -22.5f + 2 * xDis, 0.0f, zDis * -0.5f }, blockSize, 0.0f);
			physics.AddObb({ 0.0f, 0.0f, 0.0f }, blockSize, 0.0f);
		}
	}

	player->safetyAreas.push_back(new SafetyArea(&ProjectileManager::Instance()));
	player->safetyAreas.back()->SetPosition({ 0, -1.0f, -17.5 });

	//マウス位置の取得とロック
	Input::Instance().GetMouse().Lock();
	
	renderer->ResetHorror();
	//BGM
	SoundManager::Instance().GetSound(SoundList::gameBGM)->Play(true, 0.02f);
}

// 終了化
void SceneGame::Finalize()
{
	//mouseの位置ロック状態を解除
	Input::Instance().GetMouse().Unlock();

	//カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
	if (goalPoint != nullptr)
	{
		delete goalPoint;
		goalPoint = nullptr;
	}
	if (startPoint != nullptr)
	{
		delete startPoint;
		startPoint = nullptr;
	}
	if (activeBoard != nullptr)
	{
		delete activeBoard;
		activeBoard = nullptr;
	}
	if (passwordScene != nullptr)
	{
		delete passwordScene;
		passwordScene = nullptr;
	}

	delete balloon;

	//BGM
	SoundManager::Instance().GetSound(SoundList::gameBGM)->Stop();
	SoundManager::Instance().GetSound(SoundList::walkSE)->Stop();

	ProjectileManager::Instance().Clear();
	for (auto& s : player->safetyAreas)
	{
		s = nullptr;
	}
	player->safetyAreas.clear();

	//boxなどのenemyを継承しているnewはdeleteしてはいけない。EnemyManagerごと消す

	//エネミー終了化

	//EnemyManager::Instance().Clear();

	//エラー起きるかも
	EnemyManager::Instance().Clear();



}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	if (!player || !goalPoint)
		return;

	float dx = player->GetPosition().x - goalPoint->position.x;
	float dy = player->GetPosition().y - goalPoint->position.y;
	float dz = player->GetPosition().z - goalPoint->position.z;

	float distSq = dx * dx + dy * dy + dz * dz;
	float goalRadius = 1.0f;
	GamePadButton button = Input::Instance().GetGamePad().GetButtonDown();
	if (distSq <= goalRadius * goalRadius )
	{
		SpaceBool = true;
		if (button == GamePad::BTN_START)
		{
			while (ShowCursor(TRUE) < 0);
			ShowCursor(true);
			SoundManager::Instance().GetSound(SoundList::walkSE)->Stop();
			passward = true;
			SpaceBool = false;
		}
	}
	else
		SpaceBool = false;

	if (passward && GetAsyncKeyState(VK_BACK) & 0x8000)
	{
		while (ShowCursor(FALSE) >= 0);
		Mouse& mouse = Input::Instance().GetMouse();
		mouse.Lock();
		passward=false;
	}
	if (passward) passwordScene->Update(elapsedTime);
	else
	{
		ShowCursor(false);
		// --- HPチェック ---
		if (player->hp <= 0.0f)
		{
			// SceneResultGameOver に切り替え
			SceneManager::Instance().ChangeScene(new SceneResult(ResultType::GameOver));
			return; // 以降の更新は行わない
		}
		//カメラコントローラー更新処理
		DirectX::XMFLOAT3 target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->Update(elapsedTime);

		//プレイヤー更新処理
		player->Update(elapsedTime);

		if (renderer) {
			std::vector<DirectX::XMFLOAT4> saPositions;
			player->FillSafetyAreaPosition(saPositions);
			renderer->UpdateSafetyAreaLights(saPositions);
		}

		player->SetPosition(physics.CircleVsStage(player->GetPosition(), player->GethitRadius()));

		//enemyslime->SetPosition(physics.CircleVsStage(enemyslime->GetPosition(), enemyslime->GethitRadius()));

		enemyslime->Update(elapsedTime);

		Graphics& graphics = Graphics::Instance();
		ID3D11DeviceContext* dc = graphics.GetDeviceContext();
		ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
		ModelRenderer* modelRenderer = graphics.GetModelRenderer();

		// 描画準備
		RenderContext rc;
		rc.deviceContext = dc;
		//ステージ更新処理
		stage->Update(elapsedTime);

		//エネミー更新処理
		EnemyManager::Instance().Update(elapsedTime);

		//ステージ継続ダメージ
		const float stageDamagePerSec = 2.0f;
		player->AddDamage(stageDamagePerSec * elapsedTime);




		if (!player->safetyAreas.empty())
		{
			//SafetyArea内なら回復
			for (auto& s : player->safetyAreas)
			{
				if (!s) continue;

				if (s) s->Update(elapsedTime);
				if (s && s->IsInside(player->GetPosition()))
				{
					player->Heal(2.0f * elapsedTime);
					break;
				}
			}

		}


	bool inside = false;
	RBool = false;

	for (const auto& d : physics.GetDoorObbs())
	{
		if (physics.IsInside(d, { player->GetPosition().x, player->GetPosition().z }))
		{
			inside = true;
			RBool = true;
			break;
		}
	}

	player->SetPlayerInside(inside);

	
		//シーン遷移
		//GamePad& gamePad = Input::Instance().GetGamePad();
		GamePad& gamePad = Input::Instance().GetGamePad();
		if ((gamePad.GetButtonDown() & GamePad::BTN_START) && player->finish == true)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
		}
	}
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	// 描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f };	// ライト方向（下方向）
	rc.renderState = graphics.GetRenderState();

	//描画処理
	//RenderContext rc;
	rc.renderState = graphics.GetRenderState();
	rc.lightDirection = { 0.0f , -1.0f , 0.0f };	//ライト方向（下方向）

	//カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	// 3Dモデル描画
	{
		//ステージ描画
		stage->Render(rc, modelRenderer);
		
		//player->Render(rc, modelRenderer);

		enemyslime->Render(rc, modelRenderer);

		ProjectileManager::Instance().Render(rc, modelRenderer);

		EnemyManager::Instance().Render(rc, modelRenderer);
	}

	// 3Dデバッグ描画
	{
		//プレイヤーデバッグプリミティブ描画
		//player->RenderDebugPrimitive(rc, shapeRenderer);

		//当たり判定デバッグプリミティブ描画
		//physics.RenderDebugPrimitive(rc, shapeRenderer);

		//エネミーデバッグプリミティブ描画

		

		//stage->RenderDebugPrimitive(rc, shapeRenderer);

		//enemyslime->RenderDebugPrimitive(rc, shapeRenderer);


	}

	// 2Dスプライト描画
	{
		float hpRate = std::clamp((float)player->hp / player->maxHP, 0.0f, 1.0f);

		float texWidth = hpBarTex.GetWidth();
		float texHeight = hpBarTex.GetHeight();

		float SpaceW = sprSpace->GetWidth();
		float SpaceH = sprSpace->GetHeight();

		float RW = sprR->GetWidth();
		float RH = sprR->GetHeight();

		float texW = hpBarFrameTex.GetWidth();
		float texH = hpBarFrameTex.GetHeight();

		// HPバー枠描画
		hpBarFrameTex.Render(
			rc,
			0.0f, 0.0f,
			0,
			texW * 0.5f, texH * 0.5f,
			0, 0,
			texW, texH,
			0,
			1, 1, 1, 1
		);
		
		float drawWidth = texWidth * hpRate;

		// ピクセル指定でOK
		hpBarTex.Render(
			rc,
			0, 0,
			0,
			drawWidth * 0.5f, texHeight * 0.5f,     // 画面上の幅
			0, 0,                     // 切り抜き開始位置
			drawWidth, texHeight,        // 切り抜き高さ（ピクセル）
			0,
			0.5, 0.5, 0.5, 1
		);


		if (SpaceBool)
		{

			sprSpace->Render(
				rc,
				screenWidth*0.7f, screenHeight*0.6f,
				0,
				SpaceW * 0.8f, SpaceH * 0.5f,
				0, 0,
				SpaceW, SpaceH,
				0,
				1, 1, 1, 1
			);
		}

		if(RBool && player->GetCanPlaceSafeArea())
		{
			sprR->Render(
				rc,
				0, 0,
				0,
				RW * 0.7f, RH * 0.7f,
				0, 0,
				RW, RH,
				0,
				1, 1, 1, 1
			);
		}

		int remain = (int)player->GetMaxSafeAreaCount();

		float iconW = SafetyIconTex.GetWidth() * 0.5f;
		float iconH = SafetyIconTex.GetHeight() * 0.5f;

		float startX = screenWidth - iconW - 10.0f;
		float startY = 10.0f;

		for (int i = -1; i < remain; i++)
		{
			float x = startX - i * (iconW + 10);   // 右から左へ積む
			SafetyIconTex.Render(
				rc,
				x, startY,
				0,
				iconW, iconH,
				0, 0,
				SafetyIconTex.GetWidth(), SafetyIconTex.GetHeight(),
				0,
				1, 1, 1, 1
			);
		}

		if(passward)
		{
			passwordScene->Render();
		}

	}

}

// GUI描画
void SceneGame::DrawGUI()
{
	
}