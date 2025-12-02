#pragma once

#include <vector>

#include "System/ModelRenderer.h";
#include "Character.h"
#include "imgui.h"
<<<<<<< HEAD
#include "System/LoadTextures.h"

struct TextureSet
{
	std::string normal;
	std::string roughness;
	std::string metalness;
	std::string emissive;
	std::string occlusion;
};
=======
#include "WayPoint.h"

#define ROW_COUNT 20
#define COLUM_COUNT 20
#define MAX_WAY_POINT 400
>>>>>>> 6e252551dfff2d12555e88397ab1e007b2b71cc1

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

	// ウェイポイントのインデックスからポジションを取得
	DirectX::XMFLOAT3 GetIndexWayPoint(int index);
	// 指定座標から一番近いウェイポイントのインデックスを取得
	int NearWayPointIndex(DirectX::XMFLOAT3 target);
	// 接続先ポイントを設定
	void Stage::DestinationPointSet(int index);
	bool edgeAppear = false;
	bool wayPointAppear = false;
	bool stageLineAppear = true;
	bool searchEdgeAppear = false;


	WayPoint* wayPoint[MAX_WAY_POINT];
private:
	Model* model = nullptr;

	LoadTextures loadTextures;

	std::vector<std::unique_ptr<Model>> models;
	std::vector<LoadTextures> TextureList;
	
	std::unique_ptr<ModelRenderer> modelRenderer;
};
