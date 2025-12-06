#pragma once

#include "System/Sprite.h"
#include "Scene.h"
#include <vector>

enum class ResultType
{
	GameClear,
	GameOver
};

//タイトルシーン
class SceneResult :public Scene
{
public:
	SceneResult(ResultType type) { resultType = type; }
	~SceneResult() override {}

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

	void answerCheck();

private:
	ResultType resultType;
	Sprite* sprite = nullptr;
	Sprite* clear_sprite;
	Sprite* over_sprite;
	Sprite* sprite_number = nullptr;

	std::vector < std::unique_ptr<Sprite>> sprites_gameover;
	std::vector < std::unique_ptr<Sprite>> sprites_clear;

	float blinkTimer = 0.0f;
	bool  blinkVisible = true;
};
