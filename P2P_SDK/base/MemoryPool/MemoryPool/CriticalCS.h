#pragma once
#include "Config.h"
#ifdef WIN
#include <Windows.h>
#else
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h> 
#endif
class CCriticalCS
{
public:
	CCriticalCS();
	~CCriticalCS();
	void Lock();
	void UnLock();
protected:
#ifndef WIN
	pthread_mutex_t m_mutex;
#else
	CRITICAL_SECTION m_CS;
#endif
};



