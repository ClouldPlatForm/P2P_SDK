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
	//创建P2P会话
	void createP2PSessionID();
	//停止P2P会话
	void closeP2PSessionID();
	//获取P2P会话ID
	int getP2PSessionID();

	//创建relay会话
	void createRelaySessionID();
	//关闭relay会话
	void closeRelaySession();
	//获取Relay会话ID
	int getRelaySessionID();

	//设置UAC1路由信息
	void setUAC1RouterInfo(const RouterInfo routerinfo);
	RouterInfo getUAC1RouterInfo();
    //设置UAC2路由信息【针对哪种一对一打洞的情况】
	//void setUAC2RouterInfo(const RouterInfo routerinfo);
	//RouterInfo getUAC2RouterInfo();
    //设置多个对端的情况【针对哪种多对一打洞的情况】
	void setUAC2RouterInfoVec(RouterInfo routerinfo,char* szUUID);
	std::vector<RouterInfo> getUAC2RouterInfoVec();
    RouterInfo* getUAC2RouterInfoFromVec(char* szUUID);

	//设置UAC1的信息
	void setUAC1Info(const CUACInfo& uacInfo);
	CUACInfo& getUAC1Info();

	//设置UAC2的信息
	//void setUAC2Info(const CUACInfo uacInfo);
	//CUACInfo getUAC2Info();
	//设置多个对端情况
	void setUAC2InfoVec(CUACInfo uacinfo,char* szUUID);
	CUACInfo* getUAC2InfoFromVec(char* szUUID);
	std::vector<CUACInfo> getUAC2InfoVec();

    //获取P2P服务器心跳信息
	int getP2PServerHealthy();
    //获取Turn服务器心跳信息
	int getTurnSvrHealthy();
	//获取对端Peer心跳
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
