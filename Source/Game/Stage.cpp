#include "Stage.h"
#include "PlayerManager.h"
#include "WayPoint.h"

//コンストラクタ
Stage::Stage()
{
	//ステージモデルを読み込み
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
    DirectX::XMFLOAT3 doorPos = { 0,0,0 };
 

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

    DirectX::XMStoreFloat4x4(&transformDoor,
        DirectX::XMMatrixScaling(scale.x,scale.y,scale.z) *
        DirectX::XMMatrixRotationRollPitchYaw(doorAngle.x,doorAngle.y,doorAngle.z)*
        DirectX::XMMatrixTranslation(doorPos.x, doorPos.y, doorPos.z)
    );
}

//描画処理
void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
{

	for (size_t i = 0; i < models.size(); i++)
	{
        // ① テクスチャがある場合だけ Set
        if (i < TextureList.size())
        {
            TextureList[i].Set(rc);
        }

        // ② 描画
        renderer->Render(rc, transform, models[i].get(), ShaderId::Lambert);

        // ③ テクスチャがある場合だけ Clear
        if (i < TextureList.size())
        {
            TextureList[i].Clear(rc);
        }
	}

    renderer->Render(rc, transformDoor, door.get(), ShaderId::Lambert);
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
