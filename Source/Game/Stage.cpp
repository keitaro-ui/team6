#include "Stage.h"
#include "PlayerManager.h"
#include "WayPoint.h"
#include "EnemyManager.h"
#include "System/Raycast.h"
#include "ActionDerived.h"

//コンストラクタ
Stage::Stage()
{

    //ステージモデルを読み込み
    model = new Model("Data/Model/Stage/syusei.mdl");
    //model = new Model("Data/Model/Stage/StageT.mdl");
    //model = new Model("Data/Model/Stage/ExampleStage.mdl");

    scale.x = scale.y = scale.z = 0.02f;
    /*scale.x = scale.y = scale.z = 1.0f;*/
    //position.y = 4.0f;

    wayPointCount = 0;

    //for (int j = 0; j < wayPoints.size(); j++)
    //{
    //    // インデックス番号計算
    //    int index = j;
    //    //WayPoint生成
    //    //wayPoints[index] = new WayPoint();
    //    wayPoints[index] = std::make_unique<WayPoint>();

    //    // WayPointの座標決定
    //    //wayPoints[index]->position = DirectX::XMFLOAT3(i + 0.5f, 0.0f, j + 0.5f);

       

    // 手動で必要なノードを追加
   /* AddWayPoint({ -27.1f, -1.7f, 16.3f });
    AddWayPoint({ -27.1f, -1.7f, 9.7f });
    AddWayPoint({ -27.1f, -1.7f, 3.4f });
    AddWayPoint({ -27.1f, -1.7f, -3.4f });
    AddWayPoint({ -27.1f, -1.7f, -10.1f });
    AddWayPoint({ -27.1f, -1.7f, -16.2f });

    AddWayPoint({ -18, -1.7f, 16.3f });
    AddWayPoint({ -18, -1.7f, 9.7f });
    AddWayPoint({ -18, -1.7f, 3.4f });
    AddWayPoint({ -18, -1.7f, -3.4f });
    AddWayPoint({ -18, -1.7f, -10.1f });
    AddWayPoint({ -18, -1.7f, -16.2f });

    AddWayPoint({ -8.95f, -1.7f, 16.3f });
    AddWayPoint({ -8.95f, -1.7f, 9.7f });
    AddWayPoint({ -8.95f, -1.7f, 3.4f });
    AddWayPoint({ -8.95f, -1.7f, -3.4f });
    AddWayPoint({ -8.95f, -1.7f, -10.1f });
    AddWayPoint({ -8.95f, -1.7f, -16.2f });

    AddWayPoint({ 0, -1.7f, 16.3f });
    AddWayPoint({ 0, -1.7f, 9.7f });
    AddWayPoint({ 0, -1.7f, 3.4f });
    AddWayPoint({ 0, -1.7f, -3.4f });
    AddWayPoint({ 0, -1.7f, -10.1f });
    AddWayPoint({ 0, -1.7f, -16.2f });

    AddWayPoint({ 8.95f, -1.7f, 16.3f });
    AddWayPoint({ 8.95f, -1.7f, 9.7f });
    AddWayPoint({ 8.95f, -1.7f, 3.4f });
    AddWayPoint({ 8.95f, -1.7f, -3.4f });
    AddWayPoint({ 8.95f, -1.7f, -10.1f });
    AddWayPoint({ 8.95f, -1.7f, -16.2f });

    AddWayPoint({ 17.94f, -1.7f, 16.3f });
    AddWayPoint({ 17.94f, -1.7f, 9.7f });
    AddWayPoint({ 17.94f, -1.7f, 3.4f });
    AddWayPoint({ 17.94f, -1.7f, -3.4f });
    AddWayPoint({ 17.94f, -1.7f, -10.1f });
    AddWayPoint({ 17.94f, -1.7f, -16.2f });

    AddWayPoint({ 27, -1.7f, 16.3f });
    AddWayPoint({ 27, -1.7f, 9.7f });
    AddWayPoint({ 27, -1.7f, 3.4f });
    AddWayPoint({ 27, -1.7f, -3.4f });
    AddWayPoint({ 27, -1.7f, -10.1f });
    AddWayPoint({ 27, -1.7f, -16.2f });*/

    AddWayPoint({ -27.1f, 1.0f, 16.3f });
    AddWayPoint({ -27.1f, 1.0f, 9.7f });
    AddWayPoint({ -27.1f, 1.0f, 3.4f });
    AddWayPoint({ -27.1f, 1.0f, -3.4f });
    AddWayPoint({ -27.1f, 1.0f, -10.1f });
    AddWayPoint({ -27.1f, 1.0f, -16.2f });

    AddWayPoint({ -18, 1.0f, 16.3f });
    AddWayPoint({ -18, 1.0f, 9.7f });
    AddWayPoint({ -18, 1.0f, 3.4f });
    AddWayPoint({ -18, 1.0f, -3.4f });
    AddWayPoint({ -18, 1.0f, -10.1f });
    AddWayPoint({ -18, 1.0f, -16.2f });

    AddWayPoint({ -8.95f, 1.0f, 16.3f });
    AddWayPoint({ -8.95f, 1.0f, 9.7f });
    AddWayPoint({ -8.95f, 1.0f, 3.4f });
    AddWayPoint({ -8.95f, 1.0f, -3.4f });
    AddWayPoint({ -8.95f, 1.0f, -10.1f });
    AddWayPoint({ -8.95f, 1.0f, -16.2f });

    AddWayPoint({ 0, 1.0f, 16.3f });
    AddWayPoint({ 0, 1.0f, 9.7f });
    AddWayPoint({ 0, 1.0f, 3.4f });
    AddWayPoint({ 0, 1.0f, -3.4f });
    AddWayPoint({ 0, 1.0f, -10.1f });
    AddWayPoint({ 0, 1.0f, -16.2f });

    AddWayPoint({ 8.95f, 1.0f, 16.3f });
    AddWayPoint({ 8.95f, 1.0f, 9.7f });
    AddWayPoint({ 8.95f, 1.0f, 3.4f });
    AddWayPoint({ 8.95f, 1.0f, -3.4f });
    AddWayPoint({ 8.95f, 1.0f, -10.1f });
    AddWayPoint({ 8.95f, 1.0f, -16.2f });

    AddWayPoint({ 17.94f, 1.0f, 16.3f });
    AddWayPoint({ 17.94f, 1.0f, 9.7f });
    AddWayPoint({ 17.94f, 1.0f, 3.4f });
    AddWayPoint({ 17.94f, 1.0f, -3.4f });
    AddWayPoint({ 17.94f, 1.0f, -10.1f });
    AddWayPoint({ 17.94f, 1.0f, -16.2f });

    AddWayPoint({ 27, 1.0f, 16.3f });
    AddWayPoint({ 27, 1.0f, 9.7f });
    AddWayPoint({ 27, 1.0f, 3.4f });
    AddWayPoint({ 27, 1.0f, -3.4f });
    AddWayPoint({ 27, 1.0f, -10.1f });
    AddWayPoint({ 27, 1.0f, -16.2f });




     // WayPointに紐づくエッジを生成
    for (windex = 0; windex < 42; windex++)
    {
        for (int k = 0; k < 4; ++k)
        {
            wayPoints[windex]->edge[k] = new Edge();
            wayPoints[windex]->edge[k]->originPoint = windex;
        }
        ConnectWayPoints(windex);
    }

	//ステージデータを読み込む
	//model = new Model("Data/Model/Stage/smallroom.mdl");
	model = new Model("Data/Model/Stage/doorT.mdl");


	//loadTextures.LoadNormal("Data/Model/Stage/Texture/Small room/aiStandardSurface2_Normal_Utility - Raw.png");
	//loadTextures.LoadRoughness("Data/Model/Stage/Texture/Small room/aiStandardSurface2_Roughness_Utility - Raw.png");
	//loadTextures.LoadMetalness("Data/Model/Stage/Texture/Small room/aiStandardSurface2_Metallic_Utility - Raw.png");
	//loadTextures.LoadEmisive("Data/Model/Stage/Texture/Small room/aiStandardSurface2_Emissive_Utility - sRGB - Texture.png");

	loadTextures.LoadNormal("Data/Model/Stage/Texture/wall/wallceilings_standardSurface1_Normal_Utility - Raw.png");
	loadTextures.LoadRoughness("Data/Model/Stage/Texture/wall/wallceilings_standardSurface1_Roughness_Utility - Raw.png");
	loadTextures.LoadMetalness("Data/Model/Stage/Texture/wall/wallceilings_standardSurface1_Metallic_Utility - Raw.png");

	models.push_back(std::make_unique<Model>("Data/Model/Stage/wallceiling.mdl"));

	{
		LoadTextures tex;
		tex.LoadNormal("Data/Model/Stage/Texture/wall/wallceilings_standardSurface1_Normal_Utility - Raw.png");
		tex.LoadRoughness("Data/Model/Stage/Texture/wall/wallceilings_standardSurface1_Roughness_Utility - Raw.png");
		tex.LoadMetalness("Data/Model/Stage/Texture/wall/wallceilings_standardSurface1_Metallic_Utility - Raw.png");
		TextureList.push_back(tex);
	}

	models.push_back(std::make_unique<Model>("Data/Model/Stage/smallroomY.mdl"));

	{
		LoadTextures tex;
		tex.LoadNormal("Data/Model/Stage/Texture/object/walldata_Normal.png");
		tex.LoadRoughness("Data/Model/Stage/Texture/object/walldata_Roughness.png");
		tex.LoadMetalness("Data/Model/Stage/Texture/object/walldata_Metalness.png");
		TextureList.push_back(tex);
	}

    models.push_back(std::make_unique<Model>("Data/Model/Stage/objectall.mdl"));

    {
        LoadTextures tex;
        tex.LoadNormal("Data/Model/Stage/Texture/komono_poul/objectall_lambert6_Normal_Utility - Raw.png");
        tex.LoadRoughness("Data/Model/Stage/Texture/komono_poul/objectall_lambert6_Roughness_Utility - Raw.png");
        tex.LoadMetalness("Data/Model/Stage/Texture/komono_poul/objectall_lambert6_Metallic_Utility - Raw.png");
        tex.LoadEmisive("Data/Model/Stage/Texture/komono_poul/objectall_lambert6_Emissive_Utility - sRGB - Texture.png");
        TextureList.push_back(tex);
    }
	
    models.push_back(std::make_unique<Model>("Data/Model/Stage/saku.mdl"));


    
    //door.model = std::make_unique<Model>("Data/Model/Stage/door.mdl");
    door = std::make_unique<Model>("Data/Model/Stage/door.mdl");

	scale.x = scale.y = scale.z = 0.02f;
	/*scale.x = scale.y = scale.z = 1.0f;*/
    doorPos.x = 0.0f;
	doorPos.y = 1.0f;
	doorPos.z = 17.8f;
    doorAngle.y = DirectX::XMConvertToRadians(-180.0f);
	//position.y = 4.0f;
<<<<<<< HEAD
=======
    DirectX::XMFLOAT3 doorPos = { 0,0,0 };
 
>>>>>>> master

}


