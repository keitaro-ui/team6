#pragma once

#include "System/Sprite.h"
#include "Scene.h"

#include <chrono>

enum class UIButtonType
{
	Start,
	Rule
};

struct UIButton
{
	std::unique_ptr<Sprite> sprite;
	UIButtonType type = UIButtonType::Start;

	float w = 125.0f;
	float h = 75.0f;

	float Wpos = 0.5f;
	float Hpos = 0.39f;
	float WHscale = 0.251f;
	float Spacescale = 0.14f;

	float spacing = 0.0f; 
	float offsetY = 0;
	float x = 0.0f;
	float y = 0.0f;

	float drawX = 0.0f;
	float drawY = 0.0f;

	float left = 0.0f;
	float top = 0.0f;
	float right = 0.0f;
	float bottom = 0.0f;

	bool hovered = false;

	// ホラー演出用
	float baseAngle = 0.0;  // チケットの傾き
	float shake = 0.0f;

	// 画面サイズに応じて位置とサイズを更新
	void Update(float screenWidth, float screenHeight, int index = 0)
	{
		//オフセットY
		offsetY = screenHeight / 5.5; 
		spacing = screenHeight * Spacescale;

		// ボタン幅
		w = screenWidth * WHscale; 
		// ボタン高さ
		h = screenHeight * WHscale; 

		//描画位置
		drawX = (screenWidth * 0.5f) - (w * Wpos);
		drawY = (screenHeight * 0.5f) - (h * Hpos);

		//UIによるボタン判定の位置計算
		if (type == UIButtonType::Start)
		{
			x = screenWidth * 0.3789f;
			y = screenHeight * 0.6597f;
		}
		else if (type == UIButtonType::Rule)
		{
			x = screenWidth * 0.3789f;
			y = screenHeight * 0.8125f;
		}

		left = x;
		top = y;
		right = x+screenWidth*0.2422f;
		bottom = y+screenHeight*0.0903f;
		
	}

	bool IsHovered(int cursorX, int cursorY) const
	{
		return cursorX >= left && cursorX <= right &&
			cursorY >= top && cursorY <= bottom;
	}

	void Checkhover(int cursorX, int cursorY)
	{
		hovered = cursorX >= left && cursorX <= right &&
			cursorY >= top && cursorY <= bottom;
	}

	void Render(RenderContext& rc,float rgb,float elapsedTime) const
	{
		if (!sprite) return;

		float time = elapsedTime;

		float scale = 1.0f;
		float colorR = 0.0f;
		float angle = baseAngle;
		if (hovered)
		{
			scale = scale = 1.05f + 0.02f * sin(time * 6.0f);
			colorR = colorR = 0.5f + 0.2f * sin(time * 6.0f);
			angle = 100;
		}

		// 描画サイズ（拡大したサイズ）
		float drawW = w * scale;
		float drawH = h * scale;

		// 中心から拡大されるよう X/Y 補正
		float offsetX = (drawW - w) * 0.5f;
		float offsetY_local = (drawH - h) * 0.5f;

		// =====================
		// 描画
		// =====================
		if (type == UIButtonType::Start)
		{
			sprite->Render(
				rc,
				drawX,
				drawY + offsetY,
				0,
				w,
				h,
				DirectX::XMConvertToRadians(angle),
				rgb, rgb, rgb, 1.0f
			);
		}
		if (type == UIButtonType::Rule)
		{
			sprite->Render(
				rc,
				drawX,
				drawY + offsetY + spacing,
				0,
				w,
				h,
				DirectX::XMConvertToRadians(angle),
				rgb, rgb, rgb, 1.0f
			);
		}

	}
};

//タイトルシーン
class SceneTitle :public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {}

	//初期化
	void Initialize() override;

	//終了化
	void Finalize() override;

	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render() override;

	//GUI描画
	void DrawGUI() override;

	void RenderBackGround(RenderContext& rc);

private:
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Sprite> sprite2;
	std::unique_ptr<Sprite> sprite3;
	std::unique_ptr<Sprite> sprite_start;
	std::unique_ptr<Sprite> sprite_rule;

	// 点滅演出用（
	bool flashActive = false;
	float flashTimer = 0.0f;
	float flashDuration = 0.2f;

	std::unique_ptr<Sprite> background;
	std::vector<UIButton> buttons;

	float screenWidth = 0.0f;
	float screenHeight = 0.0f;

	// ボタンサイズ
	float buttonWidth = 125.0f;
	float buttonHeight = 75.0f;

	// 中央配置
	float Wpos = 0.5f;
	float Hpos = 0.39f;
	float spacing = 0.14f;

	// ボタン回転角
	float StartAngle = 0.0f;
	float RuleAngle = 0.0f;
	float renderedAngleS = 0.0f;
	float renderedAngleR = 0.0f;

	// 時間管理
	std::chrono::steady_clock::time_point startTime;
	std::chrono::steady_clock::time_point lastTime;

	float darkAmount = 0;

private:
	void UpdateLayout();
	void updateFlash(float elapsedTime);
};
