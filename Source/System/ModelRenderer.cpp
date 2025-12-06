#include <algorithm>
#include <cstring>

#include "imgui.h"
#include "Misc.h"
#include "GpuResourceUtils.h"

#include "ModelRenderer.h"
#include "BasicShader.h"
#include "LambertShader.h"
#include "PhongShader.h"
#include "ShadowMapShader.h"
#include "Camera.h"

void ModelRenderer::SetPointLight(int index, const point_lights& light)
{
	if (index < 0 || index >= 16) return;
	point_light[index] = light;
}

void ModelRenderer::SetAmbientColor(const DirectX::XMFLOAT4& color)
{
	ambient_color = color;
}

void ModelRenderer::UpdateSafetyAreaLights(const std::vector<DirectX::XMFLOAT4>& position)
{
	for (size_t i = 0; i < position.size() && i < 16; ++i)
	{
		point_light[i].position = position[i];
		point_light[i].range = 15.0f;       // 適当な範囲
		point_light[i].color = { 1, 1, 1, 1 }; // 光の色
	}
}

void ModelRenderer::ResetHorror()
{
	horrorPhase = -1;
	horrorFrame = 0;
	horrorTimer = 0.0f;

	dx = dy = dz = 0;
	moved = 0;
	accumulatedDistanceT = 0;

	lastPos = Camera::Instance().GetEye();
	nowPos = Camera::Instance().GetEye();
	
	first = true;
	Event = false;


	sceneConstantBufferData.LightSwitch = 0.0f;
	sceneConstantBufferData.SpotLightSwitch = 1.0f;
}

// コンストラクタ
ModelRenderer::ModelRenderer(ID3D11Device* device)
{
	// シーン用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbScene),
		sceneConstantBuffer.GetAddressOf());

	// スケルトン用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbSkeleton),
		skeletonConstantBuffer.GetAddressOf());

	// ライト定数バッファ生成
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(light_constants),
		light_constant_buffer.GetAddressOf());

	// デバッグ用 ShapeRenderer の生成（ライトの可視化）
	debugRenderer = std::make_unique<ShapeRenderer>(device);

	//ライトの初期化
	InitLights();

	mt = std::mt19937(std::random_device{}());
	dist = std::uniform_real_distribution<float>(0.0f, 1.0f);

	lastPos = Camera::Instance().GetEye();
	horrorTimer = 0.0f;
	horrorFrame = 0;
	horrorPhase = -1;
	first = true;
	sceneConstantBufferData.LightSwitch = 0.0f;
	sceneConstantBufferData.SpotLightSwitch = 1.0f;
	dx = dy = dz = 0;
	moved = 0;
	Event = false;

	// シェーダー生成
	shaders[static_cast<int>(ShaderId::Basic)] = std::make_unique<BasicShader>(device);
	shaders[static_cast<int>(ShaderId::Lambert)] = std::make_unique<LambertShader>(device);
	shaders[static_cast<int>(ShaderId::Phong)] = std::make_unique<PhongShader>(device);
	shaders[static_cast<int>(ShaderId::Shadow)] = std::make_unique<ShadowMapShader>(device);
}

void ModelRenderer::InitLights()
{
	// 全ライトを 0 クリア
	ZeroMemory(point_light, sizeof(point_light));
	ZeroMemory(spot_light, sizeof(spot_light));

	point_light[0].position = { 0.0f, 4.0f, 5.0f, 1.0f };
	point_light[0].range = 0.0;
	point_light[0].color = { 1, 1, 1, 1 };

	point_light[1].position = { -10.0f, -2.0f, 0.0f, 1.0f };
	point_light[1].range = 0.0f;
	point_light[1].color = { 0, 1, 0, 1 };

	point_light[2].position = { 28.0f, -2.0f, -17.0f, 1.0f };
	point_light[2].range = 3.0f;
	point_light[2].color = { 1, 1, 1, 1 };

	point_light[3].position = { -28.0f, -2.0f, -17.0f, 1.0f };
	point_light[3].range = 3.0f;
	point_light[3].color = { 1, 1, 1, 1 };

	point_light[4].position = { -28.0f,-2.0f,17.0f,1.0f };
	point_light[4].range = 3.0f;
	point_light[4].color = { 1.0f,1.0f,1.0f,1.0f };

	point_light[5].position = { 28.0f,-2.0f,17.0f,1.0f };
	point_light[5].range = 3.0f;
	point_light[5].color = { 1.0f,1.0f,1.0f,1.0f };

	point_light[6].position = { 0.0f,-2.0f,17.0f,1.0f };
	point_light[6].range = 3.0f;
	point_light[6].color = { 1.0f,1.0f,1.0f,1.0f };

	point_light[7].position = { 0.0f,-2.0f,-17.0f,1.0f };
	point_light[7].range = 3.0f;
	point_light[7].color = { 1.0f,1.0f,1.0f,1.0f };

	// スポットライトは空（カメラ追従で埋める）
	ZeroMemory(&spot_light[0], sizeof(spot_lights) * 8);
	spot_light[0].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	spot_light[0].range = 20.0;
	spot_light[0].innerCorn = cosf(DirectX::XMConvertToRadians(30.0f));
	spot_light[0].outerCorn = cosf(DirectX::XMConvertToRadians(40.0f));
}

