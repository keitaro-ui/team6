#include "SceneRule.h"
#include "SceneGame.h"
#include "System//Input.h"
#include "Scene//SceneManager.h"
#include "Scene//SceneLoading.h"


SceneRule::SceneRule()
{
	spriteRule = std::make_unique<Sprite>("Data/Sprite/rule.png");
}

void SceneRule::Initialize()
{
}

void SceneRule::Finalize()
{
}

void SceneRule::Update(float elapsedTime)
{
    Input& input = Input::Instance();

    // Enter（Return）が押されたらシーン切り替え
    if (GetAsyncKeyState(VK_RETURN) & 1)   // Enterキー
    {
        SoundManager::Instance().GetSound(SoundList::startSE)->Play(false, 1.0f);
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
        return;
    }
}

void SceneRule::Render()
{
    Graphics& graphics = Graphics::Instance();
    screenWidthRule = static_cast<float>(graphics.GetScreenWidth());
    screenHeightRule = static_cast<float>(graphics.GetScreenHeight());

    RenderContext rc;
    rc.deviceContext = graphics.GetDeviceContext();
    rc.renderState = graphics.GetRenderState();

    spriteRule->Render(rc, 0, 0, 0, screenWidthRule, screenHeightRule, 0, 1, 1, 1, 1);
}

void SceneRule::DrawGUI()
{
}
