#include "./System/Graphics.h"
#include "SearchAlgorithm.h"
#include "EnemyManager.h"

SearchAlgorithm::SearchAlgorithm()
{
	for (int i = 0; i < MAX_WAY_POINT; ++i)
	{
		findRoot.emplace_back(-1);
	}
}

SearchAlgorithm::~SearchAlgorithm()
{
	searchEdge.clear();
}


bool SearchAlgorithm::DijkstraSearch(Stage* stage,int StartIndex,int GoalIndex, bool heuristicFlg)
{
	// 過去の探索データをクリア
	SearchClear(stage);

	//エッジ型データのフロンティアツリーを作成
	std::vector<Edge*> frontier;

	//スタートIDをoriginPoint ゴールIDをdistnationPointに持つダミーエッジを作成
	Edge* edge = debug_new Edge();

	//ダミーエッジは接続元と接続先は開始地点にしておく。
	/*edge->destinationPoint = stage->NearWayPointIndex(Start::Instance().GetPosition());
	edge->originPoint = edge->destinationPoint;*/

	edge->destinationPoint = StartIndex;
	edge->originPoint = StartIndex;

	stage->GetWayPoint(StartIndex)->costFromStart = 0.0f;
	
		// コスト0を指定(折り返しでスタート地点に戻らないようにするため)
		StageManager::Instance().GetStage()->GetWayPoint(edge->originPoint)->costFromStart = 0.0f;
		//前準備としてダミーエッジデータをセット
		Edge* nowEdge = edge;

		// DijkstraSseachアルゴリズム
		while (true)
		{
			//サーチしたEdgeの記録(実行結果の表示用）	どこから来たか記憶
			findRoot[nowEdge->destinationPoint] = nowEdge->originPoint;

			//もし、次のnowEdgeのdistnationNodeがゴールだったらtrueでreturnする。
			//if (nowEdge->destinationPoint == stage->NearWayPointIndex((Goal::Instance().GetPosition())))
			if(nowEdge->destinationPoint==GoalIndex)
			{
				delete edge;
				return true;
			}

			//nowEdgeの先のノードを取得する。(今いるノード)
			WayPoint* wayPoint = StageManager::Instance().GetStage()->GetWayPoint(nowEdge->destinationPoint);

			//nowEdgeの先のノードに登録してある4本のエッジをサーチするループ
			//for (int i = 1; i <= wayPoint->GetEdgeNo(); i++) 
			for (int i = 0; i < 4; i++)
			{

				Edge* nextEdge = wayPoint->edge[i];

				// サーチするエッジが有効のとき
				if (nextEdge->destinationPoint >= 0 && nextEdge->destinationPoint < 400) {

					//進み先のノード
					WayPoint* nextPoint = StageManager::Instance().GetStage()->GetWayPoint(nextEdge->destinationPoint);

					// TODO 07_01
					// 進み先のノードまでのコストを計算
					float newCost = wayPoint->costFromStart + nextEdge->cost;

					// TODO 07_02
					// 進み先のコストがまだ計算されていないか、新しいコストの方が低ければ
					// フロンティアツリーに登録
					// (計算されていないときコストには-1.0が入っている)
					if (nextPoint->costFromStart < 0.0f || newCost < nextPoint->costFromStart)
					{
						// コストを更新
						nextPoint->costFromStart = newCost;

						// フロンティア用のエッジを複製（元のnextEdgeを使うと上書きの恐れあり）
						Edge* newEdge = debug_new Edge(*nextEdge);
						frontier.push_back(newEdge);
					}

				}

			}

			nowEdge = searchMinCostEdge(frontier, stage, heuristicFlg);

			if (nowEdge == nullptr)
			{
				delete nowEdge;
				return false;
			}
		

		//何も見つからなければfalse;
			if (nowEdge == nullptr)
				return false;
		}

}

