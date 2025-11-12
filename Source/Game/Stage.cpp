#include "Stage.h"

//コンストラクタ
Stage::Stage()
{
	//ステージモデルを読み込み
	model = new Model("Data/Model/Stage/itsuki_stagedata.mdl");

	scale.x = scale.y = scale.z = 0.02f;
	position.y = 4.0f;
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

	RenderImGui();
}

//描画処理
void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	//DirectX::XMFLOAT4X4 transform;
	//DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());

	//レンダラモデルに描画してもらう
	renderer->Render(rc, transform, model, ShaderId::Lambert);
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

