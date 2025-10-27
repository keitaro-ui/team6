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

	//時間
	game_timer = 30;

	//スプライト初期設定
	sprite = std::make_unique<Sprite>("Data/Sprite/レティクル.png");
	sprite_number = std::make_unique<Sprite>("Data/Sprite/number.png");
	sprite_text = std::make_unique<Sprite>("Data/Sprite/残り時間.png");

	//レティクル関数
	SceneGame::RoadModel();

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
	for (int i = 0; i < 20; i++)
	{
		//EnemySlime* target = new EnemySlime();
		balloon = new Balloon();
		/*target->setobjnum(i);*/

		//Board* board = new Board();
		board = new Board();

		//Box* box = new Box();
		box = new Box();
		/*box->setobjnum(i);*/
		

		//エネミー位置
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float>dist (-4.0f, 4.0f);
		std::uniform_real_distribution<float>dist2(0.5f, 4.0f);
		std::uniform_real_distribution<float>dist3(5.0f, 10.0f);

		balloon->SetPosition(DirectX::XMFLOAT3(dist(gen), dist2(gen), dist3(gen)));
		balloon->SetAngle(DirectX::XMFLOAT3(0, DirectX::XM_PI, 0));
		

		board->SetPosition(DirectX::XMFLOAT3(0, 4.0f, 14));
		board->SetAngle(DirectX::XMFLOAT3(0, DirectX::XM_PI, 0));

		if (i % 5 == 0) { num = 0; hei++; }
		
		box->SetPosition(DirectX::XMFLOAT3(-4.6f+2.3f*num, 1.45f*hei, 12));

		num++;
		
		enemyManager.Register(balloon);
		enemyManager.Register(board);
		enemyManager.Register(box);
		balloon->box = box;
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

	player->Update(elapsedTime);

	//ステージ更新処理
	stage->Update(elapsedTime);

	//プレイヤー更新処理

	//エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	//シーン遷移
	GamePad& gamePad = Input::Instance().GetGamePad();

	const GamePadButton anyButton =
		GamePad::BTN_START;

	game_timer -= elapsedTime;

	//if (gamePad.GetButtonDown() & anyButton)
	if(game_timer < 0)
	{
		//SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
		player->finish = true;
	}

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
		//EnemyManager::Instance().Render(rc, modelRenderer);
		//player->Render(rc, modelRenderer);

		//EnemyManager::Instance().Render(rc, modelRenderer);

		//player->RenderDebugPrimitive(rc, shapeRenderer);
	}

	// 3Dデバッグ描画
	{
		//プレイヤーデバッグプリミティブ描画
		//player->RenderDebugPrimitive(rc, shapeRenderer);

		//エネミーデバッグプリミティブ描画
		//EnemyManager::Instance(); 
		//.RenderDebugPrimitive(rc, shapeRenderer);
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

void SceneGame::RoadModel()
{
	//的のロード
	extern Model* models[4];

	models[0] = new Model("Data/Model/Target/target_1.mdl");
	models[1] = new Model("Data/Model/Target/target_2.mdl");
	models[2] = new Model("Data/Model/Target/target_3.mdl");
	models[3] = new Model("Data/Model/Target/target_4.mdl");

	//問題文のロード
	extern Model* model_boad[21];

	model_boad[0] = new Model("Data/Model/Boad/boad_tutorial.mdl");
	model_boad[1] = new Model("Data/Model/Boad/boad_1.mdl");
	model_boad[2] = new Model("Data/Model/Boad/boad_2.mdl");
	model_boad[3] = new Model("Data/Model/Boad/boad_3.mdl");
	model_boad[4] = new Model("Data/Model/Boad/boad_4.mdl");
	model_boad[5] = new Model("Data/Model/Boad/boad_5.mdl");
	model_boad[6] = new Model("Data/Model/Boad/boad_6.mdl");
	model_boad[7] = new Model("Data/Model/Boad/boad_7.mdl");
	model_boad[8] = new Model("Data/Model/Boad/boad_8.mdl");
	model_boad[9] = new Model("Data/Model/Boad/boad_9.mdl");
	model_boad[10] = new Model("Data/Model/Boad/boad_10.mdl");
	model_boad[11] = new Model("Data/Model/Boad/boad_11.mdl");
	model_boad[12] = new Model("Data/Model/Boad/boad_12.mdl");
	model_boad[13] = new Model("Data/Model/Boad/boad_13.mdl");
	model_boad[14] = new Model("Data/Model/Boad/boad_14.mdl");
	model_boad[15] = new Model("Data/Model/Boad/boad_15.mdl");
	model_boad[16] = new Model("Data/Model/Boad/boad_16.mdl");
	model_boad[17] = new Model("Data/Model/Boad/boad_17.mdl");
	model_boad[18] = new Model("Data/Model/Boad/boad_18.mdl");
	model_boad[19] = new Model("Data/Model/Boad/boad_19.mdl");
	model_boad[20] = new Model("Data/Model/Boad/boad_20.mdl");
}
