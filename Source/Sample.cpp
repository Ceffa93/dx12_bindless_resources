#include "Sample.h"
#include "Renderer.h"
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

    ComPtr<ID3D12Resource> Create2DTexture(UINT textureWidth, UINT textureHeight, ID3D12Device* device, DescriptorManager& descriptorManager)
    {
        D3D12_RESOURCE_DESC desc = {};
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Width = textureWidth;
        desc.Height = textureHeight;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        desc.DepthOrArraySize = 1;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        ComPtr<ID3D12Resource> texture;
        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            nullptr,
            IID_PPV_ARGS(&texture)));

        descriptorManager.allocateTexture2DUavDescriptor(texture.Get(), desc.Format);
        descriptorManager.allocateTexture2DSrvDescriptor(texture.Get(), desc.Format);
        return texture;
    }

    ComPtr<ID3D12Resource> Create3DTexture(UINT textureWidth, UINT textureHeight, UINT textureDepth, ID3D12Device* device, DescriptorManager& descriptorManager)
    {
        D3D12_RESOURCE_DESC desc = {};
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Width = textureWidth;
        desc.Height = textureHeight;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        desc.DepthOrArraySize = textureDepth;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;

        ComPtr<ID3D12Resource> texture;
        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            nullptr,
            IID_PPV_ARGS(&texture)));

        descriptorManager.allocateTexture3DUavDescriptor(texture.Get(), desc.Format);
        descriptorManager.allocateTexture3DSrvDescriptor(texture.Get(), desc.Format);
        return texture;
    }
}


Sample::Sample(Renderer& renderer)
    : m_renderer(renderer)
    , m_descriptorManager(m_renderer.m_device.Get())
    , m_graphicPipelineState(CreateGraphicPipelineState(m_renderer.m_device.Get(), m_renderer.m_assetPath, m_descriptorManager))
    , m_computePipelineState(CreateComputePipelineState(m_renderer.m_device.Get(), m_renderer.m_assetPath, m_descriptorManager))
    , m_2DTexture(Create2DTexture(TextureWidth, TextureHeight, m_renderer.m_device.Get(), m_descriptorManager))
    , m_3DTexture(Create3DTexture(TextureWidth, TextureHeight, TextureDepth, m_renderer.m_device.Get(), m_descriptorManager))
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
    m_descriptorManager.allocateSamplerDescriptor(samplerDesc);
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
        commandList->Dispatch(1, 1, 1);
    }

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_2DTexture.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_3DTexture.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    {
        commandList->SetPipelineState(m_graphicPipelineState.Get());
        m_descriptorManager.setSignature(commandList, false);
        m_descriptorManager.setTables(commandList, false);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawInstanced(3, 1, 0, 0);

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_2DTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_3DTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

    }
}
