
/*****************************************************************************
 * \File: AVXThreadPool.cpp
 * \Date: 2016/04/01 15:33
 * \Copyright: (C) 1990-2015 sky-light.com.hk
 * \Author: lijiang
 *
 * \Brief: 
 *
 * \Version: 1.0.0.0
 *
 * \Note:
 *****************************************************************************/
#include "AVXThreadPool.h"


static void avxcompat_threadpools_func(void * self)
{
	WorkItemBase * pTaskArgv = (WorkItemBase *)self;

	if (pTaskArgv->m_DoWorkFun)
	{
		pTaskArgv->m_DoWorkFun(pTaskArgv->m_pParam);
	}

	if (pTaskArgv->m_AbortFun)
	{
		pTaskArgv->m_AbortFun();
	}

	delete pTaskArgv;

	return;
}

CAVXThreadPool::CAVXThreadPool()
{
	m_ThreadCount = 0;
	m_PThreadErrorInfo = NULL;

	memset( &m_Object, 0, sizeof(AVXObject));
}

CAVXThreadPool::~CAVXThreadPool()
{

}

bool CAVXThreadPool::Create(const unsigned int nNumberOfThreads)
{
	long avx_success = 0;
	avx_success = IAVXThreadPool()->QueryInterface(&m_Object, GUID_AVXThreadPool, NULL);
	if (avx_success < 0) return false;

	AVXObject *pObj = IAVXThreadPool()->Create(&m_Object, nNumberOfThreads);
	if (pObj == NULL)
	{
		avx_success = IAVXThreadPool()->Release(&m_Object);
		return false;
	}
	m_ThreadCount = nNumberOfThreads;

	return true;
}

bool CAVXThreadPool::InsertWorkItem(WorkItemBase* pWorkItem)
{
	/*
	*
	*/
	WorkItemBase * workItem = new WorkItemBase;
	if (NULL == workItem)
	{
		m_PThreadErrorInfo->Error("CThreadPool", __FILE__, __LINE__, __FUNCTION__, "workItem == NULL!!!\n");
		return false;
	}

	memcpy(workItem, pWorkItem, sizeof(WorkItemBase));

	/*
	*
	*/
	long avx_success = 0;
	AVXThreadTask pTaskDesc = { 0 };
	pTaskDesc.func = avxcompat_threadpools_func;
	pTaskDesc.argv = workItem;
	avx_success = IAVXThreadPool()->AddTask(&m_Object, &pTaskDesc);

	return 1;
}

void CAVXThreadPool::Destroy(int iWairSecond)
{
	long avx_success = 0;
	avx_success = IAVXThreadPool()->Destory(&m_Object);
	avx_success = IAVXThreadPool()->Release(&m_Object);
}

int CAVXThreadPool::GetThreadTotalNum()
{

	return m_ThreadCount;
}

bool CAVXThreadPool::initThreadErrorInfo(CErrorInfo *pErrorInfo)
{
	m_PThreadErrorInfo = pErrorInfo;
	return 1;
}

