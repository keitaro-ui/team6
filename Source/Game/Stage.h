#pragma once

#include "System/ModelRenderer.h";
#include "Character.h"
#include "imgui.h"

//ステージ
class Stage : public Character
{
public:
	Stage();
	~Stage();

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//ImGui表示
	void RenderImGui();

private:
	Model* model = nullptr;
};
