#include "PthreadMutexLock.h"
/*
#include <sys/time.h>

//互斥锁类
CMutex::CMutex()
{
 pthread_mutexattr_init(&m_attr);
 pthread_mutexattr_settype(&m_attr,PTHREAD_MUTEX_NORMAL);
 pthread_mutex_init(&m_mutex,&m_attr);
}
CMutex::CMutex(MType_T type_t)
{
 pthread_mutexattr_init(&m_attr);
 pthread_mutexattr_settype(&m_attr,type_t);
 pthread_mutex_init(&m_mutex,&m_attr);
}
CMutex::~CMutex()
{
 pthread_mutex_destroy(&m_mutex);
 pthread_mutexattr_destroy(&m_attr);
}

//读写锁类
CRWLock::CRWLock()
{
 pthread_rwlock_init(&m_rwlock,NULL);
}
CRWLock::~CRWLock()
{
 pthread_rwlock_destroy(&m_rwlock);
}

//条件变量类
CCond::CCond()
{
 pthread_cond_init(&m_cond,NULL);
}

CCond::~CCond()
{
 pthread_cond_destroy(&m_cond);
}

bool CCond::Wait( long int sencond )
{
 timespec tsp;
 struct timeval now;
 gettimeofday( &now, NULL );
 sencond += now.tv_sec;
 tsp.tv_sec = now.tv_sec + sencond;
 tsp.tv_nsec = now.tv_usec*1000 ;

 return 0 == pthread_cond_timedwait( &m_cond, &m_lock.GetMutex(), &tsp );
}
*/
