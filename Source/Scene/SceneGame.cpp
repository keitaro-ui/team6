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


// 初期化
void SceneGame::Initialize()
{
	//ステージ初期化
	stage = std::make_unique<Stage>();

	//プレイヤー初期化
	player = std::make_unique<Player>();
	sprites.push_back(std::make_unique<Sprite>("Data/Sprite/Oxygen_gauge_frame.png"));
	sprites.push_back(std::make_unique<Sprite>("Data/Sprite/Oxygen_gauge.png"));
	//PlayerManager::Instance().Register(player.get());

	Graphics& graphics = Graphics::Instance();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();
	player->SetRenderer(modelRenderer);

	renderer = modelRenderer;

	PlayerManager::Instance().Register(player.get());

	enemyslime = std::make_unique<EnemySlime>();
	//enemyslime->player = player.get();

	//スタート位置初期化


	//ゴール位置初期化
	goalPoint = new GoalPoint({ 0.0f, 0.0f, 16.0f });


	//スプライト初期設定
	{
		spr = std::make_unique<Sprite>("Data/Sprite/LoadingIcon.png");
		Graphics& graphics = Graphics::Instance();
		//HPバー読み込み
		hpBarTex = Sprite("Data/Sprite/hp_bar.png");

	}
	//カメラ初期設定
	//Graphics& graphics = Graphics::Instance();
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
	//マウス位置の取得とロック
	Input::Instance().GetMouse().Lock();

	//BGM
	SoundManager::Instance().GetSound(SoundList::gameBGM)->Play(true, 0.5f);
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

	delete balloon;

	SoundManager::Instance().GetSound(SoundList::gameBGM)->Stop();

	//boxなどのenemyを継承しているnewはdeleteしてはいけない。EnemyManagerごと消す

	//エネミー終了化
	EnemyManager::Instance().Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
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


	enemyslime->Update(elapsedTime);

	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	// 描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	renderer->RenderImGui(rc);
	//ステージ更新処理
	stage->Update(elapsedTime);

	//エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	//ステージ継続ダメージ
	const float stageDamagePerSec = 5.0f;
	player->AddDamage(stageDamagePerSec * elapsedTime);

	// --- HPチェック ---
	if (player->hp == 0.0f)
	{
		// SceneResult に切り替え
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneResult()));
		return; // 以降の更新は行わない
	}

	if (!player->safetyAreas.empty())
	{
		//SafetyArea内なら回復
		for (auto& s : player->safetyAreas)
		{
			if (!s) continue;

			if (s) s->Update(elapsedTime);
			if (s && s->IsInside(player->GetPosition()))
			{
				player->Heal(5.0f * elapsedTime);
				break;
			}
		}
	}

	bool inside = false;

	for (const auto& d : physics.GetDoorObbs())
	{
		if (physics.IsInside(d, { player->GetPosition().x, player->GetPosition().z }))
		{
			inside = true;
			break;
		}
	}

	player->SetPlayerInside(inside);

	//クイズ処理
	//if (!quizFlag)   // ←クイズ中は検知しない
	//{
	//	for (auto& board : boards)
	//	{
	//		//quizFlag = false;
	//		if (board->CheckNearBoard(player.get()))
	//		{
	//			fRenFlag = true;
	//			auto& gamepad = Input::Instance().GetGamePad();
	//			//if (gamepad.GetButtonDown() & GamePad::BTN_A)
	//			if (GetAsyncKeyState('F') & 0x8000)
	//			{
	//				quizFlag = true;
	//				activeBoard = board;
	//				board->StartQuiz();
	//				break;
	//			}
	//		}
	//	}
	//}

	//if (quizFlag && activeBoard && GetAsyncKeyState('F') & 0x8000)
	//{
	//	activeBoard->EndQuiz();
	//	quizFlag = false;
	//	fRenFlag = false;
	//	activeBoard = nullptr;
	//}

	//シーン遷移
	//GamePad& gamePad = Input::Instance().GetGamePad();
	GamePad& gamePad = Input::Instance().GetGamePad();
	if ((gamePad.GetButtonDown() & GamePad::BTN_START) && player->finish == true)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
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

		//enemyslime->Render(rc, modelRenderer);

		ProjectileManager::Instance().Render(rc, modelRenderer);

		EnemyManager::Instance().Render(rc, modelRenderer);
	}

	// 3Dデバッグ描画
	{
		//プレイヤーデバッグプリミティブ描画
		player->RenderDebugPrimitive(rc, shapeRenderer);

		//当たり判定デバッグプリミティブ描画
		physics.RenderDebugPrimitive(rc, shapeRenderer);

		//エネミーデバッグプリミティブ描画
		enemyslime->RenderDebugPrimitive(rc, shapeRenderer);

		//modelRenderer->RenderImGui(rc);

	}

	// 2Dスプライト描画
	{


		for (size_t i = 0; i < sprites.size(); i++)
		{
			sprites[i]->Render(rc,
				0, 0, 0,
				screenWidth, screenHeight,
				0,
				1, 1, 1, 1);
		}

		float hpRate = std::clamp((float)player->hp / player->maxHP, 0.0f, 1.0f);

		float texWidth = hpBarTex.GetWidth();
		float texHeight = hpBarTex.GetHeight();

		float drawWidth = texWidth * hpRate;

		// ピクセル指定でOK
		hpBarTex.Render(
			rc,
			0, 0,
			0,
			drawWidth, texHeight,     // 画面上の幅
			0, 0,                     // 切り抜き開始位置
			drawWidth, texHeight,        // 切り抜き高さ（ピクセル）
			0,
			1, 1, 1, 1
		);

	}

}

// GUI描画
void SceneGame::DrawGUI()
{
	//プレーヤーデバッグ処理
	player->DrawDebugGUI();

	//クイズ関連のデバッグ
	ImGui::Begin("Boards Debug");

	if (ImGui::BeginTabBar("Board Tabs"))
	{
		for (int i = 0; i < 4; i++)
		{
			if (boards[i])
			{
				std::string tabName = "Board " + std::to_string(boards[i]->GetQuizNum());
				if (ImGui::BeginTabItem(tabName.c_str()))
				{
					// ← Board.cpp の GUI 値編集部分に任せる
					boards[i]->DrawGUIValues();

					ImGui::EndTabItem();
				}
			}
		}
		ImGui::EndTabBar();
	}

	//ステージデバッグ描画
	stage->RenderImGui();
	
	ImGui::End();
}