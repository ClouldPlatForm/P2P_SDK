#pragma once
#include <iostream>
#ifdef WIN32
#include <process.h>
#include <windows.h>
#else
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <ctime>
#include <errno.h>
#endif

 
typedef int (*threadFun)(void*);

class CPThreadProcess
{
public:
	CPThreadProcess(void);
	~CPThreadProcess(void);
public: 
	void createThread();
	void setThreadFun(threadFun threadFun,char* szParam);
#ifdef WIN32
	static DWORD WINAPI run(LPVOID sparam);	
#else
	int run(void);
#endif
private:
	 void init();
	 void uninit();
	 void setThreadSignals(int how, int num, ...);
private:
#ifdef WIN32 
	HANDLE _handle;
#else
	pthread_t*  _thread;
	pthread_attr_t* _attr;
#endif
	static threadFun     _threadRunFun;
	static char*         m_szFuncParam;
};
