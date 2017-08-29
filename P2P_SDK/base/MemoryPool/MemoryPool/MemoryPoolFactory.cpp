#include "MemoryPoolFactory.h"
#include "CMemoryPool.h"
#include "Config.h"
#define PRGNAME "MemoryPoolLib"
void CMemoryPoolFactory::Version()
{
	printf("%s:Version:%d.%d.%d\n", PRGNAME, MAR_VERSION, MIN_VERSION, RELEASE_VERSION);
}
CMemoryPoolFactory::CMemoryPoolFactory()
{
	m_pMemoryPool = NULL;
}


CMemoryPoolFactory::~CMemoryPoolFactory()
{
	if (NULL != m_pMemoryPool)
	{
		delete m_pMemoryPool;
		m_pMemoryPool = NULL;
	}
}
bool CMemoryPoolFactory::CreateMemoryPool()
{
	if (NULL == m_pMemoryPool)
	{
        m_pMemoryPool = new CMemoryPool();
        if (m_pMemoryPool == NULL)
        {
            return false;
        }
	}
	return true;
}