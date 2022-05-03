#pragma once
#include "DescriptorManager.h"
#include "Descriptor.h"
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
    ComPtr<ID3D12Resource> m_buffer;

    Descriptor_RWTexture2D_float4 m_2D_uav;
    Descriptor_RWTexture3D_float4 m_3D_uav;
    Descriptor_Texture3D m_3D_srv;
    Descriptor_Texture2D m_2D_srv;
    Descriptor_SamplerState m_sv;
};
