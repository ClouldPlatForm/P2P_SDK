#include "DBPool.h"
#include <stdarg.h>

using namespace std;

#include "Config.h"
#include <stdio.h>
//#define PRGNAME "DBPoolLib"
void CDBPool::Version()
{
	printf("%s:Version:%d.%d.%d\n", DBPOOLNAME, MAR_VERSION,MIN_VERSION ,RELEASE_VERSION );
}
CDBPool::CDBPool()
{

}

//连接池的构造函�?
bool CDBPool::InitDBPool(CDBParam * pDBParam, DBTYPE DbType, int maxSize)
{
	
	/*
	if (!m_CriticalSectionFactory.CreateProduct())
	{
		return false;
	}
    else
    {
        m_CriticalSectionFactory.m_pCS->InitErroInfoMethod((void*)m_pDatabaseErroInfoMethod);
    }
	*/
	m_maxSize = maxSize;
	m_curSize = 0;
	m_pDBParam = pDBParam;
	m_DbType = DbType;
    
	if (!InitConnection(maxSize))
	{
        //m_pDatabaseErroInfoMethod->Error("InitConnection Failed!\n");
		m_pDatabaseErroInfoMethod->Error("CDBPool", __FILE__, __LINE__, __FUNCTION__, "InitConnection Failed!\n");
		return false;
	}
	
	return true;
}


//初始化连接池，创建最大连接数的一半连接数�?
bool CDBPool::InitConnection(int iInitialSize)
{
	CDatabaseFactory*pConn = NULL;
	//m_CriticalSectionFactory.m_pCS->Lock();
	m_AtomicLock.Lock();
	for (int i = 0; i < iInitialSize; i++)
	{
		pConn = CreateConnection();
		if (pConn)
		{
			m_connList.push_back(pConn);
			++(m_curSize);
		}
		else
		{
			//m_CriticalSectionFactory.m_pCS->UnLock();
			m_AtomicLock.UnLock();
            //m_pDatabaseErroInfoMethod->Error("CreateConnection Failed!\n");
			m_pDatabaseErroInfoMethod->Error("CDBPool", __FILE__, __LINE__, __FUNCTION__, "CreateConnection Failed!\n");
			return false;
		}
	}
	//m_CriticalSectionFactory.m_pCS->UnLock();
	m_AtomicLock.UnLock();

	sem_init(&m_sem, 0, iInitialSize);

	return true;
}

//创建连接,返回一个Connection
CDatabaseFactory* CDBPool::CreateConnection()
{
	CDatabaseFactory*pConn = NULL;
	pConn = new CDatabaseFactory();
	if (NULL == pConn)
	{
		m_pDatabaseErroInfoMethod->Error("CDBPool", __FILE__, __LINE__, __FUNCTION__, "NULL == pConn!\n");
		return NULL;
	}
	pConn->SelectDb(m_DbType);
	if (NULL == pConn->m_pDataBase)
	{
		delete pConn;
		pConn = NULL;
		m_pDatabaseErroInfoMethod->Error("CDBPool", __FILE__, __LINE__, __FUNCTION__, "NULL == pConn!\n");
		return NULL;
	}

    if (!pConn->m_pDataBase->IniteErroInfoMethod((void*)m_pDatabaseErroInfoMethod))
    {
        delete pConn;
        pConn = NULL;
		m_pDatabaseErroInfoMethod->Error("CDBPool", __FILE__, __LINE__, __FUNCTION__, "NULL == pConn!\n");
        return NULL;
    }

    if (!pConn->m_pDataBase->InitConnect(m_pDBParam))
	{
		delete pConn;
		pConn = NULL;
	}
	return pConn;
}

