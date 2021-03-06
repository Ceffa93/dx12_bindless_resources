#include "Sample.h"
#include "Renderer.h"
#include <Shared/Flags.h>
#include <External/DXSampleHelper.h>
#include <D3Dcompiler.h>

namespace
{
    ComPtr<ID3D12PipelineState> CreateComputePipelineState(ID3D12Device* device, const std::wstring& assetPath, Device& descriptorManager)
    {
        UINT8* shaderData;
        UINT shaderDataLength;
        ThrowIfFailed(ReadDataFromFile((assetPath + L"cs.cso").c_str(), &shaderData, &shaderDataLength));

        D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = descriptorManager.m_rootSignature.Get();
        psoDesc.CS = CD3DX12_SHADER_BYTECODE(shaderData, shaderDataLength);
        ComPtr<ID3D12PipelineState> pipelineState;
        ThrowIfFailed(device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));

        delete shaderData;
        return pipelineState;
    }

    ComPtr<ID3D12PipelineState> CreateGraphicPipelineState(ID3D12Device* device, const std::wstring& assetPath, Device& descriptorManager)
    {
        UINT8* pVertexShaderData;
        UINT8* pPixelShaderData;
        UINT vertexShaderDataLength;
        UINT pixelShaderDataLength;

        ThrowIfFailed(ReadDataFromFile((assetPath + L"vs.cso").c_str(), &pVertexShaderData, &vertexShaderDataLength));
        ThrowIfFailed(ReadDataFromFile((assetPath + L"ps.cso").c_str(), &pPixelShaderData, &pixelShaderDataLength));

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = descriptorManager.m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(pVertexShaderData, vertexShaderDataLength);
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pPixelShaderData, pixelShaderDataLength);
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        ComPtr<ID3D12PipelineState> pipelineState;
        ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));

        delete pVertexShaderData;
        delete pPixelShaderData;
        return pipelineState;
    }
}


Sample::Sample(Renderer& renderer)
    : m_renderer(renderer)
    , m_device(m_renderer.m_device.Get())
    , m_graphicPipelineState(CreateGraphicPipelineState(m_renderer.m_device.Get(), m_renderer.m_assetPath, m_device))
    , m_computePipelineState(CreateComputePipelineState(m_renderer.m_device.Get(), m_renderer.m_assetPath, m_device))
    , m_2D_srv(m_device)
    , m_2D_uav(m_device)
    , m_3D_srv(m_device)
    , m_3D_uav(m_device)
    , m_sv(m_device)
{
    {
        auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
        {
            D3D12_RESOURCE_DESC desc = {};
            desc.MipLevels = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.Width = TextureWidth;
            desc.Height = TextureHeight;
            desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            desc.DepthOrArraySize = 1;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            ThrowIfFailed(m_renderer.m_device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &desc,
                D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                nullptr,
                IID_PPV_ARGS(&m_2DTexture)));
        }
        {
            D3D12_TEX2D_SRV desc{};
            desc.MipLevels = 1;
            m_2D_srv.set(m_2DTexture.Get(), format, D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING, desc);
        }
        {
            D3D12_TEX2D_UAV desc{};
            m_2D_uav.set(m_2DTexture.Get(), format, desc);
        }
    }
    {
        auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
        {
            D3D12_RESOURCE_DESC desc = {};
            desc.MipLevels = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.Width = TextureWidth;
            desc.Height = TextureHeight;
            desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            desc.DepthOrArraySize = TextureDepth;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;

            ThrowIfFailed(m_renderer.m_device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &desc,
                D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                nullptr,
                IID_PPV_ARGS(&m_3DTexture)));
        }
        {
            D3D12_TEX3D_SRV desc{};
            desc.MipLevels = 1;
            m_3D_srv.set(m_3DTexture.Get(), format, D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING, desc);
        }
        {
            D3D12_TEX3D_UAV desc{};
            desc.WSize = -1;
            m_3D_uav.set(m_3DTexture.Get(), format, desc);
        }
    }
    {
        D3D12_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc.MipLODBias = 0;
        samplerDesc.MaxAnisotropy = 0;
        samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc.MinLOD = 0.0f;
        samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
        m_sv.set(samplerDesc);
    }
    {
        ThrowIfFailed(m_renderer.m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(sizeof(Flags)),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_buffer)));
    }
    
    {
        CD3DX12_RANGE readRange(0, 0);
        Flags flags;
        flags.srv2D = m_2D_srv.get();
        flags.uav2D = m_2D_uav.get();
        flags.srv3D = m_3D_srv.get();
        flags.uav3D = m_3D_uav.get();
        flags.sampler = m_sv.get();

        constexpr int t = sizeof(flags);

        UINT8* cbvData;
        ThrowIfFailed(m_buffer->Map(0, &readRange, reinterpret_cast<void**>(&cbvData)));
        memcpy(cbvData, &flags, sizeof(flags));
        m_buffer->Unmap(0, &readRange);
    }
}

void Sample::OnUpdate()
{
}

void Sample::OnRender(ID3D12GraphicsCommandList* commandList)
{
    m_device.setHeaps(commandList);

    {
        commandList->SetPipelineState(m_computePipelineState.Get());
        m_device.setSignature(commandList, true);
        m_device.setTables(commandList, true);
        m_device.setRootCbv(commandList, 0, m_buffer.Get(), true);
        commandList->Dispatch(1, 1, 1);
    }

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_2DTexture.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_3DTexture.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    {
        commandList->SetPipelineState(m_graphicPipelineState.Get());
        m_device.setSignature(commandList, false);
        m_device.setTables(commandList, false);
        m_device.setRootCbv(commandList, 0, m_buffer.Get(), false);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawInstanced(3, 1, 0, 0);

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_2DTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_3DTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

    }
}