Stage::~Stage()
{
	//ステージモデルを破棄
	delete model;
    
   // Clear();
}

//更新処理
void Stage::Update(float elapsedTime)
{
	UpdateTransform();

	RenderImGui();
	//DirectX::XMVector2LinePointDistance();
	//RenderImGui();
    DirectX::XMStoreFloat4x4(&transformDoor,
        DirectX::XMMatrixScaling(scale.x,scale.y,scale.z) *
        DirectX::XMMatrixRotationRollPitchYaw(doorAngle.x,doorAngle.y,doorAngle.z)*
        DirectX::XMMatrixTranslation(doorPos.x, doorPos.y, doorPos.z)
    );
}

//描画処理
void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    //消す
    // ウェイポイントの描画処理
    for (int i = 0; i <= MAX_WAY_POINT; i++)
    {
        // MAX_WAY_POINTよりwayPointsの要素が少ない場合範囲外アクセスでエラーが発生

        // wayPointsのサイズより大きい場合にアクセスしないように制限
        if (i < wayPoints.size())
            wayPoints[i]->Render(rc, renderer);
    }
	//DirectX::XMFLOAT4X4 transform;
	//DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());


	//レンダラモデルに描画してもらう
	//renderer->Render(rc, transform, model, ShaderId::Lambert);

	for (size_t i = 0; i < models.size(); i++)
	{
        // �@ �e�N�X�`��������ꍇ���� Set
        if (i < TextureList.size())
        {
            TextureList[i].Set(rc);
        }

        // �A �`��
        renderer->Render(rc, transform, models[i].get(), ShaderId::Lambert);

        // �B �e�N�X�`��������ꍇ���� Clear
        if (i < TextureList.size())
        {
            TextureList[i].Clear(rc);
        }
	}



