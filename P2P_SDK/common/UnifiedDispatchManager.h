#pragma once
#include "../base/TemplateEntityClass.h"
#include "../../common/protocol/RelayMsgProtocol.h"
typedef void (*P2PCALLBACK1)(CBaseEntityClass<char*>,char*); 
typedef void (*CALLBACKFUN)(char* recvbuf,int nLen);
typedef void (*UDPCALLBACK)(CBaseEntityClass<char*>,char*,int);

class CCommunicationMsgOperater;

class CUnifiedDispatchManager
{
private:
	CUnifiedDispatchManager(void);
	~CUnifiedDispatchManager(void);
public:
	static CUnifiedDispatchManager* getInstance();
	void init();
    void setHandler(CALLBACKFUN objFunhandler);//common relay
	void setStunHandler(UDPCALLBACK objStunHandler);//stun
	void setP2PHandler1(P2PCALLBACK1 objFunP2Phandler);//p2p
	void parseDispatcher(char* pRecvbuff,int currentFd = -1,int type=1);//1：TCP处理；2：UDP处理

private:
	static CUnifiedDispatchManager*  pobjUnifiedDispatchManager;
	CCommunicationMsgOperater*  pobjCommunicationMsgOperater;
	CALLBACKFUN  _FunHandler;
	P2PCALLBACK1 _p2pHandler1;
	UDPCALLBACK   _udpHandler;
	RelayMsgHandler      objRelayMsgHandler;
};
