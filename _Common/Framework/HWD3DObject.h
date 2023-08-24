// D3D Hello World

#pragma once

#include "HWD3DTypes.h"

class HWD3DObject
{
private:
	
	int m_NumObjectRefs;

public:
	
	void AddRef();
	void Release();

protected:
	
	HWD3DObject() : m_NumObjectRefs(1) { }
	virtual ~HWD3DObject() { }
};