// 描画実行
void ModelRenderer::Render(const RenderContext& rc, 
	const DirectX::XMFLOAT4X4& worldTransform, 
	const Model* model,
	ShaderId shaderId)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	nowPos = Camera::Instance().GetEye();
	if (first)
	{
		lastPos = nowPos; 
		first = false;

	}

	dx = nowPos.x - lastPos.x;
	dy = nowPos.y - lastPos.y;
	dz = nowPos.z - lastPos.z;

	moved = sqrtf(dx * dx + dy * dy + dz * dz);
	accumulatedDistanceT += moved;

	lastPos = nowPos;

	if (accumulatedDistanceT > 8.0f&&!Event)
	{
		horrorTimer = 0.0f;
		horrorPhase = 0;
		Event = true;
	}

	UpdateLightSwitch();

	UpdataLightsSS();

	// カメラ位置にスポットライトを追従
	UpdateSpotLightFromCamera();

	// 定数バッファ更新
	UpdateSceneConstantBuffer(dc, rc);
	UpdateLightConstantBuffer(dc);

	// 定数バッファ・サンプラーをバインド
	SetConstantBuffers(dc);
	SetSamplers(dc, rc);

	// モデルの描画
	DrawModel(rc, dc, worldTransform, model,shaderId);
	
	// 描画後にバインディング解除
	ClearBinding(dc);
}

void ModelRenderer::RenderImGui(const RenderContext& rc)
{
	if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 環境光 / ディレクショナル
		ImGui::ColorEdit3("Ambient Color", &ambient_color.x);
		ImGui::ColorEdit3("Directional Color", &directional_light_color.x);
		ImGui::SliderFloat3("Directional Dir", &directional_light_direction.x, -1.0f, 1.0f);


		// ライト ON/OFF
		bool lightingEnabled = sceneConstantBufferData.useLighting > 0.5f;
		if (ImGui::Checkbox("Enable Lighting", &lightingEnabled))
		{
			sceneConstantBufferData.useLighting = lightingEnabled ? 1.0f : 0.0f;
		}
		if (GetAsyncKeyState('U') & 1)
		{
			lightingEnabled = !lightingEnabled;
			sceneConstantBufferData.useLighting = lightingEnabled ? 1.0f : 0.0f;
		}

		// ポイントライト可視化
		if (ImGui::CollapsingHeader("Point Lights"))
		{
			for (int i = 0; i < 8; ++i)
			{
				std::string label = "PointLight " + std::to_string(i);
				if (ImGui::TreeNode(label.c_str()))
				{
					ImGui::ColorEdit3("Color", &point_light[i].color.x);
					ImGui::SliderFloat3("Position", &point_light[i].position.x, -30.0f, 30.0f);
					ImGui::SliderFloat("Range", &point_light[i].range, 0.1f, 50.0f);
					ImGui::TreePop();
				}

				if (point_light[i].range > 0.0f)
				{
					DirectX::XMFLOAT3 pos = { point_light[i].position.x, point_light[i].position.y, point_light[i].position.z };
					DirectX::XMFLOAT4 col = point_light[i].color;
					debugRenderer->RenderSphere(rc, pos, 0.5f, col);
				}
			}
		}

		// スポットライト可視化
		if (ImGui::CollapsingHeader("Spot Lights"))
		{
			for (int i = 0; i < 8; ++i)
			{
				std::string label = "SpotLight " + std::to_string(i);
				if (ImGui::TreeNode(label.c_str()))
				{
					ImGui::ColorEdit3("Color", &spot_light[i].color.x);
					ImGui::SliderFloat3("Position", &spot_light[i].position.x, -20.0f, 20.0f);
					ImGui::SliderFloat3("Direction", &spot_light[i].direction.x, -2.0f, 1.0f);
					ImGui::SliderFloat("Range", &spot_light[i].range, 0.1f, 50.0f);

					ImGui::SliderFloat("DirLight Yaw", &dirLightYaw, -45.0f, 45.0f);
					ImGui::SliderFloat("DirLight Pitch", &dirLightPitch, -45.0f, 45.0f);

					float innerDeg = acosf(spot_light[i].innerCorn) * 180.0f / DirectX::XM_PI;
					float outerDeg = acosf(spot_light[i].outerCorn) * 180.0f / DirectX::XM_PI;
					if (ImGui::SliderFloat("Inner Angle (deg)", &innerDeg, 1.0f, 60.0f))
						spot_light[i].innerCorn = cosf(DirectX::XMConvertToRadians(innerDeg));
					if (ImGui::SliderFloat("Outer Angle (deg)", &outerDeg, 1.0f, 70.0f))
						spot_light[i].outerCorn = cosf(DirectX::XMConvertToRadians(outerDeg));

					ImGui::TreePop();
				}

				if (spot_light[i].range > 0.0f)
				{
					DirectX::XMFLOAT3 pos = { spot_light[i].position.x, spot_light[i].position.y, spot_light[i].position.z };
					DirectX::XMFLOAT4 col = spot_light[i].color;
					//debugRenderer->RenderCircle(rc, pos, 0.6f, col);
				}
				DirectX::XMFLOAT3 dir;
				dir.x = cosf(dirLightPitch) * cosf(dirLightYaw);
				dir.y = sinf(dirLightPitch);
				dir.z = cosf(dirLightPitch) * sinf(dirLightYaw);

				// 正規化
				DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&dir);
				v = DirectX::XMVector3Normalize(v);
				DirectX::XMStoreFloat3(&dir, v);

				// Directional Light に反映
				directional_light_direction = { dir.x, dir.y, dir.z,1.0f };
			}
		}
	}
}

