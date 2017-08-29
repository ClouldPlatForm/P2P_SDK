#pragma hdrstop
#ifdef WIN
#include "WorkQueue.h"
#include "../../../Global/Global.h"
//---------------------------------------------------------------------------
#include <assert.h>

typedef struct _THREAD_CONTEXT
{
	CWorkQueue* pWorkQueue;
	void*       pThreadData;
} THREAD_CONTEXT, *PTHREAD_CONTEXT;

/*------------------------------------------------------------------------
建立多线程   nNumberOfThreads多线程数目  ThreadData线程函数执行的参数
------------------------------------------------------------------------*/

bool CWorkQueue::Create(const unsigned int  nNumberOfThreads)
{

	//创建任务队列，存放后续将要执行的任务
	m_pWorkItemQueue = new WorkItemQueue();

	if (NULL == m_pWorkItemQueue)
	{
		ScreenPrintf("CWorkQueue", "NULL == m_pWorkItemQueue!!!\n");
		return false;
	}

	//m_phSincObjectsArray保存了线程池的信号量和事件
	//m_phSincObjectsArray[ABORT_EVENT_INDEX]保存的是事件，当用户设置退出事件时使用
	//m_phSincObjectsArray[SEMAPHORE_INDEX]保存信号量，当用户设置执行任务时使用


	//创建信号量（多线程同步使用）
	/*在信号量上我们定义两种操作： Wait（等待） 和 Release（释放）。
	当一个线程调用Wait操作时，它要么得到资源然后将信号量减一，要么一直等下去（指放入阻塞队列），
	直到信号量大于等于一时。Release（释放）实际上是在信号量上执行加操作*/
	m_phSincObjectsArray[SEMAPHORE_INDEX] = CreateSemaphore(NULL, 0, LONG_MAX, NULL);

	if (m_phSincObjectsArray[SEMAPHORE_INDEX] == NULL)
	{
		delete m_pWorkItemQueue;
		m_pWorkItemQueue = NULL;
		ScreenPrintf("CWorkQueue", "m_phSincObjectsArray[SEMAPHORE_INDEX] == NULL!!!\n");
		return false;
	}

	//创建事件为手动置位，一次只能进入一个，False为初始不是运行状态（多线程同步使用）
	m_phSincObjectsArray[ABORT_EVENT_INDEX] = CreateEvent(NULL, TRUE, FALSE, NULL);


	if (m_phSincObjectsArray[ABORT_EVENT_INDEX] == NULL)
	{
		delete m_pWorkItemQueue;
		m_pWorkItemQueue = NULL;
		CloseHandle(m_phSincObjectsArray[SEMAPHORE_INDEX]);
		ScreenPrintf("CWorkQueue", "m_phSincObjectsArray[ABORT_EVENT_INDEX] == NULL!!!\n");
		return false;
	}


	//创建并初始化临界区（多线程互斥访问使用）
	InitializeCriticalSection(&m_CriticalSection);

	//创建线程数组
	m_phThreads = new HANDLE[nNumberOfThreads];

	if (m_phThreads == NULL)
	{
		delete m_pWorkItemQueue;
		m_pWorkItemQueue = NULL;
		CloseHandle(m_phSincObjectsArray[SEMAPHORE_INDEX]);
		CloseHandle(m_phSincObjectsArray[ABORT_EVENT_INDEX]);
		DeleteCriticalSection(&m_CriticalSection);
		ScreenPrintf("CWorkQueue", "m_phThreads == NULL!!!\n");
		return false;
	}

	unsigned int i;

	m_nNumberOfThreads = nNumberOfThreads;

	DWORD dwThreadId;
	PTHREAD_CONTEXT pThreadsContext;

	//创建所有的线程
	for (i = 0; i < nNumberOfThreads; i++)
	{
		//初始化线程函数运行时传入的参数
		pThreadsContext = new THREAD_CONTEXT;
		pThreadsContext->pWorkQueue = this;
		pThreadsContext->pThreadData = NULL;

		//创建线程
		m_phThreads[i] = CreateThread(NULL,
			0,
			CWorkQueue::ThreadFunc,
			pThreadsContext,
			0,
			&dwThreadId);


		if (m_phThreads[i] == NULL)
		{
			delete pThreadsContext;
			m_nNumberOfThreads = i;
			Destroy(5);
			ScreenPrintf("CWorkQueue", "m_phThreads[i] == NULL!!!\n");
			return false;
		}
	}

	return true;
}


/*------------------------------------------------------------------------
			  向任务队列添加任务
			  任务执行类通过继承基类WorkItemBase之后使用多态函数DoWork来完成真实任务

			  ------------------------------------------------------------------------*/
bool CWorkQueue::InsertWorkItem(WorkItemBase* pWorkItem)
{

	assert(pWorkItem != NULL);
	//多线程互斥访问，进入临界区
	EnterCriticalSection(&m_CriticalSection);
	WorkItemBase* ptr = new WorkItemBase;
	memcpy(ptr, pWorkItem, sizeof(WorkItemBase));
	//将任务插入队列
	m_pWorkItemQueue->push(ptr);

	//离开临界区
	LeaveCriticalSection(&m_CriticalSection);

	//释放信号量，使信号量加1，促使后面的Wailt操作执行
	if (!ReleaseSemaphore(m_phSincObjectsArray[SEMAPHORE_INDEX], 1, NULL))
	{
		assert(false);
		ScreenPrintf("CWorkQueue", "ReleaseSemaphore failed!!!\n");
		return false;
	}

	return true;

}

