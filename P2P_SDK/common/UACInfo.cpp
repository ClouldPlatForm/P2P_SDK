#include "UACInfo.h"

CUACInfo::CUACInfo(void)
{
	//m_szSrcUUID= new char[32];
	memset(m_szSrcUUID,0,32);
	memset(m_szDstUUID,0,32);
	memset(m_szLocalIP,0,15);
	memset(m_szRemoteIP,0,15);
	memset(m_szdevType,0,16);
	//m_szDstUUID= new char[32];
	//m_szLocalIP= new char[15];
	//m_szRemoteIP= new char[15];
	//m_szdevType= new char[16];
}

CUACInfo::~CUACInfo(void)
{
	/*if (NULL != m_szSrcUUID)
	{
		delete m_szSrcUUID;
		m_szSrcUUID = NULL;
	}
	if (NULL != m_szDstUUID)
	{
		delete m_szDstUUID;
		m_szDstUUID = NULL;
	}
	if (NULL != m_szLocalIP)
	{
		delete m_szLocalIP;
		m_szLocalIP = NULL;
	}
	if (NULL != m_szRemoteIP)
	{
		delete m_szRemoteIP;
		m_szRemoteIP = NULL;
	}
	if (NULL != m_szdevType)
	{
		delete m_szdevType;
		m_szdevType = NULL;
	}*/
}
CUACInfo& CUACInfo::operator=(const CUACInfo& obj)
{
	memcpy(m_szSrcUUID,obj.getSrcUUID(),32);
	memcpy(m_szDstUUID,obj.getDstUUID(),32);
	strcpy(m_szLocalIP,obj.getLocalIP());
	m_nLocalPort = obj.getLocalPort();
	strcpy(m_szRemoteIP,obj.getRemoteIP());
	m_nRemotePort = obj.getRemotePort();
	strcpy(m_szdevType,obj.getDevType());
	m_nStreamInfo = obj.getStreamInfo();
	m_sessionIdVec.clear();
	std::vector<char*> objVec = obj.getSessionId();
	for (int i=0;i<objVec.size();i++)
	{
       m_sessionIdVec.push_back(objVec[i]);
	}
	m_P2PsessionIdVec.clear();
	std::vector<char*> objP2PVec = obj.getP2PSessionId();
	int nSize = objP2PVec.size();
	for (int i=0;i<nSize;i++)
	{
		m_P2PsessionIdVec.push_back(objP2PVec[i]);
	}
   return *this;
}
char*  CUACInfo::getSrcUUID()const
{
	return (char*)m_szSrcUUID;
}
void   CUACInfo::setSrcUUID(const char* szUUID)
{
	strcpy(m_szSrcUUID,szUUID );
}
char*  CUACInfo::getDstUUID()const
{
	return (char*)(m_szDstUUID);
}
void   CUACInfo::setDstUUID(const char* szUUID)
{
	strcpy( m_szDstUUID ,szUUID );
}
char*  CUACInfo::getLocalIP()const
{
	return (char*)m_szLocalIP;
}
void   CUACInfo::setLocalIP(const char* szIP)
{
	strcpy(m_szLocalIP,szIP);
}
int    CUACInfo::getLocalPort()const
{
	return m_nLocalPort;
}
void   CUACInfo::setLocalPort(const int nPort)
{
	m_nLocalPort = nPort;
}
char*  CUACInfo::getRemoteIP()const
{
     return (char*)m_szRemoteIP;
}
void   CUACInfo::setRemoteIP(const char* szIP)
{
	strcpy(m_szRemoteIP,szIP);
}
int    CUACInfo::getRemotePort()const
{
	return m_nRemotePort;
}
void   CUACInfo::setRemotePort(const int nPort)
{
	m_nRemotePort = nPort;
}
void   CUACInfo::setSessionId(const char* szSessionID)
{
	//TODO 这里的值
	strcpy(szTemSessionID,szSessionID);
	m_sessionIdVec.push_back(szTemSessionID);//const_cast<char*>
}
vector<char*> CUACInfo::getSessionId()const
{
	return m_sessionIdVec;
}

void CUACInfo::setDevType(const char* szDevType)
{
	strcpy(m_szdevType,szDevType);
}

char* CUACInfo::getDevType()const
{
	return (char*)m_szdevType;
}

void   CUACInfo::setP2PSessionId(const char* szSessionID)
{
	m_P2PsessionIdVec.push_back(const_cast<char*>(szSessionID));
}
vector<char*> CUACInfo::getP2PSessionId()const
{
	return m_P2PsessionIdVec;
}

void   CUACInfo::clearRelaySessionId()
{
	m_sessionIdVec.clear();
}

void   CUACInfo::clearP2PSessionId()
{
	m_P2PsessionIdVec.clear();
}

void   CUACInfo::setStreamInfo(const int nStreamInfo)
{
	m_nStreamInfo = nStreamInfo;
}

int    CUACInfo::getStreamInfo()const
{
	return m_nStreamInfo;
}