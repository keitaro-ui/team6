#include "System/Graphics.h"
#include "SceneTitle.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "System/Mouse.h"
#include "../Game/Player.h"
#include "../Scene/SceneTutorial.h"

//������
void SceneTitle::Initialize()
{
	//�X�v���C�g������
	sprite = new Sprite("Data/Sprite/title.png");
    sprite2 = new Sprite("Data/Sprite/start.png");
    sprite3 = new Sprite("Data/Sprite/tutorial.png");

    ShowCursor(true);
    //extern int count_1=0,count_2=0,count_3=0,count_4=0
}

extern POINT cursorPos;

//�I����
void SceneTitle::Finalize()
{
	//�X�v���C�g�I����
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}

    ShowCursor(false);
	
}

//�X�V����
void SceneTitle::Update(float elapsedTime)
{
    GetCursorPos(&cursorPos);
    HWND hwnd = GetForegroundWindow();
    ScreenToClient(hwnd, &cursorPos);

    Mouse& mouse = Input::Instance().GetMouse();


    //���N���b�N�ŉ�ʑJ��
    //�X�^�[�g
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        if (cursorPos.x >= 505 && cursorPos.x <= 765)
        {
            if (cursorPos.y >= 520 && cursorPos.y <= 585)
            {
                SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
            }
        }
    }
    //�`���[�g���A��
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        if (cursorPos.x >= 505 && cursorPos.x <= 765)
        {
            if (cursorPos.y >= 600 && cursorPos.y <= 670)
            {
                SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTutorial));
            }
        }
    }
}

//�`�揈��
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//�`�揀��
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	//2D�X�v���C�g�`��
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
        sprite->Render(rc,
            0, 0, 0, screenWidth, screenHeight,
            0,
            1, 1, 1, 1);
            //�X�^�[�g�ƃ`���[�g���A���̕`��Ɗg��
            if (cursorPos.x >= 505 && cursorPos.x <= 765)
            {
                //�X�^�[�g
                if (cursorPos.y >= 520 && cursorPos.y <= 585)
                {
                    //�g��
                    sprite2->Render(rc,
                        60, 25, 0, 1200, 700,
                        0,
                        1, 1, 1, 1);
                }
                else
                {
                    sprite2->Render(rc,
                        150, 100, 0, 1000, 600,
                        0,
                        1, 1, 1, 1);
                }

                //�`���[�g���A��
                if (cursorPos.y >= 600 && cursorPos.y <= 670)
                {
                    //�g��
                    sprite3->Render(rc,
                        60, 20, 0, 1200, 700,
                        0,
                        1, 1, 1, 1);
                }
                else
                {
                    sprite3->Render(rc,
                        150, 100, 0, 1000, 600,
                        0,
                        1, 1, 1, 1);
                }
            }
            else
            {
                //�ʏ펞�̕`��
                sprite2->Render(rc,
                    150, 100, 0, 1000, 600,
                    0,
                    1, 1, 1, 1);

                sprite3->Render(rc,
                    150, 100, 0, 1000, 600,
                    0,
                    1, 1, 1, 1);
            }


	}
}

//GUI�`��
void SceneTitle::DrawGUI()
{

}