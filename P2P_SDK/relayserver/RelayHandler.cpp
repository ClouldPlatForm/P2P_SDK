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
	//������Դ
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
	//���ӻص�����
	pobjUnifiedDispatchManager = CUnifiedDispatchManager::getInstance();
	pobjUnifiedDispatchManager->setHandler(relayParseHandler);

	//��ʼ���߳�,����relayʹ��һ���̴߳���
	pobjPThreadProcess = new CPThreadProcess();
	pobjPThreadProcess->setThreadFun(run,NULL);
	pobjPThreadProcess->createThread();
}

void RelayHandler::setTurnSvrAddr(char* szTurnIP,int nPort)
{
	strcpy(m_szTurnSvrIP,szTurnIP);
	m_nPort = nPort;
}

//��init֮ǰ�����Ȼ�ȡ��P2PHandler
int RelayHandler::initNet()
{
	//����RelaysessionId
	m_pobjSessionManager->setUAC1RelaySessionId();

	//����RelayRequestStream
	CUACInfo objUACInfo = m_pobjSessionManager->getUAC1Info();
	m_pobjP2PHandler->requestRelayStream(objUACInfo);
	//����TCP����
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
		//�쳣����
		if (NULL == relayIP)
		{
			//����־
			return -1;
		}
	}
    int locolPort = objUACInfo.getLocalPort();
	for(int i=0; i<tmpszSessionVec.size();i++)
	{
		//��ʼ�����磬����tcp����
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
		//��¼,�˴�����ܹ��ж��£���������
		loginUpSvr(const_cast<char*>(szSessionId));
	}
	//������
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
			//ÿ��200ms��ȡһ��P2Pserver������������Ϣ��
			bool bRet = (m_RelayHandler->pobjIOHandler)->pulse();
			if (!bRet)
			{
				return -1;
			}
		}
		
		////˯��200ms
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
		//û�ҵ�
		return;
	}
	//��װ��Ϣ��
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

	//��ʼ������Ϣ��relaySvr
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
		  
		  //���һ��ʱ���ط�
		  struct timeval timeout;
		  get_ReSend_Time(nCount,&timeout);
		  CIOHandler::setTimerBySelect(timeout);
	   }  
	}
	//�����close stream��Ҫ������Դ
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

//���͵�¼relaySvrǰ����Ҫ��sessionmanager
void RelayHandler::loginUpSvr(char* szSessionId)
{
	if (NULL == m_pobjSessionManager)
	{
		//TODO ����־
		return;
	}
	std::map<char*,int>::iterator itr = m_SessionIdFdMap.find(szSessionId);
	if(itr == m_SessionIdFdMap.end())
	{//log
		return;
	}
	//��װ��Ϣ��
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

    //��ʼ������Ϣ��relaySvr
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
	//����Э��
	_respData_t_ objRespData;
	RelayMsgHandler objRelayMsgHandler;
    objRelayMsgHandler.parserRelayMsg(buf,objRespData);
    
	uint16_t cmdType = objRespData.respHeader.cmd;     					
	uint16_t extCode = objRespData.respHeader.code; 
	
	if(cmdType == CMD_LOGIN_RSP)
	{
		std::map<char*,int>::iterator itr = m_RelayHandler->m_SessionIdFdMap.begin();
		//�����ж�ע���Ƿ�ɹ�
		if(extCode == ERRNO_OK)
		{
			//֪ͨ����
			while(itr++ != m_RelayHandler->m_SessionIdFdMap.end())
			{
               (m_RelayHandler->requestOpenOrCloseStream)(itr->first,true);
			   m_RelayHandler->updateConnStatus(itr->first,1);
			}
		}
		else
		{
			//���µ�¼
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
			//�������������Ӧ��ֱ�Ӹ���
			//ע�����������������
			m_RelayHandler->updateConnStatus(itr->first,2);
			int nSize = strlen(buf);
			(*(m_RelayHandler->pobjCallBackFun))(buf,nSize);
		}
		else if(extCode == EUNFOUNDDST)
		{
			//�ر�relay
			while(itr++ != m_RelayHandler->m_SessionIdFdMap.end())
			{
				(m_RelayHandler->requestOpenOrCloseStream)(itr->first,false);
				m_RelayHandler->updateConnStatus(itr->first,4);
			}
		}
		else
		{
			//�����쳣,��������
			while(itr++ != m_RelayHandler->m_SessionIdFdMap.end())
			{
				(m_RelayHandler->requestOpenOrCloseStream)(itr->first,true);
				m_RelayHandler->updateConnStatus(itr->first,2);
			}
		}
	}
	else if(cmdType == CMD_RELAY_STREAM)
	{
		//֪ͨ�Ѿ��յ���
        //�����ü�������
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