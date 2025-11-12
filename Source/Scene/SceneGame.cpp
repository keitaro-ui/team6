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

//float game_timer;

// 初期化
void SceneGame::Initialize()
{
	//ステージ初期化
	stage = std::make_unique<Stage>();

	//プレイヤー初期化
	player = std::make_unique<Player>();

	//スプライト初期設定
	{
		
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
	int num = 0;		
	int hei = 0;
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
		{
			/*Board* board_0 = new Board();
			board_0->SetPosition(Board::boardPos[0]);
			board_0->SetAngle(Board::boardAng[0]);
			enemyManager.Instance().Register(board_0);
			boards.push_back(board_0);
			
			Board* board_1 = new Board();
			board_1->SetPosition(Board::boardPos[1]);
			board_1->SetAngle(Board::boardAng[1]);
			enemyManager.Instance().Register(board_1);
			boards.push_back(board_1);

			Board* board_2 = new Board();
			board_2->SetPosition(Board::boardPos[2]);
			board_2->SetAngle(Board::boardAng[2]);
			enemyManager.Instance().Register(board_2);
			boards.push_back(board_2);

			Board* board_3 = new Board();
			board_3->SetPosition(Board::boardPos[3]);
			board_3->SetAngle(Board::boardAng[3]);
			enemyManager.Instance().Register(board_3);
			boards.push_back(board_3);*/
		}
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


	// カメラ更新
	cameraController->Update(elapsedTime);

	//プレイヤー更新処理
	player->Update(elapsedTime);

	//ステージ更新処理
	stage->Update(elapsedTime);

	//エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// プレイヤーがボードに近づいた時
	for (auto& board : boards)
	{
		if (board->CheckPlayerOnBoard(player.get()))
		{
			quizFlag = true;
            board->StartQuiz(); // クイズ開始処理
			break;
		}
	}

	//シーン遷移
	GamePad& gamePad = Input::Instance().GetGamePad();
	const GamePadButton anyButton = GamePad::BTN_START;

	if (gamePad.GetButtonDown() & anyButton&&player->finish==true)
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
		stage->Render(rc, modelRenderer);
		
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
	}

	// 2Dスプライト描画
	{
		
	}
}

// GUI描画
void SceneGame::DrawGUI()
{
	//プレーヤーデバッグ処理
	player->DrawDebugGUI();
}