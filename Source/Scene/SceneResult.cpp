#include "System/Graphics.h"
#include "SceneResult.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include <imgui.h>
#include "SceneTitle.h"

extern int board_answer, count_1, count_2, count_3, count_4;
int result = -1, point = 0;

//初期化
void SceneResult::Initialize()
{
	//スプライト初期化
	clear_sprite = new Sprite("Data/Sprite/result1.png");
	over_sprite = new Sprite("Data/Sprite/gameover.png");
	sprite_number = new Sprite("Data/Sprite/number.png");

	//point = 0;
	/*count_1 = 0;
	count_2 = 0;
	count_3 = 0;
	count_4 = 0;*/
}

//終了化
void SceneResult::Finalize()
{
	//スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
	if (clear_sprite != nullptr)
	{
		delete clear_sprite;
		clear_sprite = nullptr;
	}
	if (over_sprite != nullptr)
	{
		delete over_sprite;
		over_sprite = nullptr;
	}
	if (sprite_number != nullptr)
	{
		delete sprite_number;
		sprite_number = nullptr;
	}
	ShowCursor(true);

	/*point = 0;
	result = -1;
	count_1 = 0;
	count_2 = 0;
	count_3 = 0;
	count_4 = 0;*/
}

//更新処理
void SceneResult::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//なにかボタンを押したらゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_START;

	answerCheck();

	if (gamePad.GetButtonDown() & anyButton)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
		//SceneManager::Instance().ChangeScene((new SceneGameproject));
	}


}

//描画処理
void SceneResult::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	//2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());

		if (resultType == ResultType::GameClear)
		{
			clear_sprite->Render(rc,0, 0, 0, screenWidth, screenHeight, 0, 1, 1, 1, 1);
		}
		if (resultType == ResultType::GameOver)
		{
			over_sprite->Render(rc, 0, 0, 0, screenWidth, screenHeight, 0, 1, 1, 1, 1);
		}

	}
}

//GUI描画
void SceneResult::DrawGUI()
{
	
}

void SceneResult::answerCheck()
{
}