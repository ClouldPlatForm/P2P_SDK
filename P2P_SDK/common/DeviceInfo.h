#pragma once
#include <iostream>
#include <stdio.h>

using namespace std;

class CDeviceInfo
{
public:
	CDeviceInfo(void);
	~CDeviceInfo(void);
public:
	void setqId(const char* szqId);
	char* getqId()const;

	void setSessionId(const char* szsessionId);
	char* getSessionId()const;

    void setDevType(const char* szDevType);
	char* getDevType()const;

	void setUUID(const char* szUUID);
	char* getUUID()const;
private:
	char  m_szqId[32];
	char  m_szsessionId[16];
    char  m_szdevType[16];
    char  m_szuuid[32];
};
