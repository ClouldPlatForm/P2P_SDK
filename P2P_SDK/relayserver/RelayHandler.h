#pragma once
#include "../base/TemplateEntityClass.h"
#include "../common/DeviceInfo.h"
#include "../common/UACInfo.h"
#include "../common/protocol/RelayMsgProtocol.h"
#include "../base/communication/TCPClientBaseCommunication.h"

typedef int (*LISTENERFUN)(char* buffer,int& len);

class CSessionManager;
//class CTCPBaseCommunication;
class CIOHandler;
class CCommunicationMsgOperater;
class CUnifiedDispatchManager;
class CPThreadProcess;
class P2PHandler;

class RelayHandler
{
public:
	RelayHandler(CSessionManager* pobjSessionManager,P2PHandler* pobjP2PHandler);
	~RelayHandler(void);
public:
	int initNet();
	static int run(void* lpParam=NULL);
	void setTurnSvrAddr(char* szTurnIP,int nPort);
	void setSessionManager(CSessionManager* pobjSessionManager);
	void uninit();
	int findFdBySessionId(char* szSessionId);
    int findConnectStatusBySessionId(char* szSessionId);
	void setRelayRecvCallFun(LISTENERFUN pobjFun);
private:
	void init();
	void requestOpenOrCloseStream(char* szSessionId,bool bOpenflag=true); 
	static void relayParseHandler(char* buf,int nLen);
	void loginUpSvr(char* szSessionId);
	void getP2PHandler(P2PHandler* pobjP2PHandler);
	void updateConnStatus(char* szSessionId,int nStatus);
private:
	LISTENERFUN		 pobjCallBackFun;
	CSessionManager* m_pobjSessionManager;
	P2PHandler*      m_pobjP2PHandler;
	RelayMsgHandler  objRelayMsgHandler;
    CIOHandler*      pobjIOHandler;
	CUnifiedDispatchManager*         pobjUnifiedDispatchManager;
	static RelayHandler*		     m_RelayHandler;
	CPThreadProcess*		         pobjPThreadProcess;
	char*							 m_szTurnSvrIP;
	int							     m_nPort;
	std::map<char*,int>              m_SessionIdFdMap;
	std::map<char*,int>              m_SessionIdConnStstusMap;//0:建立连接；1：登录；2：流通道建立；3：流通道维持；4;流通道关闭；
	CTCPBaseCommunication            m_objTCPBaseCommunication;
};
