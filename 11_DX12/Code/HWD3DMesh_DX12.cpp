// D3D Hello World

#include "HWD3DMesh_DX12.h"
#include "HWD3DGame_DX12.h"

HWD3DMesh* HWD3DMesh::CreateMesh(class HWD3DGame* InGame, const char* InFilename)
{
	return new HWD3DMesh_DX12(dynamic_cast<HWD3DGame_DX12*>(InGame), InFilename);
}

void HWD3DMesh_DX12::Draw()
{
	if (m_Game && m_Game->GetCommandList() && m_IBView.IsValid() && m_VBView.IsValid())
	{
		const D3D12_VERTEX_BUFFER_VIEW VbViews[] = 
		{
			{ m_VBView.GpuVirtualPtr , m_VBView.Size , sizeof(hwd3d_vertex) },
		};
		m_Game->GetCommandList()->IASetVertexBuffers(0, _countof(VbViews), VbViews);
		D3D12_INDEX_BUFFER_VIEW IbView = { m_IBView.GpuVirtualPtr , m_IBView.Size , DXGI_FORMAT_R16_UINT };
		m_Game->GetCommandList()->IASetIndexBuffer(&IbView);
		m_Game->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_Game->GetCommandList()->DrawIndexedInstanced(m_Triangles.size()*3, 1, 0, 0, 0);
	}
}

HWD3DMesh_DX12::HWD3DMesh_DX12(class HWD3DGame_DX12* InGame, const char* InFilename)
	: m_Game(InGame)
{
	LoadMeshFile(InFilename);
	CreateBuffers();
}

HWD3DMesh_DX12::~HWD3DMesh_DX12()
{
	if (m_Game)
	{
		m_Game->GetBufferViewProvider().DestroyView(m_IBView);
		m_Game->GetBufferViewProvider().DestroyView(m_VBView);
	}

	HWD3D_SafeRelease(m_IB);
	HWD3D_SafeRelease(m_VB);
}

bool HWD3DMesh_DX12::CreateBuffers()
{
	ID3D12Device* Dev = m_Game ? m_Game->GetDevice() : nullptr;

	if (!Dev)
	{
		return false;
	}

	// const D3D12_USAGE Usage = D3D12_USAGE_IMMUTABLE;
	

	// Fill Vertex Buffer
	{
		const D3D12_HEAP_PROPERTIES HeapProps = { D3D12_HEAP_TYPE_DEFAULT , D3D12_CPU_PAGE_PROPERTY_UNKNOWN , D3D12_MEMORY_POOL_UNKNOWN, 0x1 , 0x1 };
		
		const D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE;

		D3D12_RESOURCE_DESC ResDesc = { };
		ResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResDesc.Alignment = 0;
		ResDesc.Width = m_Vertices.size()*sizeof(hwd3d_vertex);
		ResDesc.Height = 1;
		ResDesc.DepthOrArraySize = 1;
		ResDesc.MipLevels = 1;
		ResDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResDesc.SampleDesc = {1, 0};
		ResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		const HRESULT CreateRes = Dev->CreateCommittedResource(&HeapProps, HeapFlags, &ResDesc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&m_VB));
		if (FAILED(CreateRes) || !m_VB)
		{
			return false;
		}

		m_VBView = m_Game->GetBufferViewProvider().CreateView();
		m_VBView.GpuVirtualPtr = m_VB->GetGPUVirtualAddress();
		m_VBView.Size = m_Vertices.size()*sizeof(hwd3d_vertex);
	}

	// Fill index Buffer
	{
		std::vector<hwd3d_graphics_index> Indices;
		Indices.reserve(m_Triangles.size()*3);

		for (int i = 0; i < static_cast<int>(m_Triangles.size()); i++)
		{
			Indices.push_back(m_Triangles[i].v1);
			Indices.push_back(m_Triangles[i].v2);
			Indices.push_back(m_Triangles[i].v3);
		}

		const UINT IbSize = Indices.size()*sizeof(hwd3d_graphics_index);
		const D3D12_HEAP_PROPERTIES HeapProps = { D3D12_HEAP_TYPE_DEFAULT , D3D12_CPU_PAGE_PROPERTY_UNKNOWN , D3D12_MEMORY_POOL_UNKNOWN, 0x1 , 0x1 };

		const D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE;

		D3D12_RESOURCE_DESC ResDesc = { };
		ResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResDesc.Alignment = 0;
		ResDesc.Width = IbSize;
		ResDesc.Height = 1;
		ResDesc.DepthOrArraySize = 1;
		ResDesc.MipLevels = 1;
		ResDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResDesc.SampleDesc = {1, 0};
		ResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		const HRESULT CreateRes = Dev->CreateCommittedResource(&HeapProps, HeapFlags, &ResDesc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&m_IB));
		if (FAILED(CreateRes) || !m_IB)
		{
			return false;
		}

		m_IBView = m_Game->GetBufferViewProvider().CreateView();
		m_IBView.GpuVirtualPtr = m_IB->GetGPUVirtualAddress();
		m_IBView.Size = IbSize;
	}

	return true;
}
