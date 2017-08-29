#pragma once
#include "CMemoryPool.h"
class CMemoryPoolFactory
{
public:
	void Version();
	CMemoryPoolFactory();
	~CMemoryPoolFactory();
	bool CreateMemoryPool();
	CMemoryPool *m_pMemoryPool;
};

