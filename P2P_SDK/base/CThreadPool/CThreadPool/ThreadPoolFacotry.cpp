
#include "ThreadPoolFacotry.h"
#include "ThreadPoolForLinux.h"
#include "AVXThreadPool.h"

#include "WorkQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;
#include "Config.h"
#ifndef WIN
#include <unistd.h>
#endif
#include <stdio.h>
//#define PRGNAME "ThreadPoolLib"
void CThreadPoolFacotry::Version()
{
	printf("%s:Version:%d.%d.%d\n", PRGNAME, MAR_VERSION, MIN_VERSION, RELEASE_VERSION);
}



CThreadPoolFacotry::CThreadPoolFacotry()
{
}


CThreadPoolFacotry::~CThreadPoolFacotry()
{
	if (NULL != m_pThreadBase)
	{
		delete m_pThreadBase;
		m_pThreadBase = NULL;
	}
}
bool CThreadPoolFacotry::CreateProduct()
{
	bool bRet =true;
#ifndef WIN

#if 0
	m_pThreadBase = new CThreadPoolForLinux();
	if (NULL == m_pThreadBase)
	{
		bRet = false;
	}
#else
	m_pThreadBase = new CAVXThreadPool();
	if (NULL == m_pThreadBase)
	{
		bRet = false;
	}
#endif

#else

	m_pThreadBase = new CWorkQueue();
	if (NULL == m_pThreadBase)
	{
		bRet = false;
	}

#endif
	return bRet;
}


/*
struct p1
{
	int count;
	char Name2[20];
};
void ThreadProc1(void * pParam)
{
	p1*pThis = (p1*)pParam;
	for (int i = 0; i < pThis->count;i++)
	{
		cout << "1Num:" << i << "" << pThis->Name2 << endl;
#ifdef WIN
		Sleep(1);
#else
		sleep(1);
#endif;
	}
}
struct  p2
{
	int count;
	char Name[20];
};
void ThreadProc2(void * pParam)
{
	p2*pThis = (p2*)pParam;
	for (int i = 0; i < pThis->count; i++)
	{
		cout << "2Num:" << i << "" << pThis->Name << endl;
#ifdef WIN
		Sleep(1);
#else
		sleep(1);
#endif;
	}
}

*/

// int main()
// {
// 	CThreadPoolFacotry m_fac;
// 	if (!m_fac.CreateProduct())
// 	{
// 		return 0;
// 	}
// 	m_fac.m_pThreadBase->Create(2);
// 	p1 param;
// 	param.count = 100;
// 	strcpy(param.Name2, "hello");
// 	WorkItemBase Base;
// 	Base.m_DoWorkFun = ThreadProc1;
// 	Base.m_pParam = (void*)&param;
//       printf("Base:m_DoworkFucn:%d\r\n", Base.m_DoWorkFun);;
//       printf("Base:m_m_pParam:%d\r\n ",Base.m_pParam);
// 	m_fac.m_pThreadBase->InsertWorkItem(&Base);
// 
// 
// 	p2 param1;
// 	param1.count = 100;
// 	strcpy(param1.Name, "good");
// 	WorkItemBase Base1;
// 	Base1.m_DoWorkFun = ThreadProc2;
//       
// 	Base1.m_pParam = (void*)&param1;
//       printf("Base1:m_DoworkFucn:%d\r\n", Base1.m_DoWorkFun);;
//       printf("Base1:m_m_pParam:%d\r\n ",Base1.m_pParam);
// 	m_fac.m_pThreadBase->InsertWorkItem(&Base1);
// #ifdef WIN
// 	Sleep(10000000000000000000);
// #else
// 	sleep(100000000000000000000000);
// #endif;
// }