void ModelRenderer::UpdateSpotLightFromCamera()
{
	const auto& cam = Camera::Instance();
	DirectX::XMFLOAT3 pos = cam.GetEye();
	DirectX::XMFLOAT3 forward = cam.GetFront();
	DirectX::XMFLOAT3 right = cam.GetRight();
	DirectX::XMFLOAT3 up = cam.GetUp();

	// カメラの前・横・下へオフセット
	float forwardOffset = 0.6f;  
	float downOffset = -0.3f; 
	float sideOffset = 0.2f;

	DirectX::XMFLOAT3 lightPos = {
	pos.x + forward.x * forwardOffset + right.x * sideOffset,
	pos.y + forward.y * forwardOffset + right.y * sideOffset + downOffset,
	pos.z + forward.z * forwardOffset + right.z * sideOffset
	};

	float downTilt = -sinf(DirectX::XMConvertToRadians(dirLightYaw));
	float sideTilt = sinf(DirectX::XMConvertToRadians(dirLightPitch));


	DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&forward);

	// Yaw (Y軸回転)
	DirectX::XMMATRIX yawRot = DirectX::XMMatrixRotationY(sideTilt);

	// Pitch (右方向軸で回転)
	DirectX::XMVECTOR rightVec = DirectX::XMLoadFloat3(&right);
	DirectX::XMMATRIX pitchRot = DirectX::XMMatrixRotationAxis(rightVec, downTilt);

	dir = DirectX::XMVector3TransformNormal(dir, pitchRot * yawRot);

	DirectX::XMFLOAT3 lightDir;
	DirectX::XMStoreFloat3(&lightDir, dir);

	// 正規化
	dir = DirectX::XMVector3Normalize(dir);
	DirectX::XMStoreFloat3(&lightDir, dir);

	spot_light[0].position = { lightPos.x, lightPos.y, lightPos.z, 1.0f };
	spot_light[0].direction = { lightDir.x, lightDir.y, lightDir.z, 0.0f };


}

void ModelRenderer::UpdateLightConstantBuffer(ID3D11DeviceContext* dc)
{
	light_constants lights{};
	lights.ambient_color = ambient_color;
	lights.directional_light_direction = directional_light_direction;
	lights.directional_light_color = directional_light_color;
	// 配列コピー
	memcpy_s(lights.point_light, sizeof(lights.point_light), point_light, sizeof(point_light));
	memcpy_s(lights.spot_light, sizeof(lights.spot_light), spot_light, sizeof(spot_light));

	dc->UpdateSubresource(light_constant_buffer.Get(), 0, 0, &lights, 0, 0);
}

