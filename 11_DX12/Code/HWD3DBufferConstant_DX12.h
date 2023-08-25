// D3D Hello World

#pragma once

#include "HWD3DBuffer_DX12.h"

enum class hwd3d_constant_buffer_t
{
	None ,
	VertexBuffer ,
	IndexBuffer ,
	ConstantBuffer ,
};

class HWD3DBufferConstant_DX12 : public HWD3DBufferBase_DX12
{
public:

	static HWD3DBufferConstant_DX12* CreateConstantBuffer(hwd3d_constant_buffer_t InType, UINT InSize, ID3D12Device& InDev);

private:

	hwd3d_constant_buffer_t m_ConstantBufferType = hwd3d_constant_buffer_t::None;

public:

	void PrepareForDraw(ID3D12GraphicsCommandList& Context);

private:

	void Init(hwd3d_constant_buffer_t InType, UINT InSize, ID3D12Device& InDev);

};
