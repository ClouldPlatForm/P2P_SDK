#pragma once
#include "HearBeat.h"
#include "RouterInfo.h"
#include "UACInfo.h"
#include "P2PHearBeat.h"
#include <map>

class CSessionManager
{
public:
	CSessionManager(void);
	~CSessionManager(void);
public:
	//����P2P�Ự
	void createP2PSessionID();
	//ֹͣP2P�Ự
	void closeP2PSessionID();
	//��ȡP2P�ỰID
	int getP2PSessionID();

	//����relay�Ự
	void createRelaySessionID();
	//�ر�relay�Ự
	void closeRelaySession();
	//��ȡRelay�ỰID
	int getRelaySessionID();

	//����UAC1·����Ϣ
	void setUAC1RouterInfo(const RouterInfo routerinfo);
	RouterInfo getUAC1RouterInfo();
    //����UAC2·����Ϣ���������һ��һ�򶴵������
	//void setUAC2RouterInfo(const RouterInfo routerinfo);
	//RouterInfo getUAC2RouterInfo();
    //���ö���Զ˵������������ֶ��һ�򶴵������
	void setUAC2RouterInfoVec(RouterInfo routerinfo,char* szUUID);
	std::vector<RouterInfo> getUAC2RouterInfoVec();
    RouterInfo* getUAC2RouterInfoFromVec(char* szUUID);

	//����UAC1����Ϣ
	void setUAC1Info(const CUACInfo& uacInfo);
	CUACInfo& getUAC1Info();

	//����UAC2����Ϣ
	//void setUAC2Info(const CUACInfo uacInfo);
	//CUACInfo getUAC2Info();
	//���ö���Զ����
	void setUAC2InfoVec(CUACInfo uacinfo,char* szUUID);
	CUACInfo* getUAC2InfoFromVec(char* szUUID);
	std::vector<CUACInfo> getUAC2InfoVec();

    //��ȡP2P������������Ϣ
	int getP2PServerHealthy();
    //��ȡTurn������������Ϣ
	int getTurnSvrHealthy();
	//��ȡ�Զ�Peer����
    int getOtherPeerHealthy();
    
    void setUAC1P2PSessionId();
	void setUAC1RelaySessionId();
	void uninit();

	int getP2PSessionId()
	{
      return m_nP2PSessionID;
	}
	int getRelaySessionId()
	{
	  return m_nRelaySessionID;
	}
private:
	long createSessionID();
	void init();
	
private:
   CUACInfo       m_UAC1Info;
   CUACInfo       m_UAC2Info;
   RouterInfo     m_UAC1RouterInfo;
   RouterInfo     m_UAC2RouterInfo;
   int		      m_nP2PSessionID;
   int		      m_nRelaySessionID;
   std::map<char*,RouterInfo> m_uuidRouterInfoMap;
   std::map<char*,CUACInfo>   m_uuidUACInfoMap;
};