////在连接池中获得一个连�?
//CDatabaseFactory*CDBPool::GetConnection()
//{
//	CDatabaseFactory*pCon = NULL;
//	//m_CriticalSectionFactory.m_pCS->Lock();
//	m_AtomicLock.AtomicLock();
//
//	if (m_connList.size() > 0)
//	{   //连接池容器中还有连接
//		pCon = m_connList.front(); //得到第一个连�?
//		m_connList.pop_front();   //移除第一个连�?	
//		//如果连接为空，则创建连接出错
//		if (pCon == NULL)
//		{
//			--m_curSize;
//		}
//		//m_CriticalSectionFactory.m_pCS->UnLock();
//		m_AtomicLock.AtomicUnLock();
//		return pCon;
//	}
//	else
//	{
//		if (m_curSize < m_maxSize)
//		{ //还可以创建新的连�?
//			pCon = CreateConnection();
//			if (pCon)
//			{
//				++m_curSize;
//				//m_CriticalSectionFactory.m_pCS->UnLock();
//				m_AtomicLock.AtomicUnLock();
//				return pCon;
//			}
//			else
//			{
//				//m_CriticalSectionFactory.m_pCS->UnLock();
//				m_AtomicLock.AtomicUnLock();
//				m_pDatabaseErroInfoMethod->Error("CDBPool", __FILE__, __LINE__, __FUNCTION__, "NULL == pConn!\n");
//				return NULL;
//			}
//		}
//		else
//		{
//			//建立的连接数已经达到maxSize
//			//m_CriticalSectionFactory.m_pCS->UnLock();
//			m_AtomicLock.AtomicUnLock();
//			m_pDatabaseErroInfoMethod->Error("CDBPool", __FILE__, __LINE__, __FUNCTION__, "NULL == pConn!\n");
//			return NULL;
//		}
//	}
//}


//在连接池中获得一个连�?
CDatabaseFactory*CDBPool::GetConnection()
{
	CDatabaseFactory*pCon = NULL;
	//m_CriticalSectionFactory.m_pCS->Lock();

	sem_wait(&m_sem);

	m_AtomicLock.Lock();
	if (m_connList.size() > 0)
	{   //连接池容器中还有连接
		pCon = m_connList.front(); //得到第一个连�?
		m_connList.pop_front();   //移除第一个连�?
		//如果连接为空，则创建连接出错
		if (pCon == NULL)
		{
			--m_curSize;
		}
		pCon->m_pDataBase->Ping();
		//m_CriticalSectionFactory.m_pCS->UnLock();
		m_AtomicLock.UnLock();
		return pCon;
	}
	m_AtomicLock.UnLock();

}

//回收数据库连�?
void CDBPool::ReleaseConnection(CDatabaseFactory * pConn)
{
	if (NULL != pConn)
	{
		
		//m_CriticalSectionFactory.m_pCS->Lock();
		m_AtomicLock.Lock();
		m_connList.push_back(pConn);
		//m_CriticalSectionFactory.m_pCS->UnLock();
		m_AtomicLock.UnLock();
		sem_post(&m_sem);
	}
}

//连接池的析构函数
CDBPool::~CDBPool()
{
	DestoryConnPool();
}

//销毁连接池,首先要先销毁连接池的中连接
void CDBPool::DestoryConnPool()
{
	list<CDatabaseFactory*>::iterator icon;
	//m_CriticalSectionFactory.m_pCS->Lock();
	m_AtomicLock.Lock();
	for (icon = m_connList.begin(); icon != m_connList.end(); ++icon)
	{
		DestoryConnection(*icon); //销毁连接池中的连接
	}
	m_curSize = 0;
	m_connList.clear(); //清空连接池中的连�?
	//m_CriticalSectionFactory.m_pCS->UnLock();
	m_AtomicLock.UnLock();
}

//销毁一个连�?
void CDBPool::DestoryConnection(CDatabaseFactory* pConn)
{
	if (NULL != pConn)
	{
		delete pConn;
		pConn = NULL;
	}
}

bool CDBPool::InitErroInfoMethod(void* pDatabaseErroInfoMethod)
{
    bool bRet = false;

    if (NULL == pDatabaseErroInfoMethod)
    {
		ScreenPrintf(DBPOOLNAME, "NULL == pDatabaseErroInfoMethod!!!\n");
        return bRet;
    }

    m_pDatabaseErroInfoMethod = (CErrorInfo*)pDatabaseErroInfoMethod;


    bRet = true;
    return bRet;
}
