#include "RelayHandler.h"
#include "../common/SessionManager.h"
#include "../p2pServer/P2PHandler.h"
//#include "../base/communication/TCPClientBaseCommunication.h"
#include "../common/protocol/RelayMsgProtocol.h"
#include "../base/communication/IOHandler.h"
#include "../common/UnifiedDispatchManager.h"
//#include "../base/Timer.h"
#include "../base/PThreadProcess.h"
#include "../natcross/StunUtil.h"
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <stdlib.h>

RelayHandler*  RelayHandler::m_RelayHandler = NULL;

RelayHandler::RelayHandler(CSessionManager* pobjSessionManager,P2PHandler* pobjP2PHandler)
{
	m_RelayHandler = this;
	m_szTurnSvrIP = NULL;
	m_nPort = -1;
	setSessionManager(pobjSessionManager);
	getP2PHandler(pobjP2PHandler);
	init();
}

RelayHandler::~RelayHandler(void)
{
	uninit();
	if (NULL != pobjPThreadProcess)
	{
		delete pobjPThreadProcess;
		pobjPThreadProcess = NULL;
	}
}
void RelayHandler::uninit()
{
	//清理资源
	std::map<char*,int>::iterator itr = m_SessionIdFdMap.begin();
	while(itr != m_SessionIdFdMap.end())
	{
		pobjIOHandler->clearIOFd(itr->second);
		m_SessionIdFdMap.erase(itr++);
	}
	(m_pobjSessionManager->getUAC1Info()).clearRelaySessionId();
	m_SessionIdConnStstusMap.clear();
}

void RelayHandler::init()
{
	pobjCallBackFun = NULL;
	//增加回调函数
	pobjUnifiedDispatchManager = CUnifiedDispatchManager::getInstance();
	pobjUnifiedDispatchManager->setHandler(relayParseHandler);

	//初始化线程,整个relay使用一个线程处理
	pobjPThreadProcess = new CPThreadProcess();
	pobjPThreadProcess->setThreadFun(run,NULL);
	pobjPThreadProcess->createThread();
}

void RelayHandler::setTurnSvrAddr(char* szTurnIP,int nPort)
{
	strcpy(m_szTurnSvrIP,szTurnIP);
	m_nPort = nPort;
}

//在init之前必须先获取好P2PHandler
int RelayHandler::initNet()
{
	//产生RelaysessionId
	m_pobjSessionManager->setUAC1RelaySessionId();

	//请求RelayRequestStream
	CUACInfo objUACInfo = m_pobjSessionManager->getUAC1Info();
	m_pobjP2PHandler->requestRelayStream(objUACInfo);
	//创建TCP连接
	pobjIOHandler = CIOHandler::getInstance();
	std::vector<char*> tmpszSessionVec = objUACInfo.getSessionId();
	if (0 == tmpszSessionVec.size())
	{
		return -1;
	}
	char relayIP[15];
	memset(relayIP,0,15);
	int relayPort = -1;
	if(NULL != m_szTurnSvrIP)
	{
		strcpy(relayIP,m_szTurnSvrIP);
		relayPort = m_nPort;
	}
	else
	{
		m_pobjP2PHandler->getRelaySvrAddr(relayIP,relayPort);
		//异常处理
		if (NULL == relayIP)
		{
			//记日志
			return -1;
		}
	}
    int locolPort = objUACInfo.getLocalPort();
	for(int i=0; i<tmpszSessionVec.size();i++)
	{
		//初始化网络，建立tcp连接
		//CTCPBaseCommunication  objTCPBaseCommunication;
		m_objTCPBaseCommunication.setLocalPort(locolPort+i);
		m_objTCPBaseCommunication.init(relayIP,relayPort);
		int fd = m_objTCPBaseCommunication.getSockFd();
		const char* szSessionId = tmpszSessionVec[i];
		pobjIOHandler->addReadSockFd(const_cast<char*>(szSessionId),fd);
		pobjIOHandler->addWriteSockFd(fd);
		pobjIOHandler->setClientCommunication(&m_objTCPBaseCommunication,szSessionId);
		m_SessionIdFdMap.insert(std::make_pair(const_cast<char*>(szSessionId),fd));
		updateConnStatus(const_cast<char*>(szSessionId),0);
		//登录,此处最好能够判断下，后续改正
		loginUpSvr(const_cast<char*>(szSessionId));
	}
	//请求开流
	for(int i=0; i<tmpszSessionVec.size();i++)
	{
		requestOpenOrCloseStream(tmpszSessionVec[i],true);
	}
	return 0;
}

int RelayHandler::run(void* lpParam)
{
	/*while (1)
	{*/	
		if(!m_RelayHandler->m_SessionIdFdMap.empty())
		{
			//每隔200ms获取一次P2Pserver反馈过来的消息。
			bool bRet = (m_RelayHandler->pobjIOHandler)->pulse();
			if (!bRet)
			{
				return -1;
			}
		}
		
		////睡眠200ms
		//struct timeval timeout;
		//timeout.tv_sec =0;
		//timeout.tv_usec =200;
		//(m_RelayHandler->pobjIOHandler)->setTimerBySelect(timeout);
		return 0;
//	}
}

