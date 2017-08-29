/*****************************************************************************
 * \File: AVXThreadPool.h
 * \Date: 2016/04/01 15:32
 * \Copyright: (C) 1990-2015 sky-light.com.hk
 * \Author: lijiang
 *
 * \Brief: 
 *
 * \Version: 1.0.0.0
 *
 * \Note:
 *****************************************************************************/
#ifndef __SKYCLOUD_AVXTHTRADPOOL_H
#define __SKYCLOUD_AVXTHTRADPOOL_H

#include "ThreadBase.h"
#include "AVX/AVXCompat/ThreadPool.h"

class CAVXThreadPool : public CThreadBase
{
public:
	CAVXThreadPool();
	virtual ~CAVXThreadPool();
	virtual bool Create(const unsigned int nNumberOfThreads);
	virtual bool InsertWorkItem(WorkItemBase* pWorkItem);
	virtual void Destroy(int iWairSecond);
	virtual int GetThreadTotalNum();
	virtual bool initThreadErrorInfo(CErrorInfo *pErrorInfo);
private:
	int m_ThreadCount;
	CErrorInfo * m_PThreadErrorInfo;
	AVXObject    m_Object;
};


#endif //!__SKYCLOUD_AVXTHTRADPOOL_H
