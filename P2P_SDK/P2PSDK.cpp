#include "P2PSDK.h"
#include <stdio.h>
#include <stdlib.h>
#include "common/commonUtil.h"
#include "common/protocol/CommunicationMsgOperater.h"
#include "base/TemplateEntityClass.h"
#include "base/communication/UDPClientBaseCommunication.h"
#include "base/communication/TCPClientBaseCommunication.h"

LISTENERFUN	P2PDevSDK::m_objTCPRecvListener = NULL;
LISTENERFUN	P2PDevSDK::m_objUDPRecvListener = NULL;
P2PDevSDK*  P2PDevSDK::m_objP2PDevSDK;
P2PDevSDK::P2PDevSDK(int nMode)
{
	m_objP2PDevSDK = this;
	m_nMode = nMode;
	m_szConfigPath = NULL;
    memset(m_szDevType,0,16);
	memset(m_szUUID,0,32);
	memset(m_szlocalIp,0,15);
	memset(m_szP2PSvrIP,0,15);
	memset(m_szStunSvrIP,0,15);
	//初始化工作类
	m_objSessionManager = new CSessionManager(); 
	m_objNatCrossHelper = new NatCrossHelper(m_objSessionManager);
	m_objP2PHandler     = new P2PHandler(m_objSessionManager,m_objNatCrossHelper);
	m_objNatCrossHelper->Attach(m_objP2PHandler);
	m_objRelayHandler   = new RelayHandler(m_objSessionManager,m_objP2PHandler);
	m_pobjIOHandler = CIOHandler::getInstance();
}

P2PDevSDK::~P2PDevSDK()
{
	cleanUp();
}

void P2PDevSDK::cleanUp()
{
	if (NULL != m_objSessionManager)
	{
		delete m_objSessionManager;
		m_objSessionManager = NULL;
	}
	if (NULL != m_objNatCrossHelper)
	{
		delete m_objNatCrossHelper;
		m_objNatCrossHelper =NULL;
	}
    if(NULL != m_objP2PHandler)
	{
		delete m_objP2PHandler;
		m_objP2PHandler = NULL;
	}
	if (NULL != m_pobjIOHandler)
	{
		delete m_pobjIOHandler;
		m_pobjIOHandler = NULL;
	}
	if (NULL != m_objRelayHandler)
	{
		delete m_objRelayHandler;
		m_objRelayHandler = NULL;
	}


}
void P2PDevSDK::setDevType(const char* szDevType)
{	
	strcpy(m_szDevType,szDevType);
}

void P2PDevSDK::setDevUUID(const char* szUUID)
{
	strcpy(m_szUUID,szUUID);
}

void P2PDevSDK::setDevStreamInfo(const int nStreamInfo)
{
	m_nStreamInfo = nStreamInfo;
}

void P2PDevSDK::setDevLocalIP(const char* szIP)
{
    strcpy(m_szlocalIp,szIP);
}

void P2PDevSDK::setDevLocalPort(const int nPort)
{
	m_nPort = nPort;
}

void P2PDevSDK::setP2PSvrAddr(char* szIp,int nPort)
{
	strcpy(m_szP2PSvrIP,szIp);
	m_nP2PSvrPort = nPort;
}

void P2PDevSDK::setStunSvrAddr(char* szIp,int nPort)
{
	strcpy(m_szStunSvrIP,szIp);
	m_nStunSvrPort = nPort;
}

void P2PDevSDK::setTurnSvrAddr(char* szIp,int nPort)
{
    m_objRelayHandler->setTurnSvrAddr(szIp,nPort);
}

void P2PDevSDK::setConfigPath(char* szPath)
{ 
   strcpy(m_szConfigPath,szPath);
}