<<<<<<< HEAD
=======
    renderer->Render(rc, transformDoor, door.get(), ShaderId::Lambert);


>>>>>>> master
}

//void Stage::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
//{
//    
//}



void Stage::DestinationPointSet(int index)
{
#if 0
    //edge[0] = 上（12時）
    //edge[1] = 右（3時）
    //edge[2] = 下（6時）
    //edge[3] = 左（9時)

    int x = index % COLUM_COUNT;

    // 0: 12時（上）
    int point = index - COLUM_COUNT;
    if (point >= 0)
    {
        wayPoint[index]->edge[0]->destinationPoint = point;

        DirectX::XMVECTOR dest = DirectX::XMLoadFloat3(&wayPoint[point]->position);
        DirectX::XMVECTOR orig = DirectX::XMLoadFloat3(&wayPoint[index]->position);
        DirectX::XMVECTOR len = DirectX::XMVector3Length(DirectX::XMVectorSubtract(dest, orig));

        wayPoint[index]->edge[0]->cost = DirectX::XMVectorGetX(len);
    }
    else
    {
        wayPoint[index]->edge[0]->destinationPoint = -1;
        wayPoint[index]->edge[0]->cost = FLT_MAX;
    }


    // 1: 3時（右）
    point = index + 1;
    if ((x + 1) < COLUM_COUNT)
    {
        wayPoint[index]->edge[1]->destinationPoint = point;

        DirectX::XMVECTOR dest = DirectX::XMLoadFloat3(&wayPoint[point]->position);
        DirectX::XMVECTOR orig = DirectX::XMLoadFloat3(&wayPoint[index]->position);
        DirectX::XMVECTOR len = DirectX::XMVector3Length(DirectX::XMVectorSubtract(dest, orig));

        wayPoint[index]->edge[1]->cost = DirectX::XMVectorGetX(len);
    }
    else
    {
        wayPoint[index]->edge[1]->destinationPoint = -1;
        wayPoint[index]->edge[1]->cost = FLT_MAX;
    }


    // 2: 6時（下）
    point = index + COLUM_COUNT;
    if (point < MAX_WAY_POINT)
    {
        wayPoint[index]->edge[2]->destinationPoint = point;

        DirectX::XMVECTOR dest = DirectX::XMLoadFloat3(&wayPoint[point]->position);
        DirectX::XMVECTOR orig = DirectX::XMLoadFloat3(&wayPoint[index]->position);
        DirectX::XMVECTOR len = DirectX::XMVector3Length(DirectX::XMVectorSubtract(dest, orig));

        wayPoint[index]->edge[2]->cost = DirectX::XMVectorGetX(len);
    }
    else
    {
        wayPoint[index]->edge[2]->destinationPoint = -1;
        wayPoint[index]->edge[2]->cost = FLT_MAX;
    }


    // 3: 9時（左）
    point = index - 1;
    if (x > 0)
    {
        wayPoint[index]->edge[3]->destinationPoint = point;

        DirectX::XMVECTOR dest = DirectX::XMLoadFloat3(&wayPoint[point]->position);
        DirectX::XMVECTOR orig = DirectX::XMLoadFloat3(&wayPoint[index]->position);
        DirectX::XMVECTOR len = DirectX::XMVector3Length(DirectX::XMVectorSubtract(dest, orig));

        wayPoint[index]->edge[3]->cost = DirectX::XMVectorGetX(len);
    }
    else
    {
        wayPoint[index]->edge[3]->destinationPoint = -1;
        wayPoint[index]->edge[3]->cost = FLT_MAX;
    }

    //余った４～７個を無効にする
    for (int i = 4; i < 8; i++)
    {
        wayPoint[index]->edge[i]->destinationPoint = -1;
        wayPoint[index]->edge[i]->cost = FLT_MAX;
    }
#endif
}

