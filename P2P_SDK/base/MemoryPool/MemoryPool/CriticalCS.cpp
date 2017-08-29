#include "CriticalCS.h"
#include "Config.h"
CCriticalCS::CCriticalCS()
{
#ifdef WIN
	InitializeCriticalSection(&m_CS);
#else
	pthread_mutex_init(&m_mutex, NULL);
#endif
}
CCriticalCS::~CCriticalCS()
{
#ifdef WIN
	LeaveCriticalSection(&m_CS);
#else
	pthread_mutex_destroy(&m_mutex);
#endif

}
void CCriticalCS::Lock()
{
#ifdef WIN
	EnterCriticalSection(&m_CS);
#else
	pthread_mutex_lock(&m_mutex);
#endif

}
void CCriticalCS::UnLock()
{
#ifdef WIN
	LeaveCriticalSection(&m_CS);
#else
	pthread_mutex_unlock(&m_mutex);
#endif
}