#include <stdlib.h>
#include <fstream>
#include <functional>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include "Misc.h"
#include "GpuResourceUtils.h"
#include "ModelResource.h"

const std::vector<D3D11_INPUT_ELEMENT_DESC> ModelResource::InputElementDescs =
{
	{ "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BINORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",        0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BONE_WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BONE_INDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// CEREALバージョン定義
CEREAL_CLASS_VERSION(ModelResource::Node, 1)
CEREAL_CLASS_VERSION(ModelResource::Material, 1)
CEREAL_CLASS_VERSION(ModelResource::Subset, 1)
CEREAL_CLASS_VERSION(ModelResource::Vertex, 2)
CEREAL_CLASS_VERSION(ModelResource::Mesh, 1)
CEREAL_CLASS_VERSION(ModelResource::NodeKeyData, 1)
CEREAL_CLASS_VERSION(ModelResource::Keyframe, 1)
CEREAL_CLASS_VERSION(ModelResource::Animation, 1)
CEREAL_CLASS_VERSION(ModelResource, 1)


static void ComputeTangentsForMesh(ModelResource::Mesh& mesh)
{
	// まずゼロ初期化（累積用）
	for (auto& v : mesh.vertices)
	{
		v.tangent = DirectX::XMFLOAT3(0, 0, 0);
		v.bitangent = DirectX::XMFLOAT3(0, 0, 0);
	}

	// インデックスはトライアングル前提（逆順やストリップなら調整）
	for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3)
	{
		uint32_t i0 = mesh.indices[i + 0];
		uint32_t i1 = mesh.indices[i + 1];
		uint32_t i2 = mesh.indices[i + 2];

		auto& v0 = mesh.vertices[i0];
		auto& v1 = mesh.vertices[i1];
		auto& v2 = mesh.vertices[i2];

		DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&v0.position);
		DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&v1.position);
		DirectX::XMVECTOR p2 = DirectX::XMLoadFloat3(&v2.position);

		DirectX::XMVECTOR uv0 = DirectX::XMLoadFloat2(&v0.texcoord);
		DirectX::XMVECTOR uv1 = DirectX::XMLoadFloat2(&v1.texcoord);
		DirectX::XMVECTOR uv2 = DirectX::XMLoadFloat2(&v2.texcoord);

		DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(p1, p0);
		DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(p2, p0);

		float x1 = DirectX::XMVectorGetX(DirectX::XMVectorSubtract(uv1, uv0));
		float y1 = DirectX::XMVectorGetY(DirectX::XMVectorSubtract(uv1, uv0));
		float x2 = DirectX::XMVectorGetX(DirectX::XMVectorSubtract(uv2, uv0));
		float y2 = DirectX::XMVectorGetY(DirectX::XMVectorSubtract(uv2, uv0));

		float denom = (x1 * y2 - x2 * y1);
		float r = denom == 0.0f ? 0.0f : 1.0f / denom;

		DirectX::XMVECTOR tangent = DirectX::XMVectorScale(
			DirectX::XMVectorSubtract(
				DirectX::XMVectorScale(edge1, y2),
				DirectX::XMVectorScale(edge2, y1)
			), r);

		DirectX::XMVECTOR bitangent = DirectX::XMVectorScale(
			DirectX::XMVectorSubtract(
				DirectX::XMVectorScale(edge2, x1),
				DirectX::XMVectorScale(edge1, x2)
			), r);

		// 加算（累積）
		DirectX::XMFLOAT3 ttmp, btmp;
		DirectX::XMStoreFloat3(&ttmp, tangent);
		DirectX::XMStoreFloat3(&btmp, bitangent);

		v0.tangent.x += ttmp.x; v0.tangent.y += ttmp.y; v0.tangent.z += ttmp.z;
		v1.tangent.x += ttmp.x; v1.tangent.y += ttmp.y; v1.tangent.z += ttmp.z;
		v2.tangent.x += ttmp.x; v2.tangent.y += ttmp.y; v2.tangent.z += ttmp.z;

		v0.bitangent.x += btmp.x; v0.bitangent.y += btmp.y; v0.bitangent.z += btmp.z;
		v1.bitangent.x += btmp.x; v1.bitangent.y += btmp.y; v1.bitangent.z += btmp.z;
		v2.bitangent.x += btmp.x; v2.bitangent.y += btmp.y; v2.bitangent.z += btmp.z;
	}

	// 正規化して、法線と直交化（Gram-Schmidt）
	for (auto& v : mesh.vertices)
	{
		DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&v.normal);
		DirectX::XMVECTOR t = DirectX::XMLoadFloat3(&v.tangent);
		// t = normalize(t - n * dot(n, t))
		DirectX::XMVECTOR proj = DirectX::XMVectorScale(n, DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, t)));
		DirectX::XMVECTOR orthT = DirectX::XMVectorSubtract(t, proj);
		orthT = DirectX::XMVector3Normalize(orthT);

		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&v.bitangent);
		// bitangent can be recomputed as cross(normal, tangent) to be consistent
		DirectX::XMVECTOR recomputedB = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(n, orthT));

		DirectX::XMStoreFloat3(&v.tangent, orthT);
		DirectX::XMStoreFloat3(&v.bitangent, recomputedB);
	}
}
// シリアライズ
namespace DirectX
{
	template<class Archive>
	void serialize(Archive& archive, XMUINT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}

template<class Archive>
void ModelResource::Node::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(id),
		CEREAL_NVP(name),
		CEREAL_NVP(path),
		CEREAL_NVP(parentIndex),
		CEREAL_NVP(scale),
		CEREAL_NVP(rotate),
		CEREAL_NVP(translate)
	);
}

