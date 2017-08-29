#pragma once
#include "ThreadBase.h"
class CThreadPoolFacotry
{
public:
	CThreadPoolFacotry();
	~CThreadPoolFacotry();
	bool CreateProduct();
	CThreadBase * m_pThreadBase;
	void Version();
};

