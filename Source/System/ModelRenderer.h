#pragma once

#include <memory>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Model.h"
#include "Shader.h"
#include "ShapeRenderer.h"
#include <random>



enum class ShaderId
{
	Basic,
	Lambert,
	Phong,
	Shadow,
	EnumCount
};

class ModelRenderer
{

public:
	ModelRenderer(ID3D11Device* device);
	~ModelRenderer() {};

	// ■ ライト初期化（色・位置・方向などの初期設定）
	void InitLights();

	// ■ モデル描画（メインの描画エントリ）
	//    worldTransform：モデルのワールド行列
	//    model：描画するモデル
	//    shaderId：使用するシェーダ
	void Render(const RenderContext& rc,
		const DirectX::XMFLOAT4X4& worldTransform,
		const Model* model,
		ShaderId shaderId);

	// ■ ImGui パネル描画（ライト調整など）
	void RenderImGui(const RenderContext& rc);

	// ■ スポットライトを常にカメラ方向へ向ける
	void UpdateSpotLightFromCamera();

	// ■ ライトの定数バッファ更新
	void UpdateLightConstantBuffer(ID3D11DeviceContext* dc);

	// ■ シーン共通情報（VP行列・カメラ位置など）の更新
	void UpdateSceneConstantBuffer(ID3D11DeviceContext* dc, const RenderContext& rc);

	// ■ メッシュ単位の描画
	void DrawMesh(
		const RenderContext& rc,
		ID3D11DeviceContext* dc,
		const ModelResource::Mesh& mesh,
		const Model* model,
		const DirectX::XMMATRIX& WorldTransform,
		Shader* shader);

	// ■ 共通の定数バッファを各ステージへセット
	void SetConstantBuffers(ID3D11DeviceContext* dc);

	// ■ サンプラ（線形補間など）をセット
	void SetSamplers(ID3D11DeviceContext* dc, const RenderContext& rc);

	// ■ モデル全体の描画（Mesh → Subset をループ）
	void DrawModel(
		const RenderContext& rc,
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& worldTransform,
		const Model* model,
		ShaderId shaderId);

	// ■ 描画後にバインドされたステートを解除
	//    （不透明 → 透明の順で描画するときに安全）
	void ClearBinding(ID3D11DeviceContext* dc);
	
	//遊び
	void UpdataLightsSS();
private:
	// VP行列・カメラ位置・ライト方向など
	struct CbScene
	{
		DirectX::XMFLOAT4X4		viewProjection;		//ビュー × プロジェクション
		DirectX::XMFLOAT4		lightDirection;		// 平行光源の方向
		DirectX::XMFLOAT4		cameraPosition;		// カメラ座標
		float lightingMultiplier{ 0.0f };			// ライト強度調整
		float useLighting{ 1.0f };					// ライトON/OFF用
		float padding[2];							// 16バイト境界揃え
	};
	CbScene sceneConstantBufferData;

	// スケルトンアニメーション用の全ボーン行列
	struct CbSkeleton
	{
		DirectX::XMFLOAT4X4		boneTransforms[256];
	};

	// シャドウマップ用定数バッファ
	struct ShadowMapConstants
	{
		DirectX::XMFLOAT4 shadowColor;
		float shadowBias;
		DirectX::XMFLOAT3 padding;
	};

	//ポイントライト
	struct point_lights
	{
		DirectX::XMFLOAT4 position = { 0,0,0,0 };
		DirectX::XMFLOAT4 color = { 1,1,1,1 };
		float range{ 0 };
		DirectX::XMFLOAT3 dummy;
	};

	//スポットライト
	struct spot_lights
	{
		DirectX::XMFLOAT4 position{ 0,0,0,0 };
		DirectX::XMFLOAT4 direction{ 0,0,1,0 };
		DirectX::XMFLOAT4 color = { 1,1,1,1 };
		float range{ 0 };
		float innerCorn{ 0.99f };
		float outerCorn{ 0.9f };
		float dummy;
	};

	//複数ライトをまとめた GPU 送信用構造体
	struct light_constants
	{
		DirectX::XMFLOAT4 ambient_color;
		DirectX::XMFLOAT4 directional_light_direction;
		DirectX::XMFLOAT4 directional_light_color;
		point_lights point_light[16];
		spot_lights spot_light[8];
	};

	struct scene_constants
	{
		DirectX::XMFLOAT4 view_projection;
		DirectX::XMFLOAT4 options;
		DirectX::XMFLOAT4 camera_position;
	};

	// ■ ライトとシーンのパラメータ
	// 環境光
	DirectX::XMFLOAT4 ambient_color = { 0.3f,0.2f,0.2f,0.2f };
	// ディレクショナルライトの方向ベクトル
	DirectX::XMFLOAT4 directional_light_direction{ 0.0f,-1.0f,1.0f,1.0f };
	// ディレクショナルライトの色
	DirectX::XMFLOAT4 directional_light_color{ 1.0f,1.0f,1.0f,1.0f };
	// ポイントライトの配列
	point_lights point_light[16];
	// スポットライトの配列
	spot_lights spot_light[8];

	//======================================================================
	// ■ DirectX 定数バッファ
	//======================================================================

	// VP行列 / カメラ / ライト方向など
	Microsoft::WRL::ComPtr<ID3D11Buffer>	sceneConstantBuffer;
	// ボーン行列
	Microsoft::WRL::ComPtr<ID3D11Buffer>	skeletonConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> scene_constants_buffer;
	// ライト（ポイント / スポット）
	Microsoft::WRL::ComPtr<ID3D11Buffer> light_constant_buffer;
	//シャドウマップ
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowMapConstantBuffer;

	//======================================================================
	// ■ シェーダ / デバッグ描画
	//======================================================================
	std::unique_ptr<Shader>					shaders[static_cast<int>(ShaderId::EnumCount)];
	std::unique_ptr<ShapeRenderer> debugRenderer;

	//======================================================================
	// ■ カメラ関連
	//======================================================================
	DirectX::XMFLOAT3 camera_position{ 0.0f, 0.0f, -10.0f };

	float timer{ 0.0f };
	bool flag{ false };
	
	// ディレクショナルライトのユーザー調整用（ImGui）
	float dirLightYaw = -10.0f;
	float dirLightPitch = 2.0f;

	// カーソル記録
	POINT cursor_position;

	float time = 0;
	std::mt19937 mt;

	std::uniform_real_distribution<float> dist;

public:
	// ポイントライト設定
	void SetPointLight(int index, const point_lights& light);
	// 環境光を設定
	void SetAmbientColor(const DirectX::XMFLOAT4& color);

	void UpdateSafetyAreaLights(const std::vector<DirectX::XMFLOAT4>& position);
};
