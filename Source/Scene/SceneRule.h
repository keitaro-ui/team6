#pragma once
#include "DirectXMath.h"
#include "System/Sprite.h"
#include "Scene.h"
#include "System/Graphics.h"
#include <memory>

class SceneRule : public Scene
{
public:
	SceneRule();
	~SceneRule() override {};

	// ‰Šú‰»
	void Initialize() override;

	// I—¹‰»
	void Finalize() override;

	// XVˆ—
	void Update(float elapsedTime) override;

	// •`‰æˆ—
	void Render() override;

	// GUI•`‰æ
	void DrawGUI() override;

private:
	std::unique_ptr<Sprite> spriteRule;

	float screenWidthRule;
	float screenHeightRule;
};