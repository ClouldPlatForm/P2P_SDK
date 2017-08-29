#include "Timer.h"

CTimer* CTimer::m_ThisTimer = NULL;
CTimer::CTimer(void)
{
	m_nIntervalSeconds=30;
	m_ThisTimer = this;
	startTime = time(NULL);
	endTime = time(NULL);
}
CTimer::~CTimer(void)
{	
}
void CTimer::initTimer()
{
	objPThreadProcess.setThreadFun(&CTimer::run,NULL);
	objPThreadProcess.createThread();
}

void CTimer::setIntervalTime(int nSeconds)
{
	m_nIntervalSeconds = nSeconds;
}

void CTimer::setTimerProc(PTimerProc pProcFun)
{
	m_ThisTimer->funPTimerProc = pProcFun;
}

std::string CTimer::getCurrentTime()
{
	time_t tt = time(NULL);
	struct tm *ptmNow;
#ifndef WIN32
	localtime_r(&tt, ptmNow);
#else
	ptmNow = gmtime(&tt);
#endif
	char str[50];
	sprintf(str, "%d-%02d-%02d %02d:%02d:02%", ptmNow->tm_year + 1900,\
		ptmNow->tm_mon + 1, ptmNow->tm_mday,\
		ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);
	std::string strTime(str);

	return strTime;
}

long CTimer::getCurTime()
{
	time_t tt = time(NULL);
	return tt;
}

int CTimer::run(void* lpParam)
{
	m_ThisTimer->endTime = time(NULL);
	if( ( (m_ThisTimer->endTime) - (m_ThisTimer->startTime) ) > (m_ThisTimer->m_nIntervalSeconds) )
	{
	    if(NULL != m_ThisTimer->funPTimerProc)
	    (*(m_ThisTimer->funPTimerProc))();
		m_ThisTimer->startTime = m_ThisTimer->endTime;
	}
	//else
	//{
//#ifdef WIN32
//		Sleep(30000);
//#else
//		sleep(30000);
//#endif
//	}
    return 0;
}
