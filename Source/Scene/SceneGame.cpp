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


// 初期化
void SceneGame::Initialize()
{
	//ステージ初期化
	stage = std::make_unique<Stage>();

	//プレイヤー初期化
	player = std::make_unique<Player>();

	//スプライト初期設定
	{
		spr = std::make_unique<Sprite>("Data/Sprite/LoadingIcon.png");
	}
	//カメラ初期設定
	Graphics& graphics = Graphics::Instance();
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
	/*int num = 0;		
	int hei = 0;*/
	fRenFlag = false;
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
		
		physics.AddObb({ -2, 0, 10 }, { 2, 0, 2 }, 0);
		physics.AddDoorObb({ 2, 0, 2 }, { 2, 0, 2 }, 0, 
			{ 0, 0, 1 }, 3.0f, 0.5f);
	}
	//マウス位置の取得とロック
	Input::Instance().GetMouse().Lock();
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

	//エネミー終了化
	EnemyManager::Instance().Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	//カメラコントローラー更新処理
	if (quizFlag && activeBoard)
	{
		if (!activeBoard->IsQuizActive())
		{
			// クイズ終了処理
			quizFlag = false;
			activeBoard = nullptr;
		}
		else
		{
			// ---- クイズ中カメラ（コントローラを使わない） ----
			DirectX::XMFLOAT3 pos = activeBoard->GetPosition();
			DirectX::XMFLOAT3 ang = activeBoard->GetAngle();

			float frontDist = -3.0f;
			float height = 1.5f;

			float rad = ang.y;
			float fx = sinf(rad);
			float fz = cosf(rad);

			DirectX::XMFLOAT3 eye = {
				pos.x - fx * frontDist,
				pos.y + height,
				pos.z - fz * frontDist
			};

			DirectX::XMFLOAT3 tgt = {
				pos.x,
				pos.y + 1.0f,
				pos.z
			};

			Camera::Instance().SetLookAt(eye, tgt, { 0,1,0 });
		}
	}
	else
	{
		// 通常カメラ
		DirectX::XMFLOAT3 target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->Update(elapsedTime);
	}
	
	//プレイヤー更新処理
	player->Update(elapsedTime);
	player->SetPosition(physics.CircleVsStage(player->GetPosition(), 1));

	//ステージ更新処理
	stage->Update(elapsedTime);

	//エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	//当たり判定更新処理
	physics.Update();

	//クイズ処理
	if (!quizFlag)   // ←クイズ中は検知しない
	{
		for (auto& board : boards)
		{
			//quizFlag = false;
			if (board->CheckNearBoard(player.get()))
			{
				fRenFlag = true;
				auto& gamepad = Input::Instance().GetGamePad();
				//if (gamepad.GetButtonDown() & GamePad::BTN_A)
				if (GetAsyncKeyState('F') & 0x8000)
				{
					quizFlag = true;
					activeBoard = board;
					board->StartQuiz();
					break;
				}
			}
		}
	}

	if (quizFlag && activeBoard && GetAsyncKeyState('F') & 0x8000)
	{
		activeBoard->EndQuiz();
		quizFlag = false;
		fRenFlag = false;
		activeBoard = nullptr;
	}

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

	// 3Dモデル描画
	{
		//ステージ描画
		//stage->Render(rc, modelRenderer);
		
		//player->Render(rc, modelRenderer);

		ProjectileManager::Instance().Render(rc, modelRenderer);

		EnemyManager::Instance().Render(rc, modelRenderer);
	}

	// 3Dデバッグ描画
	{
		//プレイヤーデバッグプリミティブ描画
		//player->RenderDebugPrimitive(rc, shapeRenderer);

		//エネミーデバッグプリミティブ描画
		//EnemyManager::Instance(); 
		//.RenderDebugPrimitive(rc, shapeRenderer);

		player->RenderDebugPrimitive(rc, shapeRenderer);
		physics.RenderDebugPrimitive(rc, shapeRenderer);
	}

	// 2Dスプライト描画
	{
		if (fRenFlag)
		{
			/*spr->Render(rc, 
				)*/
		}
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

	ImGui::End();
}