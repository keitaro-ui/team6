#include "Stage.h"

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