// Dijkstra 探索後に経路を復元して Path を作る関数
std::vector<int> SearchAlgorithm::BuildPath(Stage* stage,int startIndex,int goalIndex)
{
	path.clear();

	// ゴール地点の WayPoint インデックスを取得
	/*int goalIndex = stage->NearWayPointIndex(Goal::Instance().GetPosition());
	int startIndex = stage->NearWayPointIndex(Start::Instance().GetPosition());*/

	// findRoot に情報がなければ空を返す
	/*if (findRoot.find(goalIndex) == findRoot.end())
		return path;*/

	// ゴールからスタートまで逆に辿る
	int current = goalIndex;
	while (current != startIndex)
	{
		path.push_back(current);
		current = findRoot[current];
	}

	// スタートも追加
	path.push_back(startIndex);

	// スタート→ゴールの順に並べ替え
	std::reverse(path.begin(), path.end());

	return path;
}


Edge* SearchAlgorithm::searchMinCostEdge(std::vector<Edge*>& frontier, Stage* stage, bool heuristicFlg)
{
	Edge* answer = nullptr;//答えの入れ物
	int  answerNo = 0;
	float minCost = FLT_MAX;//最小のコストを保持するワーク

	for (int fnrNo = 0; fnrNo < frontier.size(); fnrNo++) {

		Edge* edge = frontier.at(fnrNo);

		// TODO 07_03
		// コスト計算
		// totalCostに接続元の「スタート位置からのコスト」（costFromStart）＋エッジ自体が持つコスト（cost）を入れる
		float totalCost = StageManager::Instance().GetStage()->GetWayPoint(edge->originPoint)->costFromStart + edge->cost;

		// TODO 07_04
		// コスト取り出し
		//接続先の「スタート位置からのコスト」をfrontCostに取り出す(まだ登録されていないなら０となる)
		float frontCost = StageManager::Instance().GetStage()->GetWayPoint(edge->destinationPoint)->costFromStart;

		// TODO 07_05
		// コストの比較
		// frontCostが0(まだ登録されていない)か、frontCostより少ないコストルート(>=totalCost)が
		// 発見されたなら、接続先の「スタート位置からのコスト」（costFromStart）をtotalCostに書き換え。
		// frontCostもtotalCostに更新。
		if (frontCost <= 0.0f || totalCost < frontCost)
		{
			StageManager::Instance().GetStage()->GetWayPoint(edge->destinationPoint)->costFromStart = totalCost;
			frontCost = totalCost;
		}

		if (heuristicFlg)
		{
			// TODO 08_01
			// frontCostに見積コストの加算
			// 見積コストの計算にはheuristicCulc関数を使用しても良い
			// ゴール地点の WayPoint を取得
			WayPoint* goal = StageManager::Instance().GetStage()->GetWayPoint(stage->NearWayPointIndex(Goal::Instance().GetPosition()));

			// 現在処理中のエッジの進み先ノードを取得
			WayPoint* nextPoint = StageManager::Instance().GetStage()->GetWayPoint(edge->destinationPoint);

			// 見積コスト（ヒューリスティック値）を計算して frontCost に加算
			frontCost += heuristicCulc(nextPoint, goal);

		}

		// TODO 07_06
		// エッジの記憶
		// minCostとfrontCostを比較し、frontCostが小さければminCostを更新。
		// 一番小さい接続先の「スタート位置からのコスト」を持つエッジを答え（answer）として記憶。
		// またanswerNoの更新も併せて行うこと
		if (frontCost < minCost)
		{
			minCost = frontCost;
			answer = edge;
			answerNo = fnrNo;
		}

	}

	if (answer == nullptr)return 0;
	searchEdge.push_back(answer);//サーチしたEdgeの記録(灰色ライン)

	frontier.erase(frontier.begin() + answerNo);//答えのエッジはダイクストラのサーチ候補から外す

	//次にFNRに登録するのはこの戻り値に登録されているノードのエッジ群
	return answer;


}

float SearchAlgorithm::heuristicCulc(WayPoint* w1, WayPoint* w2)
{
	// TODO 08_02
	// heuristicCulc関数を使用して見積コストの計算する場合は
	// 関数の内容を完成させること。
	// &w1->positionと&w2->positionの距離を計算
	// 計算した値を戻り値として返す
	DirectX::XMVECTOR start, end;
	start = DirectX::XMLoadFloat3(&w1->position);
	end = DirectX::XMLoadFloat3(&w2->position);

	// 距離を計算（ユークリッド距離）
	DirectX::XMVECTOR distVec = DirectX::XMVectorSubtract(end, start);
	float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(distVec));

	// 計算結果を返す
	return distance;
}

