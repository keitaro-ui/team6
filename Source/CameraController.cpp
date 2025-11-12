#include"System/Input.h"
#include"CameraController.h"
#include"Camera.h"
#include<System/Graphics.h>
#include "imgui.h"
#include <algorithm>

void CameraController::Initialize()
{

}

//更新処理
void CameraController::Update(float elapsedTime)
{
	
	Mouse& mouse = Input::Instance().GetMouse();

	static bool cKeyLast = false;
	// Cキーでカメラモード切替
	bool cKeyNow = (GetAsyncKeyState('C') & 0x8000) != 0;
	if (cKeyNow && !cKeyLast)
	{
		if (!isTopDown && !isFreeCam)
		{
			// FPS → 俯瞰に切り替える前にFPS情報を保存
			fpsBackup.eye = eye;
			fpsBackup.target = target;
			fpsBackup.angle = angle;

			isTopDown = true;
		}
		else if (isTopDown)
		{
			// 俯瞰 → 自由カメラに切り替え
			isTopDown = false;
			isFreeCam = true;
		}
		else if (isFreeCam)
		{
			// 自由カメラ → FPSに戻す
			isFreeCam = false;
			restoringFPS = true;
		}
	}
	cKeyLast = cKeyNow;

	// マウス入力による角度更新
	float movePower = 0.01f;
	angle.x -= mouse.GetDeltaPositionY() * movePower * 0.2f;
	angle.y += mouse.GetDeltaPositionX() * movePower * 0.2f;

	// カメラモードごとの処理
	if (isTopDown) //俯瞰カメラ
	{
		HandleTopDown(elapsedTime);
	}
	else if (isFreeCam) //自由カメラ
	{
		HandleFreeCam(elapsedTime);
	}
	else // FPSカメラ
	{
		// FPS復帰時の位置・角度復元
		if (restoringFPS)
		{
			eye = fpsBackup.eye;
			target = fpsBackup.target;
			angle = fpsBackup.angle;
			restoringFPS = false;
		}
		HandleNormal(elapsedTime);
	}

	// ImGuiによるデバッグ表示
	RenderImGui();

	// マウス状態更新
	mouse.Update();
}


void CameraController::MouseCamera(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	Mouse& mouse = Input::Instance().GetMouse();

	// IMGUIのマウス入力値を使ってカメラ操作する
	ImGuiIO io = ImGui::GetIO();

	// マウスカーソルの移動量を求める
	float moveX = io.MouseDelta.x * 0.02f;
	float moveY = io.MouseDelta.y * 0.02f;

}

void CameraController::ToggleTopDown()
{
	isTopDown = !isTopDown;

	if (isTopDown)
	{
		// 俯瞰カメラ位置に切替
		eye = topDownEye;
		target = topDownTarget;
	}
}

void CameraController::HandleNormal(float elapsedTime)
{
	// マウス回転で角度更新
	Mouse& mouse = Input::Instance().GetMouse();
	float mouseDeltaX = mouse.GetDeltaPositionX();
	float mouseDeltaY = mouse.GetDeltaPositionY();
	float movePower = 0.001f;

	angle.x -= mouseDeltaY * movePower * 0.2f;
	angle.y += mouseDeltaX * movePower * 0.2f;

	// X軸の制限
	angle.x = std::clamp(angle.x, minAngleX, maxAngleX);

	//カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	//回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	eye = target; 

	// target は前方向ベクトルを足す
	target.x = (eye.x + front.x);
	target.y = (eye.y - front.y);
	target.z = (eye.z + front.z);

	//向いている方向のベクトルを計算で出す
	dir.x = target.x - eye.x;
	dir.y = target.y - eye.y;
	dir.z = target.z - eye.z;
	float len = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
	dir.x /= len;
	dir.y /= len;
	dir.z /= len;

	bool isWalking = (GetAsyncKeyState('W') & 0x8000) ||
		(GetAsyncKeyState('S') & 0x8000) ||
		(GetAsyncKeyState('A') & 0x8000) ||
		(GetAsyncKeyState('D') & 0x8000);

	// Bobタイマー更新
	if (isWalking)
	{
		bobTimer += elapsedTime * bobSpeed;
	}
	else
	{
		bobTimer = 0.0f;
	}

	// Bobオフセット計算
	float bobOffsetY = sinf(bobTimer) * bobAmountY;
	float bobOffsetX = cosf(bobTimer * 0.5f) * bobAmountX;

	// カメラ位置に反映
	DirectX::XMFLOAT3 bobbedEye = eye;
	bobbedEye.x += bobOffsetX;
	bobbedEye.y += bobOffsetY;

	//カメラの視点と注視点を設定
	Camera::Instance().SetLookAt(bobbedEye, target, DirectX::XMFLOAT3(0, 1, 0));
}

