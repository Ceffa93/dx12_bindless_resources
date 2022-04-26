#pragma once
#include "DescriptorManager.h"
#include <External/d3dx12.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using Microsoft::WRL::ComPtr;

class Renderer;

class Sample
{
public:
    Sample(Renderer& renderer);

    void OnUpdate();
    void OnRender(ID3D12GraphicsCommandList*);

private:
    static const UINT TextureWidth = 8;
    static const UINT TextureHeight = 8;
    static const UINT TextureDepth = 2;

    Renderer& m_renderer;

    DescriptorManager m_descriptorManager;
    ComPtr<ID3D12PipelineState> m_graphicPipelineState;
    ComPtr<ID3D12PipelineState> m_computePipelineState;
    ComPtr<ID3D12Resource> m_2DTexture;
    ComPtr<ID3D12Resource> m_3DTexture;
};
