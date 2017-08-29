#include "PThreadProcess.h"
DWORD WINAPI run(LPVOID sparam);

threadFun     CPThreadProcess::_threadRunFun=NULL;
char* CPThreadProcess::m_szFuncParam=NULL;
CPThreadProcess::CPThreadProcess(void)
{
	init();
}

CPThreadProcess::~CPThreadProcess(void)
{
	uninit();
}

#ifdef WIN32
DWORD WINAPI  CPThreadProcess::run(LPVOID sparam)
#else
int CPThreadProcess::run(void)
#endif
{	
#ifndef WIN32
	setThreadSignals(SIG_BLOCK, 1, SIGALRM);
	InitTimer(1, NULL);
#endif
	if(NULL != _threadRunFun)
	{
	   (*_threadRunFun)(m_szFuncParam);
	}
	return 0;
}

void CPThreadProcess::init()
{
#ifndef WIN32
	pthread_attr_init(&_attr);
	pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setschedpolicy(&_attr, SCHED_FIFO);//sched_policy
#endif
}

void CPThreadProcess::createThread()
{
#ifndef WIN32	
	int ret = pthread_create(&_thread, &_attr, run, (void *)NULL);
    if( 0 != ret )
	{
		printf("pthread_create err ret =%d err_str=%s\n", ret, strerror(errno));
		return;
	}
#else
	 int  dwThreadId=NULL;
	_handle = (HANDLE)CreateThread(NULL, 0, run, NULL, 0, (LPDWORD)&dwThreadId);  
	::WaitForSingleObject(_handle, INFINITE);
#endif
}

void CPThreadProcess::setThreadFun(threadFun _tmpThreadFun,char* szParam)
{
	_threadRunFun=_tmpThreadFun;
	m_szFuncParam = szParam;
}
void CPThreadProcess::uninit()
{
#ifndef WIN32
	pthread_exit(NULL);
	pthread_attr_destroy(&_attr);
#else
	if (NULL != _handle)
	{
	//	::CloseHandle(_handle);
	}	
#endif
}

void  CPThreadProcess::setThreadSignals(int how, int num, ...)
{
 #ifndef WIN32
	sigset_t mask;
    va_list args;
    
    // 初始化信号集
    sigemptyset(&mask);
    // 初始化va_list 变量，使args 指向第一个可选参数
    va_start(args, num);

    for(; num > 0; num--)
    {
        // 在mast 指向的信号集加入一个int 型信号
        // va_arg(args,int)  获取一个int型参数，并且args指向下一个参数
        sigaddset(&mask, va_arg(args, int));
    }
    // 设置控制信号集掩码
    pthread_sigmask(how, &mask, NULL);

    va_end(args);
#endif
}