void CameraController::HandleTopDown(float elapsedTime)
{
	// 俯瞰回転の制限
	float minTopDownX = -DirectX::XM_PIDIV2 + 0.1f;
	float maxTopDownX = -0.1f;
	angle.x = std::clamp(angle.x, minTopDownX, maxTopDownX);

	// 回転行列から前方向ベクトル
	DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMVECTOR front = transform.r[2];
	DirectX::XMFLOAT3 f;
	DirectX::XMStoreFloat3(&f, front);

	// カメラ位置 = 注視点(topDownTarget)から前ベクトル方向に距離 range
	eye.x = topDownTarget.x - f.x * range;
	eye.y = topDownTarget.y + f.y * range;
	eye.z = topDownTarget.z - f.z * range;

	target = topDownTarget;

	// カメラセット
	Camera::Instance().SetLookAt(eye, target, { 0,1,0 });
}

void CameraController::HandleFreeCam(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	Mouse& mouse = Input::Instance().GetMouse();

	// マウスで回転
	float movePower = 0.002f;
	float dx = mouse.GetDeltaPositionX();
	float dy = mouse.GetDeltaPositionY();

	angle.y += dx * movePower;
	angle.x -= dy * movePower;

	// X軸回転制限（上下向きすぎ防止）
	angle.x = std::clamp(angle.x, -DirectX::XM_PIDIV2 + 0.01f, DirectX::XM_PIDIV2 - 0.01f);

	// 回転行列から前方向・右方向ベクトルを取得
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, 0.0f);
	DirectX::XMVECTOR forward = DirectX::XMVector3Normalize(rotMat.r[2]); // 視線方向
	// 水平方向の右ベクトル
	DirectX::XMVECTOR right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross({ 0,-1,0 }, forward));

	DirectX::XMFLOAT3 f, r;
	DirectX::XMStoreFloat3(&f, forward);
	DirectX::XMStoreFloat3(&r, right);

	// 移動速度
	float speed = 10.0f * elapsedTime;

	// WASD移動
	if (GetAsyncKeyState('W') & 0x8000) {
		eye.x += -f.x * speed;
		eye.y += -f.y * speed;
		eye.z += -f.z * speed;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		eye.x -= -f.x * speed;
		eye.y -= -f.y * speed;
		eye.z -= -f.z * speed;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		eye.x -= r.x * speed;
		eye.y -= r.y * speed;
		eye.z -= r.z * speed;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		eye.x += r.x * speed;
		eye.y += r.y * speed;
		eye.z += r.z * speed;
	}

	// Q/Eで上下移動
	if (GetAsyncKeyState('Q') & 0x8000) eye.y -= speed;
	if (GetAsyncKeyState('E') & 0x8000) eye.y += speed;

	// 注視点は前方方向に固定（視線ベクトル）
	target.x = eye.x - f.x;
	target.y = eye.y - f.y;
	target.z = eye.z - f.z;

	Camera::Instance().SetLookAt(eye, target, { 0,1,0 });

	mouse.Update();
}

void CameraController::RenderImGui()
{
	if (ImGui::Begin("Camera Controller"))
	{
		ImGui::Text("Eye: %.2f, %.2f, %.2f", eye.x, eye.y, eye.z);
		ImGui::Text("Target: %.2f, %.2f, %.2f", target.x, target.y, target.z);

		ImGui::Separator();

		// 各値の調整
		ImGui::DragFloat3("Angle (rad)", &angle.x, 0.01f, -DirectX::XM_PI, DirectX::XM_PI);
		ImGui::DragFloat("Bob Amount X", &bobAmountX, 0.01f, 0.0f,0.05f);
		ImGui::DragFloat("Bob Amount Y", &bobAmountY, 0.01f, 0.0f,0.05f);

	}
	ImGui::End();
}

