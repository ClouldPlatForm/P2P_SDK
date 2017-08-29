#include "ThreadPoolForLinux.h"

CThreadPoolForLinux::CThreadPoolForLinux()
{
	m_ThreadCount = 0;
#ifndef WIN
    pTp = NULL;
#endif
}


CThreadPoolForLinux::~CThreadPoolForLinux()
{
#ifndef WIN
	list<Thread_DESC*>::iterator iter;
	for (iter = theQueue.begin(); iter != theQueue.end(); iter++)
	{
		Thread_DESC* pDesc = *iter;
		if (NULL != pDesc)
		{
			delete pDesc;
			pDesc = NULL;
		}
	}
	theQueue.clear();
#endif
}
bool CThreadPoolForLinux::Create(const unsigned int nNumberOfThreads)
{
#ifndef WIN
	pTp = creat_thread_pool(nNumberOfThreads, 80);
	if (NULL == pTp)
	{
		m_PThreadErrorInfo->Error("CThreadPool", __FILE__, __LINE__, __FUNCTION__, "pTp == NULL!!!\n");
		return false;
	}
	tp_init(pTp);
#endif
	return true;
}
bool CThreadPoolForLinux::InsertWorkItem(WorkItemBase* pWorkItem)
{
#ifndef WIN
	Thread_DESC *pDesc = new Thread_DESC;
	if (NULL == pDesc)
	{
		m_PThreadErrorInfo->Error("CThreadPool", __FILE__, __LINE__, __FUNCTION__, "pDesc == NULL!!!\n");
		return false;
	}
	pDesc->m_work.process_job = pWorkItem->m_DoWorkFun;
	pDesc->m_pThreadParam = pWorkItem->m_pParam;

	tp_process_job(pTp, &(pDesc->m_work),(pDesc->m_pThreadParam));
	theQueue.push_back(pDesc);         
#endif
return true;
}
void CThreadPoolForLinux::Destroy(int iWairSecond)
{
#ifndef WIN
	tp_close(pTp);

#endif
}
int CThreadPoolForLinux::GetThreadTotalNum()
{
	return m_ThreadCount;
}

bool CThreadPoolForLinux::initThreadErrorInfo(CErrorInfo *pErrorInfo)
{
	if (NULL == pErrorInfo)
	{
		ScreenPrintf(PRGNAME, "pErrorInfo == NULL");
		return false;
	}
	m_PThreadErrorInfo = pErrorInfo;
	return true;
}