void P2PDevSDK::init()
{
	//获取配置参数
	CUACInfo objSrcUACInfo;
	static char szP2PsessionId[10];
	int2str(m_objSessionManager->getP2PSessionId(),szP2PsessionId);
	objSrcUACInfo.setP2PSessionId(szP2PsessionId);
	objSrcUACInfo.setDevType(m_szDevType);
	objSrcUACInfo.setLocalIP(m_szlocalIp);
	objSrcUACInfo.setLocalPort(m_nPort);
	objSrcUACInfo.setStreamInfo(m_nStreamInfo);
	if (NULL == m_szConfigPath)
    {
		objSrcUACInfo.setSrcUUID(m_szUUID);
		if (m_nMode==1 || m_nMode==3)
		{
			m_objNatCrossHelper->setStunSvrAddr(m_szStunSvrIP,m_nStunSvrPort);
		}
		   m_objP2PHandler->setAddress(m_szP2PSvrIP,m_nP2PSvrPort);
    }
	else
	{
		//从配置文件获取参数
       CCommunicationMsgOperater objCommunicationMsgOperater;
	   CBaseEntityClass<char*> obj;
	   //初始化参数
	   std::map<char*,paramType> templateList;
	   templateList.insert(std::pair<char*,paramType>("MainInputParam",paramType(0)));
	   templateList.insert(std::pair<char*,paramType>("P2PServerIP",paramType(4)));
	   templateList.insert(std::pair<char*,paramType>("P2PServerPort",paramType(4)));
	   templateList.insert(std::pair<char*,paramType>("StunServerIP",paramType(4)));	
	   templateList.insert(std::pair<char*,paramType>("StunServerPort",paramType(4)));	
	   templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	   obj.initList(templateList);
	   int nRet = objCommunicationMsgOperater.parse_MainParam(m_szConfigPath,obj);
       if (0 == nRet)
       {
		  objSrcUACInfo.setSrcUUID(obj.getParamValue("uuid"));
		  int nLen = strlen(obj.getParamValue("uuid"));
		  memcpy(m_szUUID,obj.getParamValue("uuid"),32);
       }
	   if (m_nMode==1 || m_nMode==3)
	   {
		   m_objNatCrossHelper->setStunSvrAddr(obj.getParamValue("StunServerIP"),atoi(obj.getParamValue("StunServerPort")));
	   }
	   m_objP2PHandler->setAddress(obj.getParamValue("P2PServerIP"),atoi(obj.getParamValue("P2PServerPort")));
	}	
	m_objSessionManager->setUAC1Info(objSrcUACInfo);
	
}

void P2PDevSDK::setSrcUACInfo(CUACInfo objSrcUACInfo)
{
	m_objSessionManager->setUAC1Info(objSrcUACInfo);
}
int P2PDevSDK::run()
{
	int nRet = 0;
	nRet = P2PHandler::run();
	if (nRet !=0)
	{
		return nRet;
	}
	if (m_nMode==1)
	{
		nRet =NatCrossHelper::run(NULL);
		if (nRet !=0)
		{
			return nRet;
		}
	}
	else if (m_nMode==2)
	{
		nRet =RelayHandler::run();
		if (nRet !=0)
		{
			return nRet;
		}
	}
	else if (m_nMode==3)
	{
		nRet =NatCrossHelper::run(NULL);
		if (nRet !=0)
		{
			return nRet;
		}
		nRet =RelayHandler::run();
		if (nRet !=0)
		{
			return nRet;
		}
	}
	
	//睡眠200ms
	struct timeval timeout;
	timeout.tv_sec =0;
	timeout.tv_usec =200;
	CIOHandler::setTimerBySelect(timeout);	

   return 0;
}
int P2PDevSDK::start()
{
	init();
	int nRet = 0;
	(m_objSessionManager->getUAC1Info()).setSrcUUID(m_szUUID);
	nRet = m_objP2PHandler->initNet();
	if(nRet != 0 )
	   return nRet;
	if (m_nMode==1)
	{
		nRet = m_objNatCrossHelper->initNet();
		if(nRet != 0 )
		   return nRet;
	    m_pobjIOHandler->addUdpReadDataListener(recvMediaData);
	}
	else if(m_nMode==2)
	{
	   nRet = m_objRelayHandler->initNet();
		if(nRet != 0 )
		  return nRet;
		m_objRelayHandler->setRelayRecvCallFun(recvMediaData);
	}
	else if(m_nMode==3)
	{
		nRet = m_objNatCrossHelper->initNet();
		if(nRet != 0 )
		   return nRet;
		nRet = m_objRelayHandler->initNet();
		if(nRet != 0 )
		   return nRet;
		m_pobjIOHandler->addUdpReadDataListener(recvMediaData);
		m_objRelayHandler->setRelayRecvCallFun(recvMediaData);
	}
    //线程循环 
	while (nRet == 0)
      {
         nRet = run();
		 if (nRet!=0)
         {
			 return nRet;
         }
		 continue;
      }
      return 0;
}

