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

//������
void SceneResult::Initialize()
{
	//�X�v���C�g������
	sprite = new Sprite("Data/Sprite/result.png");
	sprite_number = new Sprite("Data/Sprite/number.png");

	//point = 0;
	/*count_1 = 0;
	count_2 = 0;
	count_3 = 0;
	count_4 = 0;*/
}

//�I����
void SceneResult::Finalize()
{
	//�X�v���C�g�I����
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
	ShowCursor(true);

	/*point = 0;
	result = -1;
	count_1 = 0;
	count_2 = 0;
	count_3 = 0;
	count_4 = 0;*/
}

//�X�V����
void SceneResult::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//�Ȃɂ��{�^������������Q�[���V�[���֐؂�ւ�
	const GamePadButton anyButton =
		GamePad::BTN_START;

	answerCheck();

	if (gamePad.GetButtonDown() & anyButton)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
		//SceneManager::Instance().ChangeScene((new SceneGameproject));
	}


}

//�`�揈��
void SceneResult::Render()
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


		/*
	void Sprite::Render(const RenderContext& rc,
	float dx, float dy,					// ����ʒu
	float dz,							// ���s
	float dw, float dh,					// ���A����
	float sx, float sy,					// �摜�؂蔲���ʒu
	float sw, float sh,					// �摜�؂蔲���T�C�Y
	float angle,						// �p�x
	float r, float g, float b, float a	// �F
	) const
		*/

		int n[2]{};
		//n[0] = point / 100 % 10;
		n[0] = point / 10 % 10;
		n[1] = point % 10;
		if (point <= 0)
		{
			for (int i = 0; i < 2; i++)
			{
				sprite_number->Render(rc,
					32 * 2 * i + 570, 400,
					0,
					32 * 2, 32 * 2,
					372.5 * n[0], 0,
					372.5, 514,
					0,
					1, 1, 1, 1);
			}
		}
		else
		{
			for (int i = 0; i < 2; i++)
			{
				sprite_number->Render(rc,
					32 * 2 * i + 570, 400,
					0,
					32 * 2, 32 * 2,
					372.5 * n[i], 0,
					372.5, 514,
					0,
					1, 1, 1, 1);
			}
		}
	}
}

//GUI�`��
void SceneResult::DrawGUI()
{
	/*ImGui::Text("result = %d", result+1);
	ImGui::Text("point = %d", point);
	ImGui::Text("boad_answer = &d", board_answer);*/
}

void SceneResult::answerCheck()
{
	if (board_answer == 1 || board_answer == 4 || board_answer == 5 || board_answer == 20)
		result = 0;

	 if (board_answer == 2 || board_answer == 8 || board_answer == 10 || board_answer == 12 ||
	board_answer == 15 || board_answer == 17 || board_answer == 19)
	result = 1;

	 if (board_answer == 9 || board_answer == 11 || board_answer == 14 || board_answer == 18)
	result = 2;

	 if (board_answer == 3 || board_answer == 6 || board_answer == 7 || board_answer == 13 ||
		board_answer == 16)
		result = 3;

	//�ŏI�|�C���g
	if (result == 0)
	{
		point = count_1 * 2 - count_2 - count_3 - count_4;
	}
	else if (result == 1)
	{
		point = count_2 * 2 - count_1 - count_3 - count_4;
	}
	else if (result == 2)
	{
		point = count_3 * 2 - count_2 - count_1 - count_4;
	}
	else if (result == 3)
	{
		point = count_4 * 2 - count_2 - count_3 - count_1;
	}

	board_answer = -1;

}