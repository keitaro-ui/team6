#pragma once
#include <vector>

#include"Stage.h"
#include"Edge.h"
#include"WayPoint.h"

#include"Object.h"
#include"System/LineRenderer.h"

#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

class SearchAlgorithm
{
private:
	SearchAlgorithm();
	~SearchAlgorithm();
	std::vector<int> path;
public:
	static SearchAlgorithm& Instance()
	{
		static SearchAlgorithm instance;
		return instance;
	}

	// DijkstraSearch(ダイクストラサーチ)
	bool DijkstraSearch(Stage* stage,int StartIndex,int GoalIndex, bool heuristicFlg = false);
	std::vector<int> BuildPath(Stage* stage,int startIndex,int goalIndex);
	// Dijkstraで使用するコスト計算関数
	Edge* searchMinCostEdge(std::vector<Edge*>& FNR, Stage* stage, bool heuristicFlg);
	// A*で使用する見積コスト計算関数
	float heuristicCulc(WayPoint* N1, WayPoint* N2);

	std::vector<int>& GetCurrentPath() { return path; }

	//void SetCurrentPath(const std::vector<int>& path) { path = path; }

	//探索したエッジをを記憶するワーク
	std::vector<Edge*> searchEdge;

	//答えルートのエッジをを記憶するワーク
	std::vector<int> findRoot;
	// 探索したデータを表示
	void SearchRender(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, Stage* stage);
	void AddSearchArrow(Stage* stage);
	void SearchClear(Stage* stage);
	void AddAnswerArrow(Stage* stage);
	void GoldenPathSpawn(Stage* stage, DirectX::XMFLOAT3 startPosition);

};