template<class Archive>
void ModelResource::Material::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(name),
		CEREAL_NVP(textureFilename),
		CEREAL_NVP(color)
	);
}

template<class Archive>
void ModelResource::Subset::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(startIndex),
		CEREAL_NVP(indexCount),
		CEREAL_NVP(materialIndex)
	);
}

template<class Archive>
void ModelResource::Vertex::serialize(Archive& archive, int version)
{
	/*archive(
		CEREAL_NVP(position),
		CEREAL_NVP(normal),
		CEREAL_NVP(tangent),
		CEREAL_NVP(bitangent),
		CEREAL_NVP(texcoord),
		CEREAL_NVP(color),
		CEREAL_NVP(boneWeight),
		CEREAL_NVP(boneIndex)
	);*/

	archive(
		CEREAL_NVP(position),
		CEREAL_NVP(normal),
		CEREAL_NVP(tangent)
	);

	if (version >= 2)
		archive(CEREAL_NVP(bitangent));

	archive(
		CEREAL_NVP(texcoord),
		CEREAL_NVP(color),
		CEREAL_NVP(boneWeight),
		CEREAL_NVP(boneIndex)
	);
}

template<class Archive>
void ModelResource::Mesh::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(vertices),
		CEREAL_NVP(indices),
		CEREAL_NVP(subsets),
		CEREAL_NVP(nodeIndex),
		CEREAL_NVP(nodeIndices),
		CEREAL_NVP(offsetTransforms),
		CEREAL_NVP(boundsMin),
		CEREAL_NVP(boundsMax)
	);
}

template<class Archive>
void ModelResource::NodeKeyData::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(scale),
		CEREAL_NVP(rotate),
		CEREAL_NVP(translate)
	);
}

template<class Archive>
void ModelResource::Keyframe::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(seconds),
		CEREAL_NVP(nodeKeys)
	);
}

template<class Archive>
void ModelResource::Animation::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(name),
		CEREAL_NVP(secondsLength),
		CEREAL_NVP(keyframes)
	);
}

// 読み込み
void ModelResource::Load(ID3D11Device* device, const char* filename)
{
	// ディレクトリパス取得
	char drive[32], dir[256], dirname[256];
	::_splitpath_s(filename, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
	::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);

	// デシリアライズ
	Deserialize(filename);

	// モデル構築
	BuildModel(device, dirname);
}

// モデル構築
void ModelResource::BuildModel(ID3D11Device* device, const char* dirname)
{
	for (Material& material : materials)
	{
		// 相対パスの解決
		char filename[256];
		::_makepath_s(filename, 256, nullptr, dirname, material.textureFilename.c_str(), nullptr);

		// マルチバイト文字からワイド文字へ変換
		wchar_t wfilename[256];
		::MultiByteToWideChar(CP_ACP, 0, filename, -1, wfilename, 256);

		// テクスチャ読み込み
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		HRESULT hr = GpuResourceUtils::LoadTexture(device, filename, material.shaderResourceView.GetAddressOf());
		if (FAILED(hr))
		{
			hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFFFFFF, material.shaderResourceView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
	}

	for (Mesh& mesh : meshes)
	{
		// サブセット
		for (Subset& subset : mesh.subsets)
		{
			subset.material = &materials.at(subset.materialIndex);
		}

		ComputeTangentsForMesh(mesh);

		// 頂点バッファ
		{
			D3D11_BUFFER_DESC bufferDesc = {};
			D3D11_SUBRESOURCE_DATA subresourceData = {};

			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
			//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			subresourceData.pSysMem = mesh.vertices.data();
			subresourceData.SysMemPitch = 0;
			subresourceData.SysMemSlicePitch = 0;

			HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// インデックスバッファ
		{
			D3D11_BUFFER_DESC bufferDesc = {};
			D3D11_SUBRESOURCE_DATA subresourceData = {};

			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * mesh.indices.size());
			//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			subresourceData.pSysMem = mesh.indices.data();
			subresourceData.SysMemPitch = 0; //Not use for index buffers.
			subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
			HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
	}
}

// シリアライズ
void ModelResource::Serialize(const char* filename)
{
	std::ofstream ostream(filename, std::ios::binary);
	if (ostream.is_open())
	{
		cereal::BinaryOutputArchive archive(ostream);

		try
		{
			archive(
				CEREAL_NVP(nodes),
				CEREAL_NVP(materials),
				CEREAL_NVP(meshes),
				CEREAL_NVP(animations)
			);
		}
		catch (...)
		{
			char buffer[256];
			sprintf_s(buffer, sizeof(buffer), "model serialize failed.\n%s\n", filename);
			_ASSERT_EXPR_A(false, buffer);
			return;
		}
	}
}

// デシリアライズ
void ModelResource::Deserialize(const char* filename)
{
	std::ifstream istream(filename, std::ios::binary);
	if (istream.is_open())
	{
		cereal::BinaryInputArchive archive(istream);

		try
		{
			archive(
				CEREAL_NVP(nodes),
				CEREAL_NVP(materials),
				CEREAL_NVP(meshes),
				CEREAL_NVP(animations)
			);
		}
		catch (...)
		{
			char buffer[256];
			sprintf_s(buffer, sizeof(buffer), "model deserialize failed.\n%s\n", filename);
			_ASSERT_EXPR_A(false, buffer);
			return;
		}
	}
	else
	{
		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), "File not found > %s", filename);
		_ASSERT_EXPR_A(false, buffer);
	}
}