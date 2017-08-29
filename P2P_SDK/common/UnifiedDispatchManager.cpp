#include "UnifiedDispatchManager.h"
#include "IOHandler.h"
#include "../common/protocol/CommunicationMsgOperater.h"
#include "../common/commonUtil.h"

CUnifiedDispatchManager::CUnifiedDispatchManager(void)
{
}

CUnifiedDispatchManager::~CUnifiedDispatchManager(void)
{
	if(NULL != pobjUnifiedDispatchManager)
	{
		delete pobjUnifiedDispatchManager;
		pobjUnifiedDispatchManager=NULL;
	}
	if(NULL != pobjCommunicationMsgOperater)
	{
		delete pobjCommunicationMsgOperater;
		pobjCommunicationMsgOperater = NULL;
	}
}
CUnifiedDispatchManager*  CUnifiedDispatchManager::pobjUnifiedDispatchManager=NULL;;
CUnifiedDispatchManager* CUnifiedDispatchManager::getInstance()
{
	if(NULL == pobjUnifiedDispatchManager)
		pobjUnifiedDispatchManager = new CUnifiedDispatchManager();
	return pobjUnifiedDispatchManager;
}

void CUnifiedDispatchManager::init()
{
	pobjCommunicationMsgOperater = new CCommunicationMsgOperater();
}

void CUnifiedDispatchManager::parseDispatcher(char* pRecvbuff,int currentFd,int type)
{
	char* pattern="\nlen:";  
	std::vector<std::string> result=split(pRecvbuff,pattern);  
for (int i=0;i<result.size();i++)
{

	//stun消息和端到端心跳的处理
	if(type == 2)
	{
		//TODO处理stun消息，端对端心跳消息
		Msg_head msg_head;
		CBaseEntityClass<char*>  objBaseEntityClass;
		CCommunicationMsgOperater::process_checkHead(const_cast<char*>(result[i].c_str()),&msg_head);//(pRecvbuff,&msg_head);
		char* szXMLbuff = CCommunicationMsgOperater::processMsgHead(const_cast<char*>(result[i].c_str()));//(pRecvbuff);
		if( 0 ==strcmp(msg_head.type,"UdpConnectResp") )
		{
			pobjCommunicationMsgOperater->parse_UdpConnectResp(szXMLbuff,objBaseEntityClass);
		}
		if( 0 ==strcmp(msg_head.type,"UdpConnectReq") )
		{
			pobjCommunicationMsgOperater->parse_UdpConnectReq(szXMLbuff,objBaseEntityClass);
		}
		if( 0 ==strcmp(msg_head.type,"Heartbeat") )
		{
			pobjCommunicationMsgOperater->parse_Heartbeat(szXMLbuff,objBaseEntityClass);
		}
		(*_udpHandler)(objBaseEntityClass,msg_head.type,currentFd);
		return;
	}

	//Relay消息处理
	int nRelayMsgMagicPoint = objRelayMsgHandler.findRelayMagicPoint(const_cast<char*>(result[i].c_str()));//(pRecvbuff);
	if (-1 != nRelayMsgMagicPoint)
	{
		//todo
        (*_FunHandler)(const_cast<char*>(result[i].c_str()),(result[i].length()));
		return;
	}
	//p2p消息处理
	Msg_head msg_head;
	CBaseEntityClass<char*>  objBaseEntityClass;
	CCommunicationMsgOperater::process_checkHead(const_cast<char*>(result[i].c_str()),&msg_head);//(pRecvbuff,&msg_head);
     //P2P处理

	char* szXMLbuff = CCommunicationMsgOperater::processMsgHead(const_cast<char*>(result[i].c_str()));//(pRecvbuff);
	if( 0 ==strcmp(msg_head.type,"P2pHeartbeat")  || 0 ==strcmp(msg_head.type,"Heartbeat"))
	{
		pobjCommunicationMsgOperater->parse_Heartbeat(szXMLbuff,objBaseEntityClass);
	}
    if ( strcmp(msg_head.type, "MsgResponse1")==0)
    {
		pobjCommunicationMsgOperater->parse_MsgResponse1(szXMLbuff,objBaseEntityClass);
    }
	if ( strcmp(msg_head.type, "P2pResponse1")==0)
	{
		pobjCommunicationMsgOperater->parse_P2pResponse1(szXMLbuff,objBaseEntityClass);
	}
	if ( strcmp(msg_head.type, "MsgResponse")==0)
	{
		pobjCommunicationMsgOperater->parse_MsgResponse(szXMLbuff,objBaseEntityClass);
	}
	if ( strcmp(msg_head.type, "P2pResponse2")==0)
	{
		pobjCommunicationMsgOperater->parse_P2pResponse2(szXMLbuff,objBaseEntityClass);
	}
	if ( strcmp(msg_head.type, "RelayResponse")==0 )
	{
		pobjCommunicationMsgOperater->parse_RelayResponse(szXMLbuff,objBaseEntityClass);
	}
	if ( strcmp(msg_head.type, "UpdateP2pRet")==0 )
	{
		pobjCommunicationMsgOperater->parse_UpdateP2pRet(szXMLbuff,objBaseEntityClass);	
	}


	(*_p2pHandler1)(objBaseEntityClass,msg_head.type);
  
}

}

void CUnifiedDispatchManager::setHandler(CALLBACKFUN objFunhandler)
{
	_FunHandler = objFunhandler;
}

void CUnifiedDispatchManager::setP2PHandler1(P2PCALLBACK1 objFunP2Phandler)
{
	_p2pHandler1 = objFunP2Phandler;
}

void CUnifiedDispatchManager::setStunHandler(UDPCALLBACK objStunHandler)
{
	_udpHandler = objStunHandler;
}