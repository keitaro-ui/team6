#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <cstdint>

class ShadowMap
{
public:
    ShadowMap(ID3D11Device* device, uint32_t width, uint32_t height);

    void Clear(ID3D11DeviceContext* immediate_context, float depth = 1.0f);
    void Activate(ID3D11DeviceContext* immediate_context);
    void Deactive(ID3D11DeviceContext* immediate_context);

public:
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowResourceView;
    D3D11_VIEWPORT viewport{};

private:
    UINT viewportCount = 0;
    D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView; 
};