void Stage::AddWayPoint(const DirectX::XMFLOAT3& pos)
{
    //for (int j = 0; j < COLUM_COUNT; ++j)
    //{
    //    //if (wayPointCount >= MAX_WAY_POINT)
    //    //{
    //    //    OutputDebugStringA("ERROR: MAX_WAY_POINT overflow!\n");
    //    //    return;
    //    //}

    //    //// ここでウェイポイント生成する
    //    std::unique_ptr<WayPoint> w = std::make_unique<WayPoint>();

    //    ////WayPoint w = *wayPoint[wayPointCount];
    //    w->position = pos;


    //    // インデックス番号計算
    //    int index = /*COLUM_COUNT **/ j;



    //    //// Edge を初期化
    //    //for (int i = 0; i < w->GetEdgeNo(); i++)
    //    //    w->edge[i] = nullptr;

    //    ////wayPoints についか
    //    //wayPoints.emplace_back(std::move(w));

    //    //wayPointCount++;

    //    if (wayPointCount >= MAX_WAY_POINT)
    //    {
    //        OutputDebugStringA("ERROR: MAX_WAY_POINT overflow!\n");
    //        return;
    //    }

    //    //// 新しい WayPoint を生成
    //    //std::unique_ptr<WayPoint> w = std::make_unique<WayPoint>();

    //    // 位置セット
    //    w->position = pos;

    //    // Edge 初期化（8方向 or 4方向など固定長）
    //    for (int i = 0; i < 4; i++)
    //    {
    //        w->edge[i] = nullptr;
    //    }

    //    // vector に追加
    //    wayPoints.emplace_back(std::move(w));

    //    // originPoint を正しく設定するために
    //    // vector の実 index を WayPoint に渡す
    //    //wayPoints[wayPointCount]->index = wayPointCount;  // こういうメンバを追加すると良い
    //    wayPoints[index]->position = wayPoints[index]->position;

    //}
    //wayPointCount++;

        if (wayPointCount >= MAX_WAY_POINT)
        {
            OutputDebugStringA("ERROR: MAX_WAY_POINT overflow!\n");
            return;
        }

        // ここでウェイポイント生成する
        std::unique_ptr<WayPoint> w = std::make_unique<WayPoint>();

        //WayPoint w = *wayPoint[wayPointCount];
        w->position = pos;




        // Edge を初期化
        for (int i = 0; i < w->GetEdgeNo(); i++)
            w->edge[i] = nullptr;

        //wayPoints についか
        wayPoints.emplace_back(std::move(w));

        wayPointCount++;
    
}

