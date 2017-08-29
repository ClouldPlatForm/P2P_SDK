#ifndef __PTHREADMUTEXLOCK_H_
#define __PTHREADMUTEXLOCK_H_
#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
//#include "ace/OS.h"
#endif
//#include "ace/OS.h"
//互斥锁类
#ifdef WIN32
#define LOCK_MUTEXT CRITICAL_SECTION
#else
#define LOCK_MUTEXT pthread_mutex_t
#endif


class CThreadLock
{
public:
	CThreadLock(void)
	{
		Init();
	};

	~CThreadLock()
	{
		Close();
	};

	void Init()
	{
#ifdef _WIN32
		InitializeCriticalSection(&m_lock);
#else
		pthread_mutex_init(&m_lock, NULL);
#endif
	};

	void Close()
	{
#ifdef _WIN32
		DeleteCriticalSection(&m_lock);
#else
		pthread_mutex_destroy(&m_lock);
#endif
	}

	void Lock()
	{
		//m_Time = ACE_OS::gettimeofday();
#ifdef _WIN32
		EnterCriticalSection(&m_lock);
#else
		pthread_mutex_lock(&m_lock);
#endif
	};

	void UnLock()
	{
		//m_Time = ACE_OS::gettimeofday() - m_Time;

#ifdef _WIN32
		LeaveCriticalSection(&m_lock);
#else
		pthread_mutex_unlock(&m_lock);
#endif
	};

private:
	LOCK_MUTEXT m_lock;
	//ACE_Time_Value m_Time;
};

//自动加锁的类
class CAutoLock
{
public:
	CAutoLock(CThreadLock* pThreadLock) 
	{ 
		m_pThreadLock = pThreadLock;
		if(NULL != m_pThreadLock)
		{
			m_pThreadLock->Lock();
		}
	};
	//CAutoLock(){};
	~CAutoLock() 
	{
		if(NULL != m_pThreadLock)
		{
			m_pThreadLock->UnLock();
		}
	};

private:
	CThreadLock* m_pThreadLock;
};

#define PTHREAD_MUTEX_LOCK(x) (new CAutoLock(x));

/*
class CMutex
{
public:
 typedef pthread_mutex_t Mutex_T;
 typedef pthread_mutexattr_t Mattr_T;
 typedef int MType_T;

public:
 CMutex();
 CMutex(MType_T type_t);
 ~CMutex();

public:
 int Lock()
 {
  return pthread_mutex_lock(&m_mutex);
 }

 int Unlock()
 {
  return pthread_mutex_unlock(&m_mutex);
 }

 int Trylock()
 {
  return pthread_mutex_trylock(&m_mutex);
 }

 Mutex_T& GetMutex()
 {
  return m_mutex;
 }

private:
  Mutex_T m_mutex;
  Mattr_T m_attr;
};

class CMAutoLock //对线程互斥锁的控制类
{
public:
 explicit CMAutoLock(CMutex& mutex) : m_lock(mutex)
 {
  m_lock.Lock();
 }
 ~CMAutoLock()
 {
  m_lock.Unlock();
 }

private:
 CMutex& m_lock;

private:
 CMAutoLock operator=(const CMAutoLock& mutex);
 CMAutoLock(const CMAutoLock& mutex)
};

//读写锁类
class CRWLock
{
public:
 typedef pthread_rwlock_t RWLock_T;

public:
 CRWLock();

 ~CRWLock();

public:
 int RLock()
 {
  return pthread_rwlock_rdlock(&m_rwlock);
 }

 int WLock()
 {
  return pthread_rwlock_wrlock(&m_rwlock);
 }

 int TryRLock()
 {
  return pthread_rwlock_tryrdlock(&m_rwlock);
 }

 int TryWLock()
 {
  return pthread_rwlock_trywrlock(&m_rwlock);
 }

 int Unlock()
 {
  return pthread_rwlock_unlock(&m_rwlock);
 }

private:
 RWLock_T m_rwlock;
};

class CCond //条件变量类
{
public:
 typedef pthread_cond_t Cond_T;

public:
 CCond();
 ~CCond();

public:
 int Lock()
 {
  return m_lock.Lock();
 }

 int Unlock()
 {
  return m_lock.Unlock();
 }

 int Trylock()
 {
  return m_lock.Trylock();
 }

 bool Wait()
 {
  return 0 == pthread_cond_wait(&m_cond, &m_lock.GetMutex());
 }

 bool Wait( long int sencond ); //此函数非内联

 void Signal()
 {
  pthread_cond_signal(&m_cond);
 }

 void Broadcast()
 {
  pthread_cond_broadcast(&m_cond);
 }

private:
 Cond_T m_cond;
 CMutex m_lock; //互斥锁
};
*/
#endif