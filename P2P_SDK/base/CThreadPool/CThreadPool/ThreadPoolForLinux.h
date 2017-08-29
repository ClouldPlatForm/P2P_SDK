#pragma once
#include "ThreadBase.h"
#include <list>
using namespace std;
#include "Config.h"
#ifndef WIN
#include "thread-pool.h"
#define  PWD           20
struct Thread_DESC
{
    void  *m_pThreadParam;
    tp_work      m_work;
};
#endif
class CThreadPoolForLinux :	public CThreadBase
{
public:
	CThreadPoolForLinux();
	virtual ~CThreadPoolForLinux();
	virtual bool Create(const unsigned int nNumberOfThreads);
	virtual bool InsertWorkItem(WorkItemBase* pWorkItem);
	virtual void Destroy(int iWairSecond);
	virtual int GetThreadTotalNum();
	virtual bool initThreadErrorInfo(CErrorInfo *pErrorInfo);
private:
#ifndef WIN
    tp_thread_pool *pTp;
    list<Thread_DESC*> theQueue;
#endif

	int m_ThreadCount;
	CErrorInfo * m_PThreadErrorInfo;

};