void Stage::ConnectWayPoints(int index)
{
    
    int x = index % COLUM_COUNT;

    // エッジの行先指定
    // 12時の方向
    // -COLUM_COUNTの値が0以上であれば
    int point = index - COLUM_COUNT;
    DirectX::XMFLOAT3 hitPosition, hitNormal;

       if (point >= 0)
       {
           wayPoints[index]->edge[0]->destinationPoint = point;
           // 2点間の距離を計算してエッジのコストに追加
           DirectX::XMVECTOR destinationPoint = DirectX::XMLoadFloat3(&wayPoints[point]->position);
           DirectX::XMVECTOR originPoint = DirectX::XMLoadFloat3(&wayPoints[index]->position);
           DirectX::XMVECTOR cost = DirectX::XMVector3Length(DirectX::XMVectorSubtract(destinationPoint, originPoint));
          /* if (Raycast::RayCast(wayPoints[point]->position,
               wayPoints[index]->position, transform,
               model, hitPosition, hitNormal))
           {
               wayPoints[index]->edge[0]->cost = FLT_MAX;
           }*/
           /*else*/
           wayPoints[index]->edge[0]->cost = DirectX::XMVectorGetX(cost);
       }
       else
       {
           wayPoints[index]->edge[0]->destinationPoint = -1;
           wayPoints[index]->edge[0]->cost = FLT_MAX;
       }
       // 3時の方向
       // (j+1)が横幅を超えないとき
       point = index + 1;
       if ((x + 1) < COLUM_COUNT)
       {
           wayPoints[index]->edge[1]->destinationPoint = point;
           // 2点間の距離を計算してエッジのコストに追加
           DirectX::XMVECTOR destinationPoint = DirectX::XMLoadFloat3(&wayPoints[point]->position);
           DirectX::XMVECTOR originPoint = DirectX::XMLoadFloat3(&wayPoints[index]->position);
           DirectX::XMVECTOR cost = DirectX::XMVector3Length(DirectX::XMVectorSubtract(destinationPoint, originPoint));
           /*if (Raycast::RayCast(wayPoints[point]->position,
               wayPoints[index]->position, transform,
               model, hitPosition, hitNormal))
           {
               wayPoints[index]->edge[1]->cost = FLT_MAX;
           }
           else*/
           wayPoints[index]->edge[1]->cost = DirectX::XMVectorGetX(cost);
       }
       else
       {
           wayPoints[index]->edge[1]->destinationPoint = -1;
           wayPoints[index]->edge[1]->cost = FLT_MAX;
       }
       // 6時の方向
       // +COLUM_COUNTの値が最大値未満のとき
       point = index + COLUM_COUNT;
       if (point < MAX_WAY_POINT)
       {
           wayPoints[index]->edge[2]->destinationPoint = point;
           // 2点間の距離を計算してエッジのコストに追加
           DirectX::XMVECTOR destinationPoint = DirectX::XMLoadFloat3(&wayPoints[point]->position);
           DirectX::XMVECTOR originPoint = DirectX::XMLoadFloat3(&wayPoints[index]->position);
           DirectX::XMVECTOR cost = DirectX::XMVector3Length(DirectX::XMVectorSubtract(destinationPoint, originPoint));
          /* if (Raycast::RayCast(wayPoints[point]->position,
               wayPoints[index]->position, transform,
               model, hitPosition, hitNormal))
           {
               wayPoints[index]->edge[2]->cost = FLT_MAX;
           }
           else*/
               wayPoints[index]->edge[2]->cost = DirectX::XMVectorGetX(cost);
       }
       else
       {
           wayPoints[index]->edge[2]->destinationPoint = -1;
           wayPoints[index]->edge[2]->cost = FLT_MAX;
       }
       // 9時の方向
       // jが0より多きいとき
       point = index - 1;
       if (x > 0)
       {
           wayPoints[index]->edge[3]->destinationPoint = point;
           // 2点間の距離を計算してエッジのコストに追加
           DirectX::XMVECTOR destinationPoint = DirectX::XMLoadFloat3(&wayPoints[point]->position);
           DirectX::XMVECTOR originPoint = DirectX::XMLoadFloat3(&wayPoints[index]->position);
           DirectX::XMVECTOR cost = DirectX::XMVector3Length(DirectX::XMVectorSubtract(destinationPoint, originPoint));
           /*if (Raycast::RayCast(wayPoints[point]->position,
               wayPoints[index]->position, 
               transform,
               model, hitPosition, hitNormal))
           {
               wayPoints[index]->edge[3]->cost = FLT_MAX;
           }
           else*/
           wayPoints[index]->edge[3]->cost = DirectX::XMVectorGetX(cost);
       }
       else
       {
           wayPoints[index]->edge[3]->destinationPoint = -1;
           wayPoints[index]->edge[3]->cost = FLT_MAX;
       }
    
}