void SearchAlgorithm::SearchRender(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, Stage* stage)
{
	LineRenderer* lineRenderer = Graphics::Instance().GetLineRenderer();

	// サーチしたエッジを描画
	for (const auto edge : searchEdge)
	{
		// 探索したエッジを描画
		lineRenderer->AddVertex(DirectX::XMFLOAT3(StageManager::Instance().GetStage()->GetWayPoint(edge->originPoint)->position.x,
			StageManager::Instance().GetStage()->GetWayPoint(edge->originPoint)->position.y + 2.0f,
			StageManager::Instance().GetStage()->GetWayPoint(edge->originPoint)->position.z),
			DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
		lineRenderer->AddVertex(DirectX::XMFLOAT3(StageManager::Instance().GetStage()->GetWayPoint(edge->destinationPoint)->position.x,
			StageManager::Instance().GetStage()->GetWayPoint(edge->destinationPoint)->position.y + 2.0f,
			StageManager::Instance().GetStage()->GetWayPoint(edge->destinationPoint)->position.z),
			DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	}

	lineRenderer->Render(dc, view, projection);

}

//使わない
void SearchAlgorithm::AddSearchArrow(Stage* stage)
{
	//for (const auto edge : searchEdge)
	//{
	//	if (edge->destinationPoint < 0) continue;
	//	// 探索したエッジを矢印で表示
	//	Arrow06* arrow = new Arrow06();
	//	arrow->SetPosition(stage->wayPoint[edge->originPoint]->position);
	//	arrow->height = edge->cost;
	//	arrow->color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	//	if (arrow->height > 2.0f)
	//	{
	//		arrow->height /= 5.0f;
	//	}
	//	arrow->radius = 0.05f;

	//	// originPointからdestinationPointの角度を計算
	//	DirectX::XMVECTOR origin = DirectX::XMLoadFloat3(&stage->wayPoint[edge->originPoint]->position);
	//	DirectX::XMVECTOR destination = DirectX::XMLoadFloat3(&stage->wayPoint[edge->destinationPoint]->position);
	//	DirectX::XMVECTOR vector = DirectX::XMVectorSubtract(destination, origin);
	//	DirectX::XMFLOAT3 v, out;
	//	DirectX::XMStoreFloat3(&v, vector);
	//	float f = sqrtf(v.x * v.x + v.z * v.z);
	//	out.x = 0.0f;
	//	out.y = atan2f(v.z, -v.x);
	//	out.z = atan2f(f, v.y);

	//	arrow->SetAngle(out);
	//	arrow->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	//	ArrowManager06::Instance().AddArrow(arrow);
	//}
}

void SearchAlgorithm::SearchClear(Stage* stage)
{
	//ArrowManager::Instance().Clear();
	// 探索エッジをクリア
	searchEdge.clear();

	//サーチ済みのFGをリセット
	for (int i = 0; i < MAX_WAY_POINT; i++) {
		StageManager::Instance().GetStage()->GetWayPoint(i)->searchFg = false;
		StageManager::Instance().GetStage()->GetWayPoint(i)->costFromStart = -1.0f;
		findRoot[i] = -1;
	}
}

void SearchAlgorithm::AddAnswerArrow(Stage* stage)
{
	LineRenderer* lineRenderer = Graphics::Instance().GetLineRenderer();

	for (const auto edge : searchEdge)
	{
		if (edge->destinationPoint < 0) continue;

		// originPoint
		DirectX::XMFLOAT3 p0 = StageManager::Instance().GetStage()->GetWayPoint(edge->originPoint)->position;
		// destinationPoint
		DirectX::XMFLOAT3 p1 = StageManager::Instance().GetStage()->GetWayPoint(edge->destinationPoint)->position;

		// 少し浮かせる（任意）
		p0.y += 2.0f;
		p1.y += 2.0f;

		// 線の描画（赤色 or 好きな色）
		lineRenderer->AddVertex(
			p0,
			DirectX::XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f)
		);
		lineRenderer->AddVertex(
			p1,
			DirectX::XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f)
		);
	}
}

void SearchAlgorithm::GoldenPathSpawn(Stage* stage, DirectX::XMFLOAT3 startPosition)
{
	// 表示している敵を消す
	EnemyManager::Instance().Clear();
}

