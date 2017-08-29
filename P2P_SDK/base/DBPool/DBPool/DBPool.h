#pragma once
#include "../../CDatabase/CDatabase/DatabaseFactory.h"
#include <string>
#include <list>
#include "../../../Global/Global.h"
#include "../../../Global/AtomicLock.h"
#include <semaphore.h>
class CDBPool
{
public:
	CDBPool();
	~CDBPool();
	void Version();
    bool InitErroInfoMethod(void* pDatabaseErroInfoMethod);
    bool InitDBPool(CDBParam * pDBParam, DBTYPE DbType, int maxSize);
	CDatabaseFactory*GetConnection(); //获得数据库连接
	void ReleaseConnection(CDatabaseFactory *pconnFactory); //将数据库连接放回到连接池的容器中
	//CCriticalSectionFactory& GetCriticalSectionFactory() { return m_CriticalSectionFactory; }
	void DestoryConnPool();
private:
	int m_curSize; //当前已建立的数据库连接数量
	int m_maxSize; //连接池中定义的最大数据库连接数		
	list<CDatabaseFactory*> m_connList; //连接池的容器队列
	//CCriticalSectionFactory m_lock; //线程锁
	CDatabaseFactory*CreateConnection(); //创建一个连接
	bool InitConnection(int iInitialSize); //初始化数据库连接池
	void DestoryConnection(CDatabaseFactory *pconnFactory); //销毁数据库连接对象
	 //销毁数据库连接池
	CDBParam* m_pDBParam;
	//CCriticalSectionFactory m_CriticalSectionFactory;
	DBTYPE m_DbType;
    CErrorInfo* m_pDatabaseErroInfoMethod;
	CAtomic m_AtomicLock;

	sem_t m_sem;
};



