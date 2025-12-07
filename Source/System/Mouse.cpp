#include "System/Mouse.h"
#include "imgui.h"
#include "Graphics.h"

static const int KeyMap[] =
{
	VK_LBUTTON,		// 左ボタン
	VK_MBUTTON,		// 中ボタン
	VK_RBUTTON,		// 右ボタン
};

// コンストラクタ
Mouse::Mouse(HWND hWnd)
	: hWnd(hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	screenWidth = rc.right - rc.left;
	screenHeight = rc.bottom - rc.top;

	//マウスカーソルの非表示
	ShowCursor(FALSE);


}

// 更新
void Mouse::Update()
{
	// スイッチ情報
	MouseButton newButtonState = 0;

	for (int i = 0; i < ARRAYSIZE(KeyMap); ++i)
	{
		if (::GetAsyncKeyState(KeyMap[i]) & 0x8000)
		{
			newButtonState |= (1 << i);
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
	{
		// ImGui がマウスを使っている場合は更新しない
		deltaPositionX = 0;
		deltaPositionY = 0;
		return;
	}

	// ホイール
	wheel[1] = wheel[0];
	wheel[0] = 0;

	// ボタン情報更新
	buttonState[1] = buttonState[0];	// スイッチ履歴
	buttonState[0] = newButtonState;

	buttonDown = ~buttonState[1] & newButtonState;	// 押した瞬間
	buttonUp = ~newButtonState & buttonState[1];	// 離した瞬間

	bool isRightClickDown = (buttonState[0] & (1 << 2)) != 0;   // 現在押されているか
	bool wasRightClickDown = (buttonState[1] & (1 << 2)) != 0; // 前フレームの状態



	// カーソル位置の取得
	POINT cursor;
	::GetCursorPos(&cursor);
	::ScreenToClient(hWnd, &cursor);

	// 画面のサイズを取得する。
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT screenW = rc.right - rc.left;
	UINT screenH = rc.bottom - rc.top;
	//UINT viewportW = screenWidth;
	//UINT viewportH = screenHeight;

	UINT viewportW = static_cast<UINT>(Graphics::Instance().GetScreenWidth());
	UINT viewportH = static_cast<UINT>(Graphics::Instance().GetScreenHeight());

	// 画面補正
	positionX[1] = positionX[0];
	positionY[1] = positionY[0];
	positionX[0] = (LONG)(cursor.x / static_cast<float>(viewportW) * static_cast<float>(screenW));
	positionY[0] = (LONG)(cursor.y / static_cast<float>(viewportH) * static_cast<float>(screenH));

	positionX[0] = cursor.x;  // そのまま
	positionY[0] = cursor.y;


	if (bLock)
	{
		POINT cursor2 = { viewportW / 2.0f, viewportH / 2.0f };
		::ClientToScreen(hWnd, &cursor2);
		::SetCursorPos(cursor2.x, cursor2.y);

		deltaPositionX = (positionX[0] - (viewportW / 2.0f))*0.25f;
		deltaPositionY = (positionY[0] - (viewportH / 2.0f))*0.25f;


		// 中央に戻す
		positionX[0] = viewportW / 2;
		positionY[0] = viewportH / 2;
	}
}

