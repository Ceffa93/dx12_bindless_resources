#include "Sample.h"
#include "Renderer.h"
#include <Shared/Flags.h>
#include <External/DXSampleHelper.h>
#include <D3Dcompiler.h>

namespace
{
    ComPtr<ID3D12PipelineState> CreateComputePipelineState(ID3D12Device* device, const std::wstring& assetPath, DescriptorManager& descriptorManager)
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

    ComPtr<ID3D12PipelineState> CreateGraphicPipelineState(ID3D12Device* device, const std::wstring& assetPath, DescriptorManager& descriptorManager)
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
    , m_descriptorManager(m_renderer.m_device.Get())
    , m_graphicPipelineState(CreateGraphicPipelineState(m_renderer.m_device.Get(), m_renderer.m_assetPath, m_descriptorManager))
    , m_computePipelineState(CreateComputePipelineState(m_renderer.m_device.Get(), m_renderer.m_assetPath, m_descriptorManager))
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
            D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            desc.Format = format;
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipLevels = 1;
            m_2D_srv = m_descriptorManager.allocateTexture2DSrvDescriptor(m_2DTexture.Get(), desc);
        }
        {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            m_2D_uav = m_descriptorManager.allocateTexture2DUavDescriptor(m_2DTexture.Get(), desc);
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
            D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            desc.Format = format;
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
            desc.Texture3D.MipLevels = 1;
            m_3D_srv = m_descriptorManager.allocateTexture3DSrvDescriptor(m_3DTexture.Get(), desc);
        }
        {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
            desc.Texture3D.WSize = -1;
            m_3D_uav = m_descriptorManager.allocateTexture3DUavDescriptor(m_3DTexture.Get(), desc);
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
        m_sv = m_descriptorManager.allocateSamplerDescriptor(samplerDesc);
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
        flags.srv2D = DescriptorHandle_Texture2D{ m_2D_srv };
        flags.uav2D = DescriptorHandle_RWTexture2D_float4{ m_2D_uav };
        flags.srv3D = DescriptorHandle_Texture3D{ m_3D_srv };
        flags.uav3D = DescriptorHandle_RWTexture3D_float4{ m_3D_uav };
        flags.sampler = DescriptorHandle_SamplerState{ m_sv };

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
    m_descriptorManager.setHeaps(commandList);

    {
        commandList->SetPipelineState(m_computePipelineState.Get());
        m_descriptorManager.setSignature(commandList, true);
        m_descriptorManager.setTables(commandList, true);
        m_descriptorManager.setRootCbv(commandList, 0, m_buffer.Get(), true);
        commandList->Dispatch(1, 1, 1);
    }

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_2DTexture.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_3DTexture.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    {
        commandList->SetPipelineState(m_graphicPipelineState.Get());
        m_descriptorManager.setSignature(commandList, false);
        m_descriptorManager.setTables(commandList, false);
        m_descriptorManager.setRootCbv(commandList, 0, m_buffer.Get(), false);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawInstanced(3, 1, 0, 0);

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_2DTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_3DTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

    }
}