void ModelRenderer::UpdateSceneConstantBuffer(ID3D11DeviceContext* dc, const RenderContext& rc)
{
	CbScene cbScene{};
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
	// VP 行列
	DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);

	// ライト方向
	cbScene.lightDirection.x = rc.lightDirection.x;
	cbScene.lightDirection.y = rc.lightDirection.y;
	cbScene.lightDirection.z = rc.lightDirection.z;

	DirectX::XMFLOAT3 camPos = Camera::Instance().GetEye();
	cbScene.cameraPosition.x = camPos.x;
	cbScene.cameraPosition.y = camPos.y;
	cbScene.cameraPosition.z = camPos.z;
	cbScene.cameraPosition.w = 0.0f;

	cbScene.lightingMultiplier = sceneConstantBufferData.lightingMultiplier;
	cbScene.useLighting = sceneConstantBufferData.useLighting;
	cbScene.LightSwitch = sceneConstantBufferData.LightSwitch;
	cbScene.SpotLightSwitch = sceneConstantBufferData.SpotLightSwitch;

	dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
}

void ModelRenderer::DrawMesh(const RenderContext& rc,
	ID3D11DeviceContext* dc, 
	const ModelResource::Mesh& mesh,
	const Model* model, 
	const DirectX::XMMATRIX& worldTransform, 
	Shader* shader)
{
	// 頂点バッファ設定
	UINT stride = sizeof(ModelResource::Vertex);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// スケルトン用定数バッファ更新
	CbSkeleton cbSkeleton{};
	if (mesh.nodeIndices.size() > 0)
	{
		for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
		{
			int nodeIndex = mesh.nodeIndices.at(i);
			const Model::Node& node = model->GetNodes().at(nodeIndex);
			DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
			DirectX::XMMATRIX OffsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
			DirectX::XMMATRIX BoneTransform = OffsetTransform * GlobalTransform * worldTransform;
			DirectX::XMStoreFloat4x4(&cbSkeleton.boneTransforms[i], BoneTransform);
		}
	}
	else
	{
		// 単一ノードのメッシュ
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
		DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
		DirectX::XMMATRIX BoneTransform = GlobalTransform * worldTransform;
		DirectX::XMStoreFloat4x4(&cbSkeleton.boneTransforms[0], BoneTransform);
	}
	dc->UpdateSubresource(skeletonConstantBuffer.Get(), 0, 0, &cbSkeleton, 0, 0);

	// 描画
	for (const ModelResource::Subset& subset : mesh.subsets)
	{
		shader->Update(rc, *subset.material);

		dc->DrawIndexed(subset.indexCount, subset.startIndex, 0);
	}
}

void ModelRenderer::SetConstantBuffers(ID3D11DeviceContext* dc)
{
	ID3D11Buffer* vsConstantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		skeletonConstantBuffer.Get(),
		light_constant_buffer.Get(),
		shadowMapConstantBuffer.Get(),
	};
	dc->VSSetConstantBuffers(0, _countof(vsConstantBuffers), vsConstantBuffers);

	ID3D11Buffer* psConstantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		skeletonConstantBuffer.Get(),
		light_constant_buffer.Get(),
		shadowMapConstantBuffer.Get(),
	};
	dc->PSSetConstantBuffers(0, _countof(psConstantBuffers), psConstantBuffers);
}

void ModelRenderer::SetSamplers(ID3D11DeviceContext* dc, const RenderContext& rc)
{
	ID3D11SamplerState* samplerStates[] =
	{
		rc.renderState->GetSamplerState(SamplerState::LinearWrap),
		rc.renderState->GetSamplerState(SamplerState::PointClamp),
		rc.renderState->GetSamplerState(SamplerState::LinearWrap),
		rc.renderState->GetSamplerState(SamplerState::LinearClamp),
		rc.renderState->GetSamplerState(SamplerState::ShadowSampler),
	};
	dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);
}

void ModelRenderer::DrawModel(
	const RenderContext& rc,
	ID3D11DeviceContext* dc, 
	const DirectX::XMFLOAT4X4& worldTransform, 
	const Model* model, 
	ShaderId shaderId)
{
	// 深度・ラスタ・ブレンド設定
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Transparency), nullptr, 0xFFFFFFFF);

	// 描画処理
	Shader* shader = shaders[static_cast<int>(shaderId)].get();
	shader->Begin(rc);

	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);
	const ModelResource* resource = model->GetResource();

	// メッシュ単位で描画
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		DrawMesh(rc, dc, mesh, model, WorldTransform, shader);
	}

	shader->End(rc);
}

