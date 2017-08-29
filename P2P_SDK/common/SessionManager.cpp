#include "SessionManager.h"
#include "commonUtil.h"
#include <time.h>
using namespace std;
#define Random(x) (rand()%(x))

CSessionManager::CSessionManager(void)
{
	init();
}

CSessionManager::~CSessionManager(void)
{
	uninit();
}

void CSessionManager::init()
{ 
   m_nP2PSessionID = -1;
   m_nRelaySessionID = -1;
  // setUAC1P2PSessionId();
   createP2PSessionID();
   setUAC1P2PSessionId();
}


void CSessionManager::uninit()
{
	//todoԤ��
printf("CSessionManager::uninit\n");
	m_uuidRouterInfoMap.clear();
	m_uuidUACInfoMap.clear();
	closeP2PSessionID();
	closeRelaySession();
}

//����P2P�Ự
void CSessionManager::createP2PSessionID()
{
	m_nP2PSessionID = createSessionID();
}
//ֹͣP2P�Ự
void CSessionManager::closeP2PSessionID()
{
	m_nP2PSessionID = -1;
}
//��ȡP2P�ỰID
int CSessionManager::getP2PSessionID()
{
	return m_nP2PSessionID;
}
//����relay�Ự
void CSessionManager::createRelaySessionID()
{
	m_nRelaySessionID = createSessionID();
}
//�ر�relay�Ự
void CSessionManager::closeRelaySession()
{
	m_nRelaySessionID = -1;
}
//��ȡRelay�ỰID
int CSessionManager::getRelaySessionID()
{
	return m_nRelaySessionID;
}
//����UAC1·����Ϣ
void CSessionManager::setUAC1RouterInfo(const RouterInfo routerinfo)
{
	m_UAC1RouterInfo = routerinfo;
}
//����UAC2·����Ϣ
/*
void CSessionManager::setUAC2RouterInfo(const RouterInfo routerinfo)
{
	m_UAC2RouterInfo = routerinfo;
}
*/
//����UAC1����Ϣ
void CSessionManager::setUAC1Info(const CUACInfo& uacInfo)
{
	m_UAC1Info = uacInfo;	
}
//����UAC1��sessionId��Ϣ
void CSessionManager::setUAC1P2PSessionId()
{
//	createP2PSessionID();
	char szSessionId[10];
#ifdef WIN32
	int2str(m_nP2PSessionID,szSessionId);
#else	
    itoa(m_nP2PSessionID,szSessionId,10);
#endif	
	if (NULL == szSessionId)
	{
		m_nP2PSessionID = -1;
		return;
	}
	m_UAC1Info.setP2PSessionId(szSessionId);
}
void CSessionManager::setUAC1RelaySessionId()
{
	createRelaySessionID();
	char szSessionId[10];
#ifdef WIN32
	int2str(m_nRelaySessionID,szSessionId);
#else
	itoa(m_nRelaySessionID,szSessionId,10);
#endif
	if (NULL == szSessionId)
	{
		m_nRelaySessionID = -1;
		return;
	}
		printf("setUAC1RelaySessionId\n");
    m_UAC1Info.setSessionId(szSessionId);
}
int CSessionManager::getP2PServerHealthy()
{
	//todo
	return 0;
}

//��ȡTurn������������Ϣ �ݲ���
int CSessionManager::getTurnSvrHealthy()
{
	//todo
	return 0;
}

//��ȡ�Զ�Peer����
int CSessionManager::getOtherPeerHealthy()
{
	//todo
	return 0;
}

RouterInfo CSessionManager::getUAC1RouterInfo()
{
	return m_UAC1RouterInfo;
}

//RouterInfo CSessionManager::getUAC2RouterInfo()
//{
//	return m_UAC2RouterInfo;
//}

CUACInfo& CSessionManager::getUAC1Info() 
{
	return m_UAC1Info;
}

//CUACInfo CSessionManager::getUAC2Info()
//{
//	return m_UAC2Info;	
//}

long CSessionManager::createSessionID()
{
	int nSeed = (int)time(0);
	srand(nSeed);
	return Random(100000000);
}

void CSessionManager::setUAC2RouterInfoVec(RouterInfo routerinfo,char* szUUID)
{
	std::map<char*,RouterInfo>::iterator itr = m_uuidRouterInfoMap.find(szUUID);
	if(itr == m_uuidRouterInfoMap.end())
	{
		m_uuidRouterInfoMap.insert(std::make_pair(szUUID,routerinfo));
	}
}
std::vector<RouterInfo> CSessionManager::getUAC2RouterInfoVec()
{
	std::vector<RouterInfo> objDstRouterInfoVec;
	std::map<char*,RouterInfo>::iterator itr = m_uuidRouterInfoMap.begin();
	while(itr++ != m_uuidRouterInfoMap.end())
	{
		objDstRouterInfoVec.push_back(itr->second);
	}
	return objDstRouterInfoVec;
}

RouterInfo* CSessionManager::getUAC2RouterInfoFromVec(char* szUUID)
{
	std::map<char*,RouterInfo>::iterator itr = m_uuidRouterInfoMap.find(szUUID);
	if(itr != m_uuidRouterInfoMap.end())
	{
		return &(itr->second);
	}
	return NULL;
}

void CSessionManager::setUAC2InfoVec(CUACInfo uacinfo,char* szUUID)
{
	std::map<char*,CUACInfo>::iterator itr = m_uuidUACInfoMap.find(szUUID);
	if(itr == m_uuidUACInfoMap.end())
	{
		m_uuidUACInfoMap.insert(std::make_pair(szUUID,uacinfo));
	}

}
std::vector<CUACInfo> CSessionManager::getUAC2InfoVec()
{
	std::vector<CUACInfo> objDstUACInfoVec;
	std::map<char*,CUACInfo>::iterator itr = m_uuidUACInfoMap.begin();
	while(itr++ != m_uuidUACInfoMap.end())
	{
		objDstUACInfoVec.push_back(itr->second);
	}
	return objDstUACInfoVec;
}

CUACInfo* CSessionManager::getUAC2InfoFromVec(char* szUUID)
{
	std::map<char*,CUACInfo>::iterator itr = m_uuidUACInfoMap.find(szUUID);
	if(itr != m_uuidUACInfoMap.end())
	{
		return &(itr->second);
	}
	return NULL;

}