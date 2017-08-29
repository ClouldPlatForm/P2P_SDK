#include "DeviceInfo.h"

CDeviceInfo::CDeviceInfo(void)
{
	/*m_szqId =NULL;
	m_szsessionId=NULL;
    m_szdevType=NULL;
    m_szuuid=NULL;*/
	memset(m_szqId,0,32);
	memset(m_szsessionId,0,16);
	memset(m_szdevType,0,16);
	memset(m_szuuid,0,32);
}

CDeviceInfo::~CDeviceInfo(void)
{
}

void CDeviceInfo::setqId(const char* szqId)
{
	memcpy(m_szqId,szqId,32);
}

char* CDeviceInfo::getqId()const
{
	return (char*)m_szqId;
}

void CDeviceInfo::setSessionId(const char* szsessionId)
{
	memcpy(m_szsessionId,szsessionId,16);
}

char* CDeviceInfo::getSessionId()const
{
	return (char*)m_szsessionId;
}

void CDeviceInfo::setDevType(const char* szDevType)
{
	//strcpy(m_szdevType,szDevType);
      memcpy(m_szdevType,szDevType,16);
}

char* CDeviceInfo::getDevType()const
{
	return (char*)m_szdevType;
}

void CDeviceInfo::setUUID(const char* szUUID)
{
	memcpy(m_szuuid,szUUID,32);
}

char* CDeviceInfo::getUUID()const
{
	return (char*)m_szuuid;
}