void ModelRenderer::ClearBinding(ID3D11DeviceContext* dc)
{
	// 定数バッファ解除
	ID3D11Buffer* nullCBs[4] = { nullptr };
	dc->VSSetConstantBuffers(0, 4, nullCBs);
	dc->PSSetConstantBuffers(0, 4, nullCBs);

	// サンプラー解除
	ID3D11SamplerState* nullSamplers[5] = { nullptr };
	dc->PSSetSamplers(0, 5, nullSamplers);
}

void ModelRenderer::UpdateLightSwitch()
{
	if (horrorPhase < 0) return;

	// フレームカウント
	horrorFrame++;
	

	// ---- フェーズ0：全体ライトがチカチカ ----
	if (horrorPhase == 0)
	{
		if (horrorFrame < 300)
		{
			// ---- (1) 高速チカチカ ----
			int cycle = horrorFrame % 8;
			if (cycle < 3)
				sceneConstantBufferData.LightSwitch = 1.0f; // ON
			else
				sceneConstantBufferData.LightSwitch = 0.0f; // OFF
		}
		else if (horrorFrame < 450)
		{
			// ---- (2) 不規則チカチカ ----
			// ランダムで暗くなる明るくなるを繰り返す
			int r = rand() % 100;

			if (r < 10)
			{
				// 完全OFF（バチッと消える）
				sceneConstantBufferData.LightSwitch = 1.0f;
			}
			else if (r < 40)
			{
				// 弱い点灯（ちらっと光る）
				sceneConstantBufferData.LightSwitch = 0.7f;
			}
			else
			{
				// 通常ON
				sceneConstantBufferData.LightSwitch = 0.0f;
			}
		}
		else
		{
			// ---- (3) 最後のバチッ ----
			sceneConstantBufferData.LightSwitch = 1.0f;
			

			// 少しだけ暗闇を維持したあと次へ
			if (horrorFrame > 500)
			{
				
				horrorPhase = 1;
				horrorFrame = 0;
			}
		}
	}

	// ---- フェーズ1：真っ暗 ----
	else if (horrorPhase == 1)
	{
		sceneConstantBufferData.LightSwitch = 1.0f; // 

		// 0.7秒 → 約42フレーム
		if (horrorFrame > 570)
		{
			horrorPhase = 2;
			horrorFrame = 0;
		}
	}

	// ---- フェーズ2：スポットライトON ----
	else if (horrorPhase == 2)
	{
		sceneConstantBufferData.LightSwitch = 1.0f;
		sceneConstantBufferData.SpotLightSwitch = 0.0f;

		// 以降ずっとスポットライト
		horrorPhase = 3;
	}

	else if (horrorPhase >= 3)
	{
		static int SpotLightFlickerFrames = 0;
		const int MaxFlickerFrames = 100;

		// 基本点灯（HLSL側で0.0 = 最大点灯）
		sceneConstantBufferData.SpotLightSwitch = 0.0f;

		if (SpotLightFlickerFrames < MaxFlickerFrames)
		{
			// 最初の数フレームだけチカチカ
			sceneConstantBufferData.SpotLightSwitch = 0.2f + static_cast<float>(rand() % 30) / 100.0f; // 0.20?0.49
			SpotLightFlickerFrames++;
		}
	}
}

void ModelRenderer::UpdataLightsSS()
{
	float dt = 1.0f / 144.0f;

	static float intensity = 0.9f;
	static float flickerTimer = 0.0f;
	static int state = 2; // 0=OFF, 1=弱, 2=Normal
	static std::mt19937 mt(std::random_device{}());
	static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	flickerTimer -= dt;

	if (flickerTimer <= 0.0f)
	{
		float r = dist(mt);
		if (r < 0.05f) {
			state = 0;  // 一瞬OFF
			flickerTimer = 0.1f + dist(mt) * 0.2f;
		}
		else if (r < 0.2f) {
			state = 1;  // 弱め
			flickerTimer = 0.05f + dist(mt) * 0.3f;
		}
		else {
			state = 2;  // 通常
			flickerTimer = 0.2f + dist(mt) * 0.5f;
		}
	}

	float targetIntensity = (state == 0) ? 0.2f : (state == 1) ? 0.5f :0.9f;

	// 滑らか補間
	float lerpSpeed = 3.0f;
	intensity += (targetIntensity - intensity) * lerpSpeed * dt;

	// 微小ノイズ
	float noise = (dist(mt) - 0.5f) * 0.1f;
	float finalIntensity = std::clamp(intensity + noise, 0.0f, 1.0f);

	spot_light[0].color.w = finalIntensity;
}
