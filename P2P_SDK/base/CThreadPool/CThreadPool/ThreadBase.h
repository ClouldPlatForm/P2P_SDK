#pragma once

#include "../../../Global/Global.h"
/**
用法原理：通过派生类WorkItemBase的dowork方法来实现，线程处理任务
通过create任务创建线程，并且这些线程一直在for循环里等待事件监�?
一旦任务栈里有数据了触发线程执行任务�?
**/
/*------------------------------------------------------------------------
WorkItemBase

this is the basic WorkItem that the Work Queue Use its interface
This class should be inherited and these virtual abstract functions
implemented.

DoWork()

virtual abstract function is the function that is called when the
work item turn has came to be poped out of the queue and be processed.


Abort ()

This function is called, when the Destroy function is called, for each of the WorkItems
That are left in the queue.


------------------------------------------------------------------------*/
typedef void(*DoWork)(void* pThreadContext);
typedef void(*Abort)();
struct  WorkItemBase
{
	DoWork m_DoWorkFun;
	Abort  m_AbortFun;
	void * m_pParam;
};



class CThreadBase
{
public:
	CThreadBase();
	virtual ~CThreadBase();
	virtual bool Create(const unsigned int nNumberOfThreads) = 0;
	virtual bool InsertWorkItem(WorkItemBase* pWorkItem) = 0;
	virtual void Destroy(int iWairSecond) = 0;
	virtual int GetThreadTotalNum() = 0;
	virtual bool initThreadErrorInfo(CErrorInfo *pErrorInfo) = 0;
};