void P2PDevSDK::stop()
{
	if (m_nMode==1)
	{
		m_objNatCrossHelper->uninit();
		m_pobjIOHandler->removeUdpReadDataListener();
	}
	else if(m_nMode==2)
	{
		m_objRelayHandler->uninit();
		m_pobjIOHandler->removeReadDataListener();
	}
	else if(m_nMode==3)
	{
		m_objNatCrossHelper->uninit();
		m_objRelayHandler->uninit();
		m_pobjIOHandler->removeUdpReadDataListener();
		m_pobjIOHandler->removeReadDataListener();
	}
	m_objP2PHandler->uninit();
	m_objSessionManager->uninit();
}

int P2PDevSDK::sendMediaDataByP2P(char* szDstUUID,char* buf,int nSize)
{
	int nStatus =  m_objNatCrossHelper->findConnectStatusByUUID(szDstUUID);
	if (nStatus<0)
	{
		return -1;
	}
	int nfd = m_objNatCrossHelper->findFdFromUDPChannel(szDstUUID);
	if (-1 == nfd)
	{
		return -1;
	}
	CUDPClientBaseCommunication* pobjUDPClientBaseCommunication = (CUDPClientBaseCommunication*)m_pobjIOHandler->findUDPCommunicationInstance(nfd);
	int nRet = pobjUDPClientBaseCommunication->sendData(buf,nSize);
    return nRet;
}

int P2PDevSDK::sendMediaDataByRelay(char* szDstUUID,char* buf,int nSize)
{
	CUACInfo objSrcUACInfo = m_objSessionManager->getUAC1Info();
	if (!strcmp(objSrcUACInfo.getSrcUUID(),szDstUUID))
	{
		char* szRelaySessionId = objSrcUACInfo.getSessionId()[0];
		int nStatus = m_objRelayHandler->findConnectStatusBySessionId(szRelaySessionId);
		if (2!=nStatus || 3!=nStatus)
			return -1;

		int nfd = m_objRelayHandler->findFdBySessionId(szRelaySessionId);
		CTCPBaseCommunication* pobjTCPBaseCommunication = (CTCPBaseCommunication*)m_pobjIOHandler->findCommunicationInstance(nfd);
		int nRet = pobjTCPBaseCommunication->sendData(buf,nSize);
		return nRet;
	}
	return -1;
}
int P2PDevSDK::sendMediaData(char* szDstUUID,char* buf,int nSize)
{
	if (m_nMode==1)
	{
		return sendMediaDataByP2P(szDstUUID,buf,nSize);
	}
	else if(m_nMode==2)
	{
		//暂支持1对1情况
		return sendMediaDataByRelay(szDstUUID,buf,nSize);
	}
	else if(m_nMode==3)
	{
		int nStatus =  m_objNatCrossHelper->findConnectStatusByUUID(szDstUUID);
		if (nStatus >0)
		{
			return sendMediaDataByP2P(szDstUUID,buf,nSize);
		}
		else
		{
			return sendMediaDataByRelay(szDstUUID,buf,nSize);		
		}
	}
	return -1;
}


int P2PDevSDK::recvMediaData(char* buf,int& nSize)
{
	if (nSize <=0)
	{
		return -1;
	}	
	if (NULL != m_objUDPRecvListener)
	{
		(*(m_objP2PDevSDK->m_objUDPRecvListener))(buf,nSize);
	}
	else
	{
		if (NULL == m_objTCPRecvListener)
		{
			return -1;
		}
		(*(m_objP2PDevSDK->m_objTCPRecvListener))(buf,nSize);
	}
	return nSize;
}

void P2PDevSDK::setTCPRecvMediaFun(LISTENERFUN objRecvListener)
{
	m_objTCPRecvListener = objRecvListener;
}

void P2PDevSDK::setUDPRecvMediaFun(LISTENERFUN objRecvListener)
{
	m_objUDPRecvListener = objRecvListener;
}

void P2PDevSDK::setDevInfo(const char* szDevType,const char* szUUID,const char* szIP,const int nPort,const int nStreamInfo)
{
	if (NULL != szDevType)
	{
		setDevType(szDevType);
	}
	if (NULL != szUUID)
	{
	    setDevUUID(szUUID);
	}
	if (nStreamInfo>0)
	{
	    setDevStreamInfo(nStreamInfo);
	}
	if (NULL != szIP)
	{
		setDevLocalIP(szIP);
	}
	if (nPort>0)
	{
		setDevLocalPort(nPort);
	}	
}