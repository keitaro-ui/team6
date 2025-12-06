#include "ScenePassword.h"
#include "SceneManager.h"
#include "SceneResult.h"
#include"System/Input.h"
#include "SceneLoading.h"
#include <imgui.h>
#include <cstring>
#include "../Source/Scene/SceneGame.h"
#include "../Source/System/Sprite.h"
#include "System/Mouse.h"

// パスワードの最大長（必要に応じて調整）
constexpr int PASSWORD_BUFFER_SIZE = 4;

ScenePassword::ScenePassword(const std::string& correctPass)
    : correctPassword(correctPass)
{

    // char配列バッファで初期化
    char inputPassword[32];

	Password = std::make_unique<Sprite>("Data/Sprite/number.png");
	PassBackGround = std::make_unique<Sprite>("Data/Sprite/passBackground.png");
	Triangle = std::make_unique<Sprite>("Data/Sprite/BLACK_TRIANGLE.png");
	OK = std::make_unique<Sprite>("Data/Sprite/OK.png");

}

ScenePassword::~ScenePassword()
{

}

void ScenePassword::Initialize()
{
    inputPassword = "";
    wrong = false;
}



void ScenePassword::Update(float elapsedTime)
{
    const GamePadButton ESCButton = GamePad::BTN_A;
    Mouse& mouse = Input::Instance().GetMouse();
    ShowCursor(true);
    mouse.Unlock();


    if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
		OnMouseDown(static_cast<float>(mouse.GetPositionX()), static_cast<float>(mouse.GetPositionY()));
        
    }

    const GamePadButton anyButton = GamePad::BTN_START;
    if (anyButton)
    {
        // SceneGame に戻す
       //SceneManager::Instance().ChangeScene(new SceneGame);
    }



   
    // Spaceキーで決定
    if (anyButton)
    {
        if (std::string(inputPassword) == correctPassword)
        {
            if (!transitionDone)
            {
                transitionDone = true;
            }
        }
        else
        {
            wrong = true;
        }
    }
}

void ScenePassword::Render()
{
    Graphics& graphics = Graphics::Instance();
    screenWidth = static_cast<float>(graphics.GetScreenWidth());
    screenHeight = static_cast<float>(graphics.GetScreenHeight());

    RenderContext rc;
    rc.deviceContext = graphics.GetDeviceContext();
    rc.renderState = graphics.GetRenderState();

   
    float screenW = screenWidth;
    float screenH = screenHeight;
    PassBackGround->Render(
        rc,
        0,
        0,
        0,
        screenW,
        screenH,
        0,
        1, 1, 1, 1
    );

    float scale = 0.5f;
    float spacing = -10.0f;

    // ▼ここで切り抜き幅を狭くする
    int trim = 10; // ← この値を増やすほど数字が細くなる
    int croppedWidth = digitWidth - trim * 2;

    float drawWidth = croppedWidth * scale;
    float drawHeight = digitHeight * scale;

    // 全体幅計算
    float totalWidth = drawWidth * 4 + spacing * 3;
    float startX = (screenW - totalWidth) / 2.0f;
    float yPos = screenH * 0.25f;

    for (int i = 0; i < 4; i++)
    {
        int num = (int)clickNum[i];   // 0?9

        float x = startX + i * (drawWidth + spacing);
		//数字描画
        Password->Render(
            rc,
            x,
            yPos,
            0,
            drawWidth, drawHeight,
            num * digitWidth + trim, 0,
            croppedWidth, digitHeight,

            0,
            1, 1, 1, 1
        );


        float triWidth = 71.0f;   // Triangle の幅
        float triHeight = 75.0f;   // Triangle の高さ（必要なら調整）
        float triY = yPos - triHeight - 10.0f;  // ← 数字より上にオフセット

		// 三角形描画
        Triangle->Render(
            rc,
            x + 47.0f,          // ← 数字と同じXに配置！
            triY,       // ← 数字より上に配置
            0,
            triWidth,
            triHeight,
           0, 0,
            triWidth, triHeight,
            -90,
            1, 1, 1, 1
        );
        tri[i].x = x + 47.0f;
        tri[i].y = triY;
        tri[i].w = triWidth;
        tri[i].h = triHeight;

    }


    float okWidth = 200.0f;
    float okHeight = 100.0f;
    float okX = (screenW - okWidth) / 2.0f;
    float okY = yPos + drawHeight + 15.0f;

        OK->Render(
            rc,
            (screenW - 100.0f) / 2.0f,
            yPos + drawHeight + 15.0f,
            0,
            200.0f,
            100.0f,
            0, 0,
            200.0f,
            100.0f,
            0,
            1, 1, 1, 1
		);
        okButtonRect.x = okX;
        okButtonRect.y = okY;
        okButtonRect.w = okWidth;
        okButtonRect.h = okHeight;
}

void ScenePassword::OnMouseDown(float mx, float my)
{
    
    // Triangleクリック判定
    for (int i = 0; i < 4; i++)
    {
        if (mx >= tri[i].x && mx <= tri[i].x + tri[i].w &&
            my >= tri[i].y && my <= tri[i].y + tri[i].h)
        {
            clickNum[i]++;
            if (clickNum[i] > 4) clickNum[i] = 0;

            // 正解チェック
            if (clickNum[0] == 3 && clickNum[1] == 1 &&
                clickNum[2] == 3 && clickNum[3] == 2)
                isAnswerReady = true;
            else
                isAnswerReady = false;

            return;
        }

    }


	// OKボタンクリック判定
    if (mx >= okButtonRect.x && mx <= okButtonRect.x + okButtonRect.w &&
        my >= okButtonRect.y && my <= okButtonRect.y + okButtonRect.h)
    {
        OnClickOKButton();
    }

}

void ScenePassword::OnClickOKButton()
{
    if (isAnswerReady)
    {
        SceneManager::Instance().ChangeScene(
            new SceneLoading(
                new SceneResult(ResultType::GameClear)
            )
        );
    }
    else
    {
        
    }
}
