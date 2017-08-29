#ifndef _P2PHANDLER_H_
#define _P2PHANDLER_H_
#include "P2PHearBeat.h"
#include "../base/TemplateEntityClass.h"
#include "../common/DeviceInfo.h"
#include "../common/UACInfo.h"
#include "../common/PubSubModeBase.h"
#include "../common/RouterInfo.h"
class CTCPBaseCommunication;
class CIOHandler;
class CCommunicationMsgOperater;
class CUnifiedDispatchManager;
class CTimer; 
class CPThreadProcess;
class CSessionManager;
class NatCrossHelper;

//����Ķ�λΪ��1�����վ���P2Psvr������Э������ʹ���2���ͻ�����P2PSvr���������ƣ�
class P2PHandler:public Observer
{
public:
	P2PHandler();
	P2PHandler(CSessionManager* pobjSessionManager,NatCrossHelper* pobjNatCrossHelper);
	~P2PHandler();
public:
	//
	std::map<CDeviceInfo*,int>  getDeviceInfo()
	{     
		return objChannelInfoMap; 
	}
	void setAddress(const char* IP,const int port);
	void init();
	int initNet();
	void uninit();
	static int run(void* lpParam=NULL);
	static void TimerProc();
	static void p2pParseHandler(CBaseEntityClass<char*> obj,char* szMsgType);
	void setSessionManager(CSessionManager* pobjSessionManager);
	void getRelaySvrAddr(char* relayIP, int& relayPort)
	{
		if (NULL == m_szRelayIP)
		{
			relayIP = "127.0.0.1";
			relayPort=0;
			return;
		}	
		strcpy(relayIP,m_szRelayIP);
		relayPort = m_nRelayPort;
	};
    void setDevNatType(int sznattype)
	{
		 int2str(sznattype,m_szNatType);
		bNatTypeIsChanged = true;
	};
	//relay������
   void requestRelayStream(CUACInfo srcUACInfo);
   //p2p������Ŀ�Ķ˽�������,P2PSvr�ڸú��������������ת����
   void requestRemoteAddrToP2PSvr(CUACInfo srcUACInfo,char* szNatType,char* szStep,char* szStreamInfo);
   //�豸������Ϣ֪ͨ�Զ�
   void notifyOtherPeerByP2PSvr(CUACInfo srcUACInfo);
   void Update(CUACInfo objUACInfo,RouterInfo objRouterInfo);
   void Update();
private:
	void ReportHearBeat();
	void ReportDeviceInfoToP2PServer(char* szSessionId,char* szuuid,char* szdevtype);
	void ReportDevNatType(CBaseEntityClass<char*> tempObjBaseEntity);
	void ReportDevNatType(char* szSessionId,char* szUUID,char* szNatType);
	int ReportP2PResponse2();
	int sendData(CBaseEntityClass<char*> objBaseEntity);
	CTCPBaseCommunication* getCommunicationInstnce(char* sessionId);
	CTCPBaseCommunication* getCommunicationInstnce(int fd);
	int sendData(char* sessionId,char* buf);
	void setDeviceInfo(CDeviceInfo& deviceInfo);
    void setDstUACInfoValue(CBaseEntityClass<char*> objBaseEntity,CUACInfo& objUACInfo);
	void setDstRouterInfoValue(CBaseEntityClass<char*> objBaseEntity,RouterInfo& objRouterInfo);
private:
	CHearBeat*		                 objP2PHearBeat;
	CCommunicationMsgOperater*       pobjCommunicationMsgOperater;
    CIOHandler*			             pobjIOHandler;
    char				             szIPAddress[16];  //P2PServer��������ַIP
	int					             nPort;	 //P2PServer��������ַPort
    char*				             pszSendbuffer;
	char*                            pszRecvBuffer;
	CUnifiedDispatchManager*         pobjUnifiedDispatchManager;
	char                             m_szRelayIP[16];
	int                              m_nRelayPort;
	char					          m_szNatType[8];
	static P2PHandler*		         m_P2PHandler;
	CTimer*					         pobjTimer;
	CPThreadProcess*		         pobjPThreadProcess;
	std::map<CDeviceInfo*,int>       objChannelInfoMap; 
	bool                             bNatTypeIsChanged;     
	CUACInfo                         objSrcUACInfo;
	CUACInfo						 objDstUACInfo;
	RouterInfo                       m_objRouterInfo;//srcRouterInfo
	CSessionManager*				 m_pobjSessionManager;
	bool                             bRouterInfoParamIsChanged;
	bool                             bNatBeatHoleResult;//�򶴽׶α�־
	NatCrossHelper*					 m_pobjNatCrossHelper;
	CDeviceInfo                      m_objDeviceInfo;
};
#endif