void RelayHandler::requestOpenOrCloseStream(char* szSessionId,bool bOpenflag)
{
	CUACInfo objUACInfo = m_pobjSessionManager->getUAC1Info();
	std::vector<char*> szSessionVec = objUACInfo.getSessionId();
	std::vector<char*>::iterator szSessionVecItr = find(szSessionVec.begin(),szSessionVec.end(),szSessionId);
	if (szSessionVecItr == szSessionVec.end())
	{
		//没找到
		return;
	}
	//组装消息体
	relay_protocol_header_t  objRelayHead;
	objRelayHead.magic=MAGIC;
	objRelayHead.cmd = CMD_RELAY_CMD;
	objRelayHead.code = 0;
	objRelayHead.len = 4;

	_MsgBody_  objMsgBody;
	objMsgBody.len = 0;
	if (bOpenflag)
	{
		objMsgBody.type = (char)CMD_RELAY_CMD_OPEN_STREAM;
	}
	else
	{
		objMsgBody.type = (char)CMD_RELAY_CMD_EXIT_RELAY;
	}
	
	objMsgBody.pContend = NULL;

	_reqData_t_  objreqData;
	objreqData.reqHeader = objRelayHead;
    strcpy(objreqData.pMsgBody,(char*)&objMsgBody);
	
	char tempBuf[1024*128];
	memset(tempBuf,0,1024*128);
	RelayMsgHandler objRelayMsgHandler;
	objRelayMsgHandler.assembleRelayMsg(objreqData,tempBuf);

	//开始发送消息到relaySvr
	CTCPBaseCommunication* pobjTCPBaseCommunication = (CTCPBaseCommunication*)pobjIOHandler->findCommunicationInstance(szSessionId);
	int nRet = pobjTCPBaseCommunication->sendData(tempBuf,strlen(tempBuf)); 
    
	if (nRet <0 )
	{
       int nCount = 0;
	   while (nCount++<10)
	   {
		  nRet = pobjTCPBaseCommunication->sendData(tempBuf,strlen(tempBuf));
		  if(nRet == 0)
			  break;
		  
		  //间隔一定时间重发
		  struct timeval timeout;
		  get_ReSend_Time(nCount,&timeout);
		  CIOHandler::setTimerBySelect(timeout);
	   }  
	}
	//如果是close stream需要清理资源
	if (!bOpenflag)
	{
		std::map<char*,int>::iterator itr = m_SessionIdFdMap.find(szSessionId);
		if(itr != m_SessionIdFdMap.end())
		{
			pobjIOHandler->clearIOFd(itr->second);
			m_SessionIdFdMap.erase(itr++);
			updateConnStatus(szSessionId,4);
		}
	}

}

void RelayHandler::setSessionManager(CSessionManager* pobjSessionManager)
{
	m_pobjSessionManager = pobjSessionManager;
}

void RelayHandler::getP2PHandler(P2PHandler* pobjP2PHandler)
{
	m_pobjP2PHandler = pobjP2PHandler;
}

//发送登录relaySvr前必须要有sessionmanager
void RelayHandler::loginUpSvr(char* szSessionId)
{
	if (NULL == m_pobjSessionManager)
	{
		//TODO 记日志
		return;
	}
	std::map<char*,int>::iterator itr = m_SessionIdFdMap.find(szSessionId);
	if(itr == m_SessionIdFdMap.end())
	{//log
		return;
	}
	//组装消息体
	relay_protocol_header_t  objRelayHead;
	objRelayHead.magic=MAGIC;
	objRelayHead.cmd = CMD_LOGIN;
	    
	char tempBuf[1024];
	int len =0;
	CUACInfo objUACInfo = m_pobjSessionManager->getUAC1Info();
#ifdef WIN32
	sprintf_s(tempBuf,strlen(objUACInfo.getDstUUID()),"%s\n",objUACInfo.getDstUUID());
	sprintf_s(tempBuf+strlen(objUACInfo.getDstUUID()),strlen(objUACInfo.getSrcUUID()),"%s",objUACInfo.getSrcUUID());
    len = strlen(objUACInfo.getDstUUID())+ strlen(objUACInfo.getSrcUUID());
#else	
	len += snprintf(tempBuf,strlen(objUACInfo.getDstUUID()),"%s\n",objUACInfo.getDstUUID());
    len += snprintf(tempBuf+strlen(objUACInfo.getDstUUID()),strlen(objUACInfo.getSrcUUID()),"%s",objUACInfo.getSrcUUID());
#endif
	if (NULL == tempBuf)
		return;

	objRelayHead.len = len;

	_reqData_t_ objRelayRequestData;
	memset(&objRelayRequestData,0,sizeof(_reqData_t_));
	objRelayRequestData.reqHeader = objRelayHead;
	memcpy(objRelayRequestData.pMsgBody,tempBuf,strlen(tempBuf));

	RelayMsgHandler objRelayMsgHandler;
	objRelayMsgHandler.assembleRelayMsg(objRelayRequestData,tempBuf);

    //开始发送消息到relaySvr
	CTCPBaseCommunication* pobjTCPBaseCommunication = (CTCPBaseCommunication*)pobjIOHandler->findCommunicationInstance(szSessionId);
	pobjTCPBaseCommunication->sendData(tempBuf,(int)strlen(tempBuf)); 
}

