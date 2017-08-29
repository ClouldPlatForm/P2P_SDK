#pragma once
#include <stdlib.h>
#include <string.h> 
#include <iostream>
#include <stdio.h> 
#include <time.h>//包含time()函数
#ifndef WIN32
#include <sys/time.h>
#include <unistd.h> 
#else
#include <windows.h>
#endif
typedef void (*PTimerProc)(void);

#include "PThreadProcess.h"

class CTimer
{
public:
	CTimer(void);
	~CTimer(void);
public:
	void initTimer();
    void setTimerProc(PTimerProc pProcFun);
	long getCurTime();
	std::string getCurrentTime();
    void setIntervalTime(int nSeconds);//单位：秒
    static int run(void* lpParam=NULL);

private:
	CPThreadProcess  objPThreadProcess;
    int  m_nIntervalSeconds;
	PTimerProc funPTimerProc;
    static CTimer* m_ThisTimer;
	time_t startTime;
	time_t endTime;
};
