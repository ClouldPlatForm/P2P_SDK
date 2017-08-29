#pragma once
#include <iostream>
#include <stdio.h>
#include <vector>
using namespace std;

class CUACInfo
{
public:
	CUACInfo(void);
	~CUACInfo(void);
public:
	char*  getSrcUUID()const;
	void   setSrcUUID(const char* szUUID);
	char*  getDstUUID()const;
	void   setDstUUID(const char* szUUID);
	char*  getLocalIP()const;
	void   setLocalIP(const char* szIP);
	int    getLocalPort()const;
	void   setLocalPort(const int nPort);
	char*  getRemoteIP()const;
	void   setRemoteIP(const char* szIP);
	int    getRemotePort()const;
	void   setRemotePort(const int nPort);
    void   setSessionId(const char* szSessionID);//relay sessionId
	vector<char*> getSessionId()const;//relay sessionId
	void   clearRelaySessionId();
	void   setP2PSessionId(const char* szSessionID);//P2P sessionId
	vector<char*> getP2PSessionId()const;  //P2P sessionId
	void   clearP2PSessionId();
	void setDevType(const char* szDevType);
	char* getDevType()const;
	void   setStreamInfo(const int nStreamInfo);
	int    getStreamInfo()const;
	CUACInfo& operator=(const CUACInfo& boj);
private:
	char  m_szSrcUUID[32];
	char  m_szDstUUID[32];
	char  m_szLocalIP[15];
	int    m_nLocalPort;
	char  m_szRemoteIP[15];
	int    m_nRemotePort;
	char  m_szdevType[16];
	std::vector<char*> m_sessionIdVec;//relay session vector
	std::vector<char*> m_P2PsessionIdVec;//P2P session vector
	int    m_nStreamInfo;//流的信息
	char   szTemSessionID[10];
//	char   szP2PSessionID[10];
};