float Stage::Distance2D(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
{
    float dx = a.x - b.x;
    float dz = a.z - b.z;
    return sqrtf(dx * dx + dz * dz);
}



void Stage::Clear()
{
    //for (WayPoint wayPoints : wayPoints)
    //{
    //    delete wayPoints;
    //}
    //wayPoints.clear();
}


void Stage::RenderImGui()
{
	DirectX::XMFLOAT3 angleDeg = {
	DirectX::XMConvertToDegrees(angle.x),
	DirectX::XMConvertToDegrees(angle.y),
	DirectX::XMConvertToDegrees(angle.z)
	};
	if (ImGui::Begin("Stage Transform"))
	{
		ImGui::Text("Stage");
		ImGui::Separator();
		
		// 各Transform要素を操作
		ImGui::DragFloat3("Position", &position.x, 0.1f, -1000.0f, 1000.0f);
		if (ImGui::DragFloat3("Rotation (deg)", &angleDeg.x, 1.0f, -360.0f, 360.0f))
		{
			// 変更された場合のみラジアンに戻す
			angle.x = DirectX::XMConvertToRadians(angleDeg.x);
			angle.y = DirectX::XMConvertToRadians(angleDeg.y);
			angle.z = DirectX::XMConvertToRadians(angleDeg.z);
		}
		ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.001f, 10.0f);
	}
	ImGui::End();

    if (ImGui::Begin("WayPoint Editor"))
    {
        ImGui::Text("WayPoint Count : %d", wayPointCount);

        if (ImGui::Button("Add WayPoint"))
        {
            AddWayPoint({ 0, 0, 0 });
        }

        ImGui::Separator();

        for (int i = 0; i < wayPointCount; i++)
        {
            ImGui::PushID(i);

            ImGui::Text("WP %d", i);
            ImGui::DragFloat3("Position", &wayPoints[i]->position.x, 0.1f);

            ImGui::Separator();
            ImGui::PopID();
        }
    }
    ImGui::End();
}