/*------------------------------------------------------------------------
从工作队列中移除对象，并返回移除的对象

------------------------------------------------------------------------*/
WorkItemBase*  CWorkQueue::RemoveWorkItem()
{

	WorkItemBase* pWorkItem;

	//多线程间访问互斥，进入临界区
	EnterCriticalSection(&m_CriticalSection);

	//移除对象
	pWorkItem = m_pWorkItemQueue->front();
	m_pWorkItemQueue->pop();

	//离开临界区，其他等待线程可以进入临界区
	LeaveCriticalSection(&m_CriticalSection);

	assert(pWorkItem != NULL);

	return pWorkItem;
}
/*------------------------------------------------------------------------
 线程执行的函数，实际执行的是任务队列中的DoWork()
 ------------------------------------------------------------------------*/
unsigned long __stdcall CWorkQueue::ThreadFunc(void*  pParam)
{

	//创建线程时传入的参数
	PTHREAD_CONTEXT       pThreadContext = (PTHREAD_CONTEXT)pParam;

	WorkItemBase*         pWorkItem = NULL;// pThreadContext->pWorkQueue;

	CWorkQueue*           pWorkQueue = pThreadContext->pWorkQueue;

	void*                 pThreadData = pThreadContext->pThreadData;

	DWORD dwWaitResult;
	for (;;)
	{
		//WaitForMultipleObjects等待pWorkQueue->m_phSincObjectsArray信号量数组两件事
		//一个是执行任务的释放信号量，一个是异常的释放信号量
		//当WaitForMultipleObjects等到多个内核对象的时候，如果它的bWaitAll 参数设置为false。
		//其返回值减去WAIT_OBJECT_0 就是参数lpHandles数组的序号。如果同时有多个内核对象被触发，
		//这个函数返回的只是其中序号最小的那个。如果为TRUE 则等待所有信号量有效在往下执行。
		//（FALSE 当有其中一个信号量有效时就向下执行）
		//本文WaitForMultipleObjects等待执行任务的信号量和退出销毁任务信息的事件
		//当有新任务添加到任务队列，设置执行任务信号量，触发任务执行
		//当设置退出事件时，销毁任务信息，所有线程因为没有设置事件复位信息，因此会全部销毁
		dwWaitResult = WaitForMultipleObjects(NUMBER_OF_SYNC_OBJ, pWorkQueue->m_phSincObjectsArray, FALSE, INFINITE);
		//WaitForMultipleObjects返回数组pWorkQueue->m_phSincObjectsArray编号
		switch (dwWaitResult - WAIT_OBJECT_0)
		{
			//返回异常编号
		case ABORT_EVENT_INDEX:
			delete pThreadContext;
			return 0;
			//返回执行任务编号
		case SEMAPHORE_INDEX:
			//从任务队列取一个任务执行
			pWorkItem = pWorkQueue->RemoveWorkItem();

			if (pWorkItem == NULL)
			{
				assert(false);
				break;
			}
			//执行真正的任务
			pWorkItem->m_DoWorkFun(pWorkItem->m_pParam);
			if (NULL != pWorkItem)
			{
				delete pWorkItem;
				pWorkItem = NULL;
			}
			break;

		default:
			assert(false);
			delete pThreadContext;
			return 0;

		}
	}


	//删除线程参数
	delete pThreadContext;
	return 1;
}

/**
	获取线程总数
	**/
int CWorkQueue::GetThreadTotalNum()
{
	return m_nNumberOfThreads;
}

/**
   获取任务池的大小
   **/
int CWorkQueue::GetWorekQueueSize()
{
	//多线程间访问互斥，进入临界区
	EnterCriticalSection(&m_CriticalSection);
	int iWorkQueueSize = m_pWorkItemQueue->size();
	//离开临界区
	LeaveCriticalSection(&m_CriticalSection);
	return iWorkQueueSize;
}


/*------------------------------------------------------------------------
Destroy
销毁线程池
------------------------------------------------------------------------*/
void CWorkQueue::Destroy(int iWairSecond)
{
	//为防止子线程任务没有执行完，主线程就销毁线程池，就加入一个等待函数
	while (0 != GetWorekQueueSize())
	{
		//主线程等待线程池完成所有任务
		Sleep(iWairSecond * 1000);
	}

	//设置退出事件，所有线程因为没有设置事件复位信息，因此会全部销毁
	if (!SetEvent(m_phSincObjectsArray[ABORT_EVENT_INDEX]))
	{
		assert(false);
		return;
	}


	//wait for all the threads to end
	WaitForMultipleObjects(m_nNumberOfThreads, m_phThreads, true, INFINITE);



	//clean queue
	while (!m_pWorkItemQueue->empty())
	{
		m_pWorkItemQueue->front()->m_AbortFun();
		m_pWorkItemQueue->pop();
	}

	delete m_pWorkItemQueue;
	m_pWorkItemQueue = NULL;
	CloseHandle(m_phSincObjectsArray[SEMAPHORE_INDEX]);
	CloseHandle(m_phSincObjectsArray[ABORT_EVENT_INDEX]);
	DeleteCriticalSection(&m_CriticalSection);

	//close all threads handles
	for (int i = 0; i < m_nNumberOfThreads; i++)
		CloseHandle(m_phThreads[i]);

	delete[] m_phThreads;
}
#endif
#pragma package(smart_init)