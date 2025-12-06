#pragma once

#include <vector>

#include "System/ModelRenderer.h";
#include "Character.h"
#include <DirectXMath.h>
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

#include "WayPoint.h"
#include <vector>
#include <memory>

#define ROW_COUNT 7		//横
#define COLUM_COUNT 6	//縦
//#define MAX_WAY_POINT 200


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

	//デバッグプリミティブ描画
	//void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override;

	//void DebugRenderWayPointConnections(const RenderContext& rc,LineRenderer* lr, ShapeRenderer* renderer);

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

	void AddWayPoint(const DirectX::XMFLOAT3& pos);
	void ConnectWayPoints(int index);
	float Distance2D(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b);
	
	//WayPoint* wayPoint[MAX_WAY_POINT];
private:
	Model* model = nullptr;

	int wayPointCount = 0;

	// これはがウェイポイント配列の本体
	std::vector<std::unique_ptr<WayPoint>> wayPoints;

	DirectX::XMFLOAT3 angleWay = { 0,0,0 };
	DirectX::XMFLOAT3 sizeWay = { 0.5f,0.5f,0.5f };

	std::vector<int> path;   // A* の結果（WayPoint index の列）
	LoadTextures loadTextures;

	std::vector<std::unique_ptr<Model>> models;
	std::vector<LoadTextures> TextureList;
	std::unique_ptr<Model> door;
	
	std::unique_ptr<ModelRenderer> modelRenderer;

	int windex = 0;

	DirectX::XMFLOAT4X4 transformDoor;

	DirectX::XMFLOAT3 doorPos = { 0,0,0 };
	DirectX::XMFLOAT3 doorAngle = { 0,0,0 };

public:
	//エネミー数取
	int GetWayPointCount() const { return static_cast<int>(wayPoints.size()); }

	int GetIndex()const { return windex; }

	//int GetWayPointCount()const { return wayPointCount; }

	//エネミー取得
	WayPoint* GetWayPoint(int index) { 
		return wayPoints.at(index).get(); 
	}
	// 経路を設定
	void SetPath(const std::vector<int>& newPath) { path = newPath; }

	//void WallRayCast(float elapsedTime);

	// 経路を取得（読み取り専用）
	const std::vector<int>& GetPath() const { return path; }
	//エネミー全削除
	void Clear();

	

};


class StageManager
{
private:
	StageManager() {}
	~StageManager() {}

	Stage* currentStage;

public:
	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}

	void Regist(Stage* stage) { currentStage = stage; }

	Stage* GetStage() { return currentStage; }
};