// インデックス番号からウェイポイントの座標を取得
DirectX::XMFLOAT3 Stage::GetIndexWayPoint(int index)
{

	return wayPoints[index]->position;
}

// 座標から一番近いウェイポイントのインデックスを取得
int Stage::NearWayPointIndex(DirectX::XMFLOAT3 target)
{
    float minLength = FLT_MAX;
    float length = 0.0f;
    int index = -1;
    // VECTORに変換
    DirectX::XMVECTOR targetPos = DirectX::XMLoadFloat3(&target);

    for (int i = 0; i < MAX_WAY_POINT; ++i)
    {
        DirectX::XMVECTOR point = DirectX::XMLoadFloat3(&(wayPoints[i]->position));
        // 距離を求める
        DirectX::XMVECTOR vector = DirectX::XMVectorSubtract(targetPos, point);
        DirectX::XMVECTOR vectorLength = DirectX::XMVector3Length(vector);
        DirectX::XMStoreFloat(&length, vectorLength);

         //求めた距離が保存しているものより小さければ
        if (minLength > length)
        {
            // 値を更新
            minLength = length;

            index = i;
        }
    }
    return index;

   /* float minDistSq = FLT_MAX;
    int bestIndex = -1;

    DirectX::XMFLOAT3 a = target;

    for (int dest : neighborIndexes)
    {
        DirectX::XMFLOAT3 b = StageManager::Instance().GetStage()->wayPoints[dest]->position;

        float dx = a.x - b.x;
        float dy = a.y - b.y;
        float dz = a.z - b.z;

        float distSq = dx * dx + dy * dy + dz * dz;

        if (distSq < minDistSq)
        {
            minDistSq = distSq;
            bestIndex = dest;
        }
    }*/
}
