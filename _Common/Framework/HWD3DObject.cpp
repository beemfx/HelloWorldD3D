// D3D Hello World

#include "HWD3DObject.h"

void HWD3DObject::AddRef()
{
	m_NumObjectRefs++;
}

void HWD3DObject::Release()
{
	m_NumObjectRefs--;
	if (m_NumObjectRefs == 0)
	{
		delete this;
	}
}
