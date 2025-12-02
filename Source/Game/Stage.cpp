#include "Stage.h"
#include "PlayerManager.h"
#include "WayPoint.h"

//コンストラクタ
Stage::Stage()
{
	//ステージモデルを読み込み
	//model = new Model("Data/Model/Stage/smallroom.mdl");
	model = new Model("Data/Model/Stage/wallceiling.mdl");


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

	models.push_back(std::make_unique<Model>("Data/Model/Stage/smallroom.mdl"));

	{
		LoadTextures tex;
		tex.LoadNormal("Data/Model/Stage/Texture/object/walldata_Normal.png");
		tex.LoadRoughness("Data/Model/Stage/Texture/object/walldata_Roughness.png");
		tex.LoadMetalness("Data/Model/Stage/Texture/object/walldata_Metalness.png");
		TextureList.push_back(tex);
	}
	
	scale.x = scale.y = scale.z = 0.02f;
	/*scale.x = scale.y = scale.z = 1.0f;*/
<<<<<<< HEAD

=======
	position.y = 4.0f;

 
>>>>>>> 6e252551dfff2d12555e88397ab1e007b2b71cc1
}

Stage::~Stage()
{
	//ステージモデルを破棄
	delete model;
	
}

//更新処理
void Stage::Update(float elapsedTime)
{
	UpdateTransform();

	//RenderImGui();

}

//描画処理
void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	//DirectX::XMFLOAT4X4 transform;
	//DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());

	for (size_t i = 0; i < models.size(); i++)
	{
		TextureList[i].Set(rc);
		renderer->Render(rc, transform, models[i].get(), ShaderId::Lambert);
		TextureList[i].Clear(rc);
	}

}

void Stage::DestinationPointSet(int index)
{
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
}

// インデックス番号からウェイポイントの座標を取得
DirectX::XMFLOAT3 Stage::GetIndexWayPoint(int index)
{

	return wayPoint[index]->position;
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
        DirectX::XMVECTOR point = DirectX::XMLoadFloat3(&(wayPoint[i]->position));
        // 距離を求める
        DirectX::XMVECTOR vector = DirectX::XMVectorSubtract(targetPos, point);
        DirectX::XMVECTOR vectorLength = DirectX::XMVector3Length(vector);
        DirectX::XMStoreFloat(&length, vectorLength);

        // 求めた距離が保存しているものより小さければ
        if (minLength > length)
        {
            // 値を更新
            minLength = length;
            index = i;
        }
    }
    return index;
}
