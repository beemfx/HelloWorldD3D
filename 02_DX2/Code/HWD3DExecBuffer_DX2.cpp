// D3D Hello World

#include "HWD3DExecBuffer_DX2.h"
#include "HWD3DGame_DX2.h"

HWD3DExecBuffer_DX2::HWD3DExecBuffer_DX2(class HWD3DGame_DX2* InGame)
	: m_Game(InGame)
{
	
}

void HWD3DExecBuffer_DX2::OnObjectDestruct()
{
	HWD3D_SafeRelease(m_ExecBuffer);
}

void HWD3DExecBuffer_DX2::BeginInstructions()
{
	m_InsStart = m_ExecBufferData.size();
}

void HWD3DExecBuffer_DX2::FinalizeBuffer()
{
	OP_EXIT();

	assert(m_RenderStateCount == m_NumRenderStatesAdded);
	assert(m_TransformStateCount == m_NumTransformStatesAdded);

	m_ExecBufferDesc.dwSize = sizeof(m_ExecBufferDesc);
	m_ExecBufferDesc.dwFlags = D3DDEB_BUFSIZE;
	m_ExecBufferDesc.dwBufferSize	= m_ExecBufferData.size();

	const HRESULT CreateExecBufferRes = m_Game->GetDevice()->CreateExecuteBuffer(&m_ExecBufferDesc, &m_ExecBuffer, nullptr);
	if (FAILED(CreateExecBufferRes) || !m_ExecBuffer)
	{
		return;
	}

	if (SUCCEEDED(m_ExecBuffer->Lock(&m_ExecBufferDesc)))
	{
		memcpy(m_ExecBufferDesc.lpData, m_ExecBufferData.data(), m_ExecBufferData.size());

		m_ExecBuffer->Unlock();
	}

	D3DEXECUTEDATA ExecData = {};
	ExecData.dwSize = sizeof(ExecData);
	ExecData.dwInstructionOffset = m_InsStart;
	ExecData.dwInstructionLength = (ULONG)(m_ExecBufferData.size() - m_InsStart);
	ExecData.dwVertexCount = m_NumVerts;
	const HRESULT SetDataRes = m_ExecBuffer->SetExecuteData(&ExecData);
	if (FAILED(SetDataRes))
	{
		return;
	}
}

void HWD3DExecBuffer_DX2::ExecuteBuffer()
{
	if (m_ExecBuffer && m_Game && m_Game->GetDevice() && m_Game->GetViewport())
	{
		const HRESULT ExecRes = m_Game->GetDevice()->Execute(m_ExecBuffer, m_Game->GetViewport(), D3DEXECUTE_CLIPPED);
		assert(SUCCEEDED(ExecRes));
	}
}
