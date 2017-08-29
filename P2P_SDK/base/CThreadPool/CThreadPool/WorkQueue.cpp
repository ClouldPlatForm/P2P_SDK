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
�������߳�   nNumberOfThreads���߳���Ŀ  ThreadData�̺߳���ִ�еĲ���
------------------------------------------------------------------------*/

bool CWorkQueue::Create(const unsigned int  nNumberOfThreads)
{

	//����������У���ź�����Ҫִ�е�����
	m_pWorkItemQueue = new WorkItemQueue();

	if (NULL == m_pWorkItemQueue)
	{
		ScreenPrintf("CWorkQueue", "NULL == m_pWorkItemQueue!!!\n");
		return false;
	}

	//m_phSincObjectsArray�������̳߳ص��ź������¼�
	//m_phSincObjectsArray[ABORT_EVENT_INDEX]��������¼������û������˳��¼�ʱʹ��
	//m_phSincObjectsArray[SEMAPHORE_INDEX]�����ź��������û�����ִ������ʱʹ��


	//�����ź��������߳�ͬ��ʹ�ã�
	/*���ź��������Ƕ������ֲ����� Wait���ȴ��� �� Release���ͷţ���
	��һ���̵߳���Wait����ʱ����Ҫô�õ���ԴȻ���ź�����һ��Ҫôһֱ����ȥ��ָ�����������У���
	ֱ���ź������ڵ���һʱ��Release���ͷţ�ʵ���������ź�����ִ�мӲ���*/
	m_phSincObjectsArray[SEMAPHORE_INDEX] = CreateSemaphore(NULL, 0, LONG_MAX, NULL);

	if (m_phSincObjectsArray[SEMAPHORE_INDEX] == NULL)
	{
		delete m_pWorkItemQueue;
		m_pWorkItemQueue = NULL;
		ScreenPrintf("CWorkQueue", "m_phSincObjectsArray[SEMAPHORE_INDEX] == NULL!!!\n");
		return false;
	}

	//�����¼�Ϊ�ֶ���λ��һ��ֻ�ܽ���һ����FalseΪ��ʼ��������״̬�����߳�ͬ��ʹ�ã�
	m_phSincObjectsArray[ABORT_EVENT_INDEX] = CreateEvent(NULL, TRUE, FALSE, NULL);


	if (m_phSincObjectsArray[ABORT_EVENT_INDEX] == NULL)
	{
		delete m_pWorkItemQueue;
		m_pWorkItemQueue = NULL;
		CloseHandle(m_phSincObjectsArray[SEMAPHORE_INDEX]);
		ScreenPrintf("CWorkQueue", "m_phSincObjectsArray[ABORT_EVENT_INDEX] == NULL!!!\n");
		return false;
	}


	//��������ʼ���ٽ��������̻߳������ʹ�ã�
	InitializeCriticalSection(&m_CriticalSection);

	//�����߳�����
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

	//�������е��߳�
	for (i = 0; i < nNumberOfThreads; i++)
	{
		//��ʼ���̺߳�������ʱ����Ĳ���
		pThreadsContext = new THREAD_CONTEXT;
		pThreadsContext->pWorkQueue = this;
		pThreadsContext->pThreadData = NULL;

		//�����߳�
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
			  ����������������
			  ����ִ����ͨ���̳л���WorkItemBase֮��ʹ�ö�̬����DoWork�������ʵ����

			  ------------------------------------------------------------------------*/
bool CWorkQueue::InsertWorkItem(WorkItemBase* pWorkItem)
{

	assert(pWorkItem != NULL);
	//���̻߳�����ʣ������ٽ���
	EnterCriticalSection(&m_CriticalSection);
	WorkItemBase* ptr = new WorkItemBase;
	memcpy(ptr, pWorkItem, sizeof(WorkItemBase));
	//������������
	m_pWorkItemQueue->push(ptr);

	//�뿪�ٽ���
	LeaveCriticalSection(&m_CriticalSection);

	//�ͷ��ź�����ʹ�ź�����1����ʹ�����Wailt����ִ��
	if (!ReleaseSemaphore(m_phSincObjectsArray[SEMAPHORE_INDEX], 1, NULL))
	{
		assert(false);
		ScreenPrintf("CWorkQueue", "ReleaseSemaphore failed!!!\n");
		return false;
	}

	return true;

}

/*------------------------------------------------------------------------
�ӹ����������Ƴ����󣬲������Ƴ��Ķ���

------------------------------------------------------------------------*/
WorkItemBase*  CWorkQueue::RemoveWorkItem()
{

	WorkItemBase* pWorkItem;

	//���̼߳���ʻ��⣬�����ٽ���
	EnterCriticalSection(&m_CriticalSection);

	//�Ƴ�����
	pWorkItem = m_pWorkItemQueue->front();
	m_pWorkItemQueue->pop();

	//�뿪�ٽ����������ȴ��߳̿��Խ����ٽ���
	LeaveCriticalSection(&m_CriticalSection);

	assert(pWorkItem != NULL);

	return pWorkItem;
}
/*------------------------------------------------------------------------
 �߳�ִ�еĺ�����ʵ��ִ�е�����������е�DoWork()
 ------------------------------------------------------------------------*/
unsigned long __stdcall CWorkQueue::ThreadFunc(void*  pParam)
{

	//�����߳�ʱ����Ĳ���
	PTHREAD_CONTEXT       pThreadContext = (PTHREAD_CONTEXT)pParam;

	WorkItemBase*         pWorkItem = NULL;// pThreadContext->pWorkQueue;

	CWorkQueue*           pWorkQueue = pThreadContext->pWorkQueue;

	void*                 pThreadData = pThreadContext->pThreadData;

	DWORD dwWaitResult;
	for (;;)
	{
		//WaitForMultipleObjects�ȴ�pWorkQueue->m_phSincObjectsArray�ź�������������
		//һ����ִ��������ͷ��ź�����һ�����쳣���ͷ��ź���
		//��WaitForMultipleObjects�ȵ�����ں˶����ʱ���������bWaitAll ��������Ϊfalse��
		//�䷵��ֵ��ȥWAIT_OBJECT_0 ���ǲ���lpHandles�������š����ͬʱ�ж���ں˶��󱻴�����
		//����������ص�ֻ�����������С���Ǹ������ΪTRUE ��ȴ������ź�����Ч������ִ�С�
		//��FALSE ��������һ���ź�����Чʱ������ִ�У�
		//����WaitForMultipleObjects�ȴ�ִ��������ź������˳�����������Ϣ���¼�
		//������������ӵ�������У�����ִ�������ź�������������ִ��
		//�������˳��¼�ʱ������������Ϣ�������߳���Ϊû�������¼���λ��Ϣ����˻�ȫ������
		dwWaitResult = WaitForMultipleObjects(NUMBER_OF_SYNC_OBJ, pWorkQueue->m_phSincObjectsArray, FALSE, INFINITE);
		//WaitForMultipleObjects��������pWorkQueue->m_phSincObjectsArray���
		switch (dwWaitResult - WAIT_OBJECT_0)
		{
			//�����쳣���
		case ABORT_EVENT_INDEX:
			delete pThreadContext;
			return 0;
			//����ִ��������
		case SEMAPHORE_INDEX:
			//���������ȡһ������ִ��
			pWorkItem = pWorkQueue->RemoveWorkItem();

			if (pWorkItem == NULL)
			{
				assert(false);
				break;
			}
			//ִ������������
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


	//ɾ���̲߳���
	delete pThreadContext;
	return 1;
}

/**
	��ȡ�߳�����
	**/
int CWorkQueue::GetThreadTotalNum()
{
	return m_nNumberOfThreads;
}

/**
   ��ȡ����صĴ�С
   **/
int CWorkQueue::GetWorekQueueSize()
{
	//���̼߳���ʻ��⣬�����ٽ���
	EnterCriticalSection(&m_CriticalSection);
	int iWorkQueueSize = m_pWorkItemQueue->size();
	//�뿪�ٽ���
	LeaveCriticalSection(&m_CriticalSection);
	return iWorkQueueSize;
}


/*------------------------------------------------------------------------
Destroy
�����̳߳�
------------------------------------------------------------------------*/
void CWorkQueue::Destroy(int iWairSecond)
{
	//Ϊ��ֹ���߳�����û��ִ���꣬���߳̾������̳߳أ��ͼ���һ���ȴ�����
	while (0 != GetWorekQueueSize())
	{
		//���̵߳ȴ��̳߳������������
		Sleep(iWairSecond * 1000);
	}

	//�����˳��¼��������߳���Ϊû�������¼���λ��Ϣ����˻�ȫ������
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