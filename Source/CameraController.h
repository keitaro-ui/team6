#pragma once
#include <DirectXMath.h>

enum class CameraMode
{
	FirstPerson,
	TopDown,
	FreeFly,   
};

//カメラコントローラー
class CameraController
{
public:
	void Initialize();

	//更新処理
	void Update(float elapsedTime);

	//ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }


	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 dir = {};

	const DirectX::XMFLOAT3& getAngle() const{ return angle; };

	void MouseCamera(float elapsedTime);

	//カメラ切り替え
	void ToggleTopDown();

	//一人称視点
	void HandleNormal(float elapsedTime);

	//俯瞰視点
	void HandleTopDown(float elapsedTime);
	
	//カメラ自由飛行
	void HandleFreeCam(float elapsedTime);
	
	//ImGui表示
	void RenderImGui();

	bool CameraController::IsFPSMode() const
	{
		return !isTopDown && !isFreeCam;
	}

	DirectX::XMFLOAT3		target = { 0, 0, 0 }; //注視点
	DirectX::XMFLOAT3		angle = { 0, 0, 0 }; //回転角度
private:
	float					rollSpeed = DirectX::XMConvertToRadians(90); //回転速度
	float					range = 10.0f; //距離
	
	float					maxAngleX = DirectX::XMConvertToRadians(30);
	float					minAngleX = DirectX::XMConvertToRadians(-30);

	float					maxAngleY = DirectX::XMConvertToRadians(45);
	float					minAngleY = DirectX::XMConvertToRadians(-45);

	bool isTopDown = false;
	bool isFreeCam = false;

	DirectX::XMFLOAT3 normalCamEyeOffset{};
	bool wasFreeCamLastFrame = false;

	DirectX::XMFLOAT3 topDownEye{ 0,30,-10 };
	DirectX::XMFLOAT3 topDownTarget{ 0,20,0 };


	struct CameraState
	{
		DirectX::XMFLOAT3 eye;
		DirectX::XMFLOAT3 target;
		DirectX::XMFLOAT3 angle;
	};

	CameraState fpsBackup;
	bool restoringFPS = false;

	float bobTimer = 0.0f;       // 経過時間
	float bobSpeed = 6.0f;       // 歩行速度に応じて増減
	float bobAmountX = 0.020f;   // 左右揺れ幅
	float bobAmountY = 0.020f;    // 上下揺れ幅
};