void RelayHandler::relayParseHandler(char* buf,int nLen)
{
	if (m_RelayHandler->m_SessionIdFdMap.empty())
	{
		return;
	}

	if (nLen< 8 )
	{
		//TODO log
		return;
	}
	//解析协议
	_respData_t_ objRespData;
	RelayMsgHandler objRelayMsgHandler;
    objRelayMsgHandler.parserRelayMsg(buf,objRespData);
    
	uint16_t cmdType = objRespData.respHeader.cmd;     					
	uint16_t extCode = objRespData.respHeader.code; 
	
	if(cmdType == CMD_LOGIN_RSP)
	{
		std::map<char*,int>::iterator itr = m_RelayHandler->m_SessionIdFdMap.begin();
		//这里判断注册是否成功
		if(extCode == ERRNO_OK)
		{
			//通知开流
			while(itr++ != m_RelayHandler->m_SessionIdFdMap.end())
			{
               (m_RelayHandler->requestOpenOrCloseStream)(itr->first,true);
			   m_RelayHandler->updateConnStatus(itr->first,1);
			}
		}
		else
		{
			//重新登录
			while(itr++ != m_RelayHandler->m_SessionIdFdMap.end())
			{
				(m_RelayHandler->loginUpSvr)(itr->first);
				m_RelayHandler->updateConnStatus(itr->first,0);
			}
		}

	}
	else if(cmdType == CMD_RELAY_STREAM_RSP)
	{
		std::map<char*,int>::iterator itr = m_RelayHandler->m_SessionIdFdMap.begin();
		if(extCode == ERRNO_OK)
		{
			//打开流正常不会回应，直接给流
			//注册监听函数，接收流
			m_RelayHandler->updateConnStatus(itr->first,2);
			int nSize = strlen(buf);
			(*(m_RelayHandler->pobjCallBackFun))(buf,nSize);
		}
		else if(extCode == EUNFOUNDDST)
		{
			//关闭relay
			while(itr++ != m_RelayHandler->m_SessionIdFdMap.end())
			{
				(m_RelayHandler->requestOpenOrCloseStream)(itr->first,false);
				m_RelayHandler->updateConnStatus(itr->first,4);
			}
		}
		else
		{
			//打开流异常,重新推流
			while(itr++ != m_RelayHandler->m_SessionIdFdMap.end())
			{
				(m_RelayHandler->requestOpenOrCloseStream)(itr->first,true);
				m_RelayHandler->updateConnStatus(itr->first,2);
			}
		}
	}
	else if(cmdType == CMD_RELAY_STREAM)
	{
		//通知已经收到流
        //该设置监听函数
       std::map<char*,int>::iterator itr = m_RelayHandler->m_SessionIdFdMap.begin(); 
	   m_RelayHandler->updateConnStatus(itr->first,3);
	   int nSize = strlen(buf);
	  (*(m_RelayHandler->pobjCallBackFun))(buf,nSize);	   
	}
}

int RelayHandler::findFdBySessionId(char* szSessionId)
{
	if (m_SessionIdFdMap.empty())
	{
		return -1;
	}
	std::map<char*,int>::iterator itr= m_SessionIdFdMap.find(szSessionId);
	if (itr != m_SessionIdFdMap.end())
	{
		return itr->second;
	}
	return -1;

}
int RelayHandler::findConnectStatusBySessionId(char* szSessionId)
{
	if (m_SessionIdConnStstusMap.empty())
	{
		return -1;
	}
	std::map<char*,int>::iterator itr=m_SessionIdConnStstusMap.find(szSessionId);
	if (itr != m_SessionIdConnStstusMap.end())
	{
		return itr->second;
	}
	return -1;
}

void RelayHandler::updateConnStatus(char* szSessionId,int nStatus)
{
	std::map<char*,int>::iterator itr=m_SessionIdConnStstusMap.find(szSessionId);
	if (itr != m_SessionIdConnStstusMap.end())
	{
		itr->second = nStatus;
	}
	m_SessionIdConnStstusMap.insert(std::make_pair(szSessionId,nStatus));
}

void RelayHandler::setRelayRecvCallFun(LISTENERFUN pobjFun)
{
	pobjCallBackFun = pobjFun;
}