#ifndef CworkQueueH
#define CworkQueueH
#include "Config.h"
#include <queue>
#ifdef WIN
//---------------------------------------------------------------------------

//#include<vcl.h>
#include <windows.h>
#endif
class CWorkQueue;
#include "ThreadBase.h" 
using namespace std;
 
typedef std::queue<WorkItemBase*>           WorkItemQueue,*PWorkItemQueue;
 
 
/*------------------------------------------------------------------------
CWorkQueue
 
  This is the WorkOueue class also known as thread pool,
  the basic idea of this class is creating thread that are waiting on a queue
  of work item when the queue is inserted with items the threads wake up and
  perform the requered work and go to sleep again.
------------------------------------------------------------------------*/
 
class  CWorkQueue :public CThreadBase
{
public:
 
   virtual ~CWorkQueue(){};
 
  virtual bool Create(const unsigned int       nNumberOfThreads );
  virtual bool InsertWorkItem(WorkItemBase* pWorkItem);
  virtual void Destroy(int iWairSecond);
  virtual int GetThreadTotalNum();
#ifdef WIN
private:
 
   static unsigned long __stdcall ThreadFunc( void* pParam );
   WorkItemBase* RemoveWorkItem();
   int GetWorekQueueSize();
 
 
 
   enum{
      ABORT_EVENT_INDEX = 0,
      SEMAPHORE_INDEX,
      NUMBER_OF_SYNC_OBJ,
   };
 
   //申请到的线程
   PHANDLE                  m_phThreads;
   unsigned int             m_nNumberOfThreads;
   void*                    m_pThreadDataArray;
 
   HANDLE                   m_phSincObjectsArray[NUMBER_OF_SYNC_OBJ];
 
   CRITICAL_SECTION         m_CriticalSection;
   PWorkItemQueue           m_pWorkItemQueue;
#endif
};
 
 
#endif
