#ifndef _NAT_CROSS_H_
#define _NAT_CROSS_H_
#include "../common/PubSubModeBase.h"
#include "../base/TemplateEntityClass.h"
#include "../common/UACInfo.h"
#include "../common/RouterInfo.h"
#include <map>
#include <vector>

class CNatDetectionHelper;
class CSessionManager;
class CPThreadProcess;
class CTimer;
class CUDPClientBaseCommunication;
class CIOHandler;
class CUnifiedDispatchManager;

class NatCrossHelper:public Reporter
{
public:
	NatCrossHelper(CSessionManager* pobjSessionManager);
	~NatCrossHelper();
public:
	int initNet();
	void uninit();
    void setStunSvrAddr(char* hostName,int nPort);
    static int run(void* lparam);
	void getP2PInfo();
	int  findFdFromUDPChannel(char* szRecvUUID);
	int  findConnectStatusByUUID(char* szDstUUID);
private:
	static void timeProFun();
	static void udpHandler(CBaseEntityClass<char*> obj,char* szMsgType,int currentFd);	

private:
	void init();
	void getNatType();
	void assembleUdpConnectRequest(char* buf,CUACInfo objUACInfo);
	void assembleUdpConnectResponse(char* buf,char* szDstUUID,char* szSessionId);
	void assembleHearBeatResponse(char* szSrcSessionId,char* szDstUUID,char* buf,int nType =1);
    bool judgeNatValid();
	void beatHole();
	int findFdBySessionId(char* szSessionId);
	char* findSessionIdByFd(int fd);
	void sendUdpConnectMsgToOtherPeer(RouterInfo objDstRouterInfo,CUACInfo objSrcUAC);
	void onSleep(int nSecond,int nMs);//毫秒时间休息
	void SN_SNBeatHole(RouterInfo objSrcRouterInfo,RouterInfo objDstRouterInfo,CUACInfo objSrcUAC,CUACInfo objDstUAC);
	void SN_PCBeatHole(RouterInfo objSrcRouterInfo,RouterInfo objDstRouterInfo,CUACInfo objSrcUAC,CUACInfo objDstUAC);
	void PCorSN_SNof0Port(RouterInfo objSrcRouterInfo,RouterInfo objDstRouterInfo,CUACInfo objSrcUAC,CUACInfo objDstUAC);
	void PC_SNBeatHole(RouterInfo objSrcRouterInfo,RouterInfo objDstRouterInfo,CUACInfo objSrcUAC,CUACInfo objDstUAC);
	void NoSN_NoSNBeatHole(RouterInfo objSrcRouterInfo,RouterInfo objDstRouterInfo,CUACInfo objSrcUAC,CUACInfo objDstUAC);
	void ReceiverBeatHole(RouterInfo objSrcRouterInfo,RouterInfo objDstRouterInfo,CUACInfo objSrcUAC,CUACInfo objDstUAC);
	void RequesterBeatHole(RouterInfo objSrcRouterInfo,RouterInfo objDstRouterInfo,CUACInfo objSrcUAC,CUACInfo objDstUAC);
	void updateDstUUID_FdMap(char* szUUID,int fd);
	void updateSessionFdMap(char* szSessionId,int fd);
	void updateUDPConnStatus(char* szUUID,int nStatus);
	void updateUUIDTimeMap(char* szUUID,long nCurrentTime);
	void cleanOfflineDev();
	void cleanTestFd(int nCurrentFd);
	void cleanChooosedFd(int currentFd);
	void udpConnect();

private:
   CNatDetectionHelper*   m_pobjNatDetectionHelper;
   CSessionManager*       m_pobjSessionManager;
   CUACInfo               m_UACInfo;
   RouterInfo             m_RouterInfo;
   char*				  m_szStunSvrIP;
   int					  m_nStunSvrPort;
   CPThreadProcess*		  pobjPThreadProcess;
   int				      m_nbeatHoleStep;
   CTimer*                pobjTimer;
   long                   m_nCurTime;
   int				      m_ndetectionType;
   std::map<int,char*>     m_SessionFdMap;//针对发送多个请求给一个设备
   std::map<int,char*>     m_udpConnectedChannelMap;//针对接收多个设备的请求
   std::map<char*,int>     m_udpConnStatusMap;//udp连接状态:-2状态未知，-1离线，0维持在线状态，1上线。
   CIOHandler*             m_pobjIOHandler;
   CUnifiedDispatchManager* pobjUnifiedDispatchManager;
   static NatCrossHelper*   m_NatCrossHelper;
   std::map<char*,long>    m_objUUIDTimeMap;
};

#endif