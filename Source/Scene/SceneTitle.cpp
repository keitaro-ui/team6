#include "System/Graphics.h"
#include "SceneTitle.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "System/Mouse.h"
#include "../Game/Player.h"
#include "../Scene/SceneTutorial.h"
#include "SceneRule.h"



//初期化
void SceneTitle::Initialize()
{
    Graphics& graphics = Graphics::Instance();
    screenWidth = static_cast<float>(graphics.GetScreenWidth());
    screenHeight = static_cast<float>(graphics.GetScreenHeight());

    // 背景スプライト
    background = std::make_unique<Sprite>("Data/Sprite/title_.png");

    // スタートボタン
    UIButton startBtn;
    startBtn.sprite = std::make_unique<Sprite>("Data/Sprite/start_.png");
    startBtn.type = UIButtonType::Start;
    startBtn.Wpos = 0.5f;
    startBtn.Hpos = 0.39f;
    startBtn.spacing = 0.0f;

    // ルールボタン
    UIButton ruleBtn;
    ruleBtn.sprite = std::make_unique<Sprite>("Data/Sprite/rule_.png");
    ruleBtn.type = UIButtonType::Rule;
    ruleBtn.Wpos = 0.5f;
    ruleBtn.Hpos = 0.39f;
    ruleBtn.spacing = 100.0f;


    buttons.push_back(std::move(startBtn));
    buttons.push_back(std::move(ruleBtn));

    ShowCursor(true);

    startTime = std::chrono::steady_clock::now();

}

extern POINT cursorPos;

//終了化
void SceneTitle::Finalize()
{
    ShowCursor(false);
}

//更新処理
void SceneTitle::Update(float elapsedTime)
{
    GetCursorPos(&cursorPos);
    HWND hwnd = GetForegroundWindow();
    ScreenToClient(hwnd, &cursorPos);

    Mouse& mouse = Input::Instance().GetMouse();

    UpdateLayout();

    for (auto& btn : buttons)
        btn.Checkhover(cursorPos.x, cursorPos.y);

    auto now = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;

    updateFlash(elapsedTime);

    if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        for (auto& btn : buttons)
        {
            if (btn.hovered)
            {
                if (btn.type == UIButtonType::Start)
                {
                    SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
                }
                else if (btn.type == UIButtonType::Rule)
                {
                    SceneManager::Instance().ChangeScene(new SceneLoading(new SceneRule));
                }
                return;
            }
        }
    }
}

//描画処理
void SceneTitle::Render()
{
    Graphics& graphics = Graphics::Instance();
    screenWidth = static_cast<float>(graphics.GetScreenWidth());
    screenHeight = static_cast<float>(graphics.GetScreenHeight());

    RenderContext rc;
    rc.deviceContext = graphics.GetDeviceContext();
    rc.renderState = graphics.GetRenderState();

    RenderBackGround(rc);

    auto now = std::chrono::steady_clock::now();
    float elapsedTime = std::chrono::duration<float>(now - startTime).count();

    for (auto& btn : buttons)
    {
        btn.Render(rc, darkAmount,elapsedTime);
    }
}

//GUI描画
void SceneTitle::DrawGUI()
{
}

void SceneTitle::RenderBackGround(RenderContext& rc)
{
    darkAmount = 1.0f;
    if (flashActive)
    {
        darkAmount = 0.2f + static_cast<float>(rand() % 30) / 100.0f;
    }

    if (background)
        background->Render(rc, 0, 0, 0, screenWidth, screenHeight, 0,
            darkAmount, darkAmount, darkAmount, 1.0f);
}

void SceneTitle::UpdateLayout()
{
    for (size_t i = 0; i < buttons.size(); ++i)
    {
        buttons[i].Update(screenWidth, screenHeight, static_cast<int>(i));
    }
}

void SceneTitle::updateFlash(float elapsedTime)
{
    if (!flashActive && (rand() % 1000) < 2)
    {
        flashActive = true;
        flashTimer = flashDuration;
    }

    if (flashActive)
    {
        flashTimer -= elapsedTime;
        if (flashTimer <= 0.0f)
            flashActive = false;
    }
}

