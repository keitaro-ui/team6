#pragma once

#include <vector>

#include "System/ModelRenderer.h";
#include "Character.h"
#include "imgui.h"
#include "System/LoadTextures.h"

struct TextureSet
{
	std::string normal;
	std::string roughness;
	std::string metalness;
	std::string emissive;
	std::string occlusion;
};

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

	LoadTextures loadTextures;

	std::vector<std::unique_ptr<Model>> models;
	std::vector<LoadTextures> TextureList;
	
	std::unique_ptr<ModelRenderer> modelRenderer;
};
