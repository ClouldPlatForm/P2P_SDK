#include "P2PHandler.h"
#include "../base/communication/TCPClientBaseCommunication.h"
#include "../base/communication/IOHandler.h"
#include "../common/protocol/CommunicationMsgOperater.h"
#include "../common/UnifiedDispatchManager.h"
#include "../base/Timer.h"
#include "../base/PThreadProcess.h"
#include "../natcross/StunUtil.h"
#include "../common/const/NatConst.h"
#include "../common/SessionManager.h"
#include "../natcross/NatCrossHelper.h"

P2PHandler* P2PHandler::m_P2PHandler = NULL;
P2PHandler::P2PHandler()
{
	m_P2PHandler = this;
	pobjUnifiedDispatchManager = CUnifiedDispatchManager::getInstance();
	pobjCommunicationMsgOperater = new CCommunicationMsgOperater();
	bNatTypeIsChanged = false;
	bRouterInfoParamIsChanged = false;
	bNatBeatHoleResult = false;
	memset(m_szRelayIP,0,15);
	memset(szIPAddress,0,15);
	objP2PHearBeat = new CHearBeat();
	init();
}

P2PHandler::P2PHandler(CSessionManager* pobjSessionManager,NatCrossHelper* pobjNatCrossHelper)
{
	m_P2PHandler = this;
	pobjUnifiedDispatchManager = CUnifiedDispatchManager::getInstance();
	pobjCommunicationMsgOperater = new CCommunicationMsgOperater();
	bNatTypeIsChanged = false;
	setSessionManager(pobjSessionManager);
	m_pobjNatCrossHelper = pobjNatCrossHelper;
	bRouterInfoParamIsChanged = false;
	bNatBeatHoleResult = false;
	memset(m_szRelayIP,0,15);
	objP2PHearBeat = new CHearBeat();
	init();
}

void P2PHandler::init()
{
	pobjIOHandler = CIOHandler::getInstance();
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec =300;
	pobjIOHandler->setTimeout(timeout);

	//�˴����Ӵ���ص�����
	pobjUnifiedDispatchManager->setP2PHandler1(p2pParseHandler);
	//��ʼ����ʱ��
	pobjTimer = new CTimer();
	pobjTimer->setIntervalTime(30);
	pobjTimer->setTimerProc(TimerProc);

	//��ʼ���߳�
	pobjPThreadProcess = new CPThreadProcess();
	pobjPThreadProcess->setThreadFun(run,NULL);
	pobjPThreadProcess->createThread();
}

P2PHandler::~P2PHandler()
{
	if(NULL != pobjCommunicationMsgOperater)
	{
		delete pobjCommunicationMsgOperater;
		pobjCommunicationMsgOperater=NULL;
	}
	if(NULL != pobjTimer)
	{
		delete pobjTimer;
		pobjTimer = NULL;
	}
	if (NULL != pobjPThreadProcess)
	{
		delete pobjPThreadProcess;
		pobjPThreadProcess = NULL;
	}
	if(NULL != objP2PHearBeat)
	{
        delete objP2PHearBeat;
		objP2PHearBeat = NULL;
	}
	bNatTypeIsChanged = false;

	uninit();
}

void P2PHandler::setAddress(const char* IP,const int port)
{
	strcpy(szIPAddress,IP);
	nPort = port;
}

//֧��ͬһ���豸����P2PServer����������·�ͻỰ
void P2PHandler::setDeviceInfo(CDeviceInfo& deviceInfo)
{
	char* szSrcUUID = deviceInfo.getUUID();
	objP2PHearBeat->setSrcUUID(szSrcUUID);
	objP2PHearBeat->setHearBeatSessionID(deviceInfo.getSessionId());
	objP2PHearBeat->setHearBeatStatus(-1);
	objChannelInfoMap.insert(std::make_pair(&deviceInfo,-1));
}

void P2PHandler::uninit()
{
	if( objChannelInfoMap.size() == 0 )
	{
		return;
	}
   std::map<CDeviceInfo*,int>::iterator itr = objChannelInfoMap.begin();
   while ( itr != objChannelInfoMap.end() )
  {
	   pobjIOHandler->clearIOFd(itr->second);
	   itr++;
  }
  objChannelInfoMap.clear();
//  (m_pobjSessionManager->getUAC1Info()).clearP2PSessionId();
}

int P2PHandler::initNet()
{
	objSrcUACInfo = m_pobjSessionManager->getUAC1Info();
	//CDeviceInfo objDeviceinfo;
	std::vector<char*> objSessionIdVec = objSrcUACInfo.getP2PSessionId();
	if (NULL == objSrcUACInfo.getDevType() || NULL == objSrcUACInfo.getSrcUUID() || 0 == objSessionIdVec.size())
	{
		return -1;
	}
	m_objDeviceInfo.setDevType(objSrcUACInfo.getDevType());
	m_objDeviceInfo.setUUID(objSrcUACInfo.getSrcUUID());
	
	for (int i=0;i<objSessionIdVec.size();i++)
	{
		m_objDeviceInfo.setSessionId(objSessionIdVec[i]);
		setDeviceInfo(m_objDeviceInfo);
	}
    //�ݴ���
	if (0 == objChannelInfoMap.size())
	{
		return -2;
	}
	//1������socket TCP����
	std::map<CDeviceInfo*,int>::iterator itr = objChannelInfoMap.begin();
	int i=0;//�������ض˿�����
	while(itr != objChannelInfoMap.end())
	{
		//��ʼ�����磬����tcp����
		CTCPBaseCommunication*  objTCPBaseCommunication = new CTCPBaseCommunication();
		objTCPBaseCommunication->setLocalPort(LOCALPORT+i);
		i++;
		objTCPBaseCommunication->init(szIPAddress,nPort);
		int fd = objTCPBaseCommunication->getSockFd();
		itr->second = fd;
		char* szSessionId = (itr->first)->getSessionId();
		pobjIOHandler->addReadSockFd(const_cast<char*>(szSessionId),fd);
		pobjIOHandler->addWriteSockFd(fd);
		pobjIOHandler->setClientCommunication(objTCPBaseCommunication,szSessionId);

		//�ϱ��豸��Ϣ��P2PServer
		 char* szuuid = (itr->first)->getUUID();
		 char* szdevtype = (itr->first)->getDevType();
		ReportDeviceInfoToP2PServer(szSessionId,szuuid,szdevtype);
		itr++;
	}	
	return 0;
}

CTCPBaseCommunication* P2PHandler::getCommunicationInstnce(char* sessionId)
{
	return (CTCPBaseCommunication*)pobjIOHandler->findCommunicationInstance(sessionId);
}

CTCPBaseCommunication* P2PHandler::getCommunicationInstnce(int fd)
{
	return (CTCPBaseCommunication*)pobjIOHandler->findCommunicationInstance(fd);
}

int P2PHandler::sendData(const CBaseEntityClass<char*> objBaseEntity)
{
	//��װxml��buffer
	char buf[1024];
	memset(buf,0,1024);
	(m_P2PHandler->pobjCommunicationMsgOperater)->assemble_ProtocolInfo(objBaseEntity,buf);

	//��ȡTCPͨ��
	char* sessionId = objBaseEntity.getParamValue("sessionId");	
	CTCPBaseCommunication* pobjTCPBaseCommunication = getCommunicationInstnce(sessionId);
	if (NULL == pobjTCPBaseCommunication)
	{
		return -1;
	}
	int nSize = strlen(buf);
	int nRet = pobjTCPBaseCommunication->sendData(buf,nSize);
	//�ط�����
	int reSendNum = 3;
	while (nRet <= 0 && (reSendNum--) > 0)
	{
		printf("errno =%d\n",errno);
		nRet = pobjTCPBaseCommunication->sendData(buf,strlen(buf));
	}
	return nRet;
}

int P2PHandler::sendData(char* sessionId,char* buf)
{
	CTCPBaseCommunication* pobjTCPBaseCommunication = getCommunicationInstnce(sessionId);
	int nRet = pobjTCPBaseCommunication->sendData(buf,strlen(buf));
	//�ط�����
	int reSendNum = 3;
	while (nRet <= 0 && (reSendNum--) <0)
	{
		nRet = pobjTCPBaseCommunication->sendData(buf,strlen(buf));
	}
	return nRet;
}

int P2PHandler::ReportP2PResponse2()
{
	//��װ�豸��ϢXML����
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("P2pResponse2",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("destUUID",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("srcUUID",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("localIP",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("localPort",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("remoteIP",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("remotePort",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("Status",paramType(-1)));
	templateList.insert(std::pair<char*,paramType>("statusCode",paramType(9)));
	templateList.insert(std::pair<char*,paramType>("statusStr",paramType(9)));
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//��дxml�ڵ�����
	CUACInfo objUACInfo = m_pobjNatCrossHelper->GetUACInfo();
	objBaseEntity.setParamValue("sessionId",(objUACInfo.getP2PSessionId())[0]);
	objBaseEntity.setParamValue("destUUID",objUACInfo.getDstUUID());
	objBaseEntity.setParamValue("srcUUID",objUACInfo.getSrcUUID());

	RouterInfo objRouterInfo = m_pobjNatCrossHelper->GetRouterInfo();
	objBaseEntity.setParamValue("localIP",objRouterInfo.getLocalIPAddr());
	char szPort[8];  
	int2str(objRouterInfo.getLocalPortAddr(),szPort);
	objBaseEntity.setParamValue("localPort",szPort);
	objBaseEntity.setParamValue("remoteIP",objRouterInfo.getPubIPAddr());
	int nPubPort = objRouterInfo.getPubPortAddr();
	char szPubPort[16];
	int2str(nPubPort,szPubPort);
	objBaseEntity.setParamValue("remotePort",szPubPort);
	char szStatusCode[8];
	int2str(objRouterInfo.getNatBeatHoleResult(),szStatusCode);
	objBaseEntity.setParamValue("statusCode",szStatusCode);
	char  szStatusStr[256] ;
	memset(szStatusStr,0,256);
	strcpy(szStatusStr,const_cast<char*>(objRouterInfo.getNatBeatHoleString().c_str()));
	objBaseEntity.setParamValue("statusStr",szStatusStr);
   //��������
	int nRet = sendData(objBaseEntity);
	return nRet;
}

void P2PHandler::ReportHearBeat()
{
	//��װ�豸��ϢXML����
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("Heartbeat",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//��дxml�ڵ�����
	CUACInfo objSrcUACInfo = m_pobjSessionManager->getUAC1Info();
	objBaseEntity.setParamValue("sessionId",objSrcUACInfo.getP2PSessionId()[0]);
	objBaseEntity.setParamValue("uuid",objSrcUACInfo.getSrcUUID());
	//��������
	sendData(objBaseEntity);
}

void P2PHandler::ReportDeviceInfoToP2PServer(char* szSessionId,char* szuuid,char* szdevtype)
{
	//��װ�豸��ϢXML����
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("UpdateDevInfo",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("devType",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//��дxml�ڵ�����
	objBaseEntity.setParamValue("sessionId",szSessionId);
	objBaseEntity.setParamValue("uuid",szuuid);
	objBaseEntity.setParamValue("devType",szdevtype);
	//��������
	sendData(objBaseEntity);
}

void P2PHandler::ReportDevNatType(char* szSessionId,char* szUUID,char* szNatType)
{
	//��װ�豸��ϢXML����
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("UpdateNatType",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("natType",paramType(4)));
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//��дxml�ڵ�����
	objBaseEntity.setParamValue("sessionId",szSessionId);
	objBaseEntity.setParamValue("uuid",szUUID);
	objBaseEntity.setParamValue("natType",szNatType);
	//��������
	sendData(objBaseEntity);
	bNatTypeIsChanged = false;
}

//�ط�ʹ��
void P2PHandler::ReportDevNatType(CBaseEntityClass<char*> tempObjBaseEntity)
{
	//��װ�豸��ϢXML����
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("UpdateNatType",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("natType",paramType(4)));
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//��дxml�ڵ�����
	objBaseEntity.setParamValue("sessionId",tempObjBaseEntity.getParamValue("sessionId"));
	objBaseEntity.setParamValue("uuid",tempObjBaseEntity.getParamValue("uuid"));
	objBaseEntity.setParamValue("natType",m_szNatType);
	//��������
	sendData(objBaseEntity);
	bNatTypeIsChanged = false;
}

//��ʱ��������,һ����ʱ�ر����е�����
void P2PHandler::TimerProc()
{
	if(m_P2PHandler->objChannelInfoMap.empty())
	{
	  return;
	}
	//��ʱ�����Ĵ���
	if( (m_P2PHandler->objP2PHearBeat)->getHearBeatStatus() ==0 )
	{
		std::map<CDeviceInfo*,int>::iterator itr = (m_P2PHandler->objChannelInfoMap).begin();
		while(itr != (m_P2PHandler->objChannelInfoMap).end())
		{
			//�ر����е�����
			(m_P2PHandler->pobjIOHandler)->clearIOFd(itr->second);
		    itr++;
		}
	}
	else
	{
		//���������Ĵ��� 
		m_P2PHandler->ReportHearBeat();	
		(m_P2PHandler->objP2PHearBeat)->setHearBeatStatus(1); 
	}

}

//P2P��������
int P2PHandler::run(void* lpParam)
{
	   if(m_P2PHandler->objChannelInfoMap.size() != 0)
		{
		  CTimer::run();
		 //ÿ��200ms��ȡһ��P2Pserver������������Ϣ��
		 bool bFlag = (m_P2PHandler->pobjIOHandler)->pulse();
		  if (!bFlag)
		  {
			  return -1;
		  }
		   
		 //���NatType�����仯���ϱ�NatType
		 //if (true == (m_P2PHandler->bNatTypeIsChanged) )
		 //{
			////ͬһ�豸�ϵ�NatType,UUID����һ����
			//std::map<CDeviceInfo*,int>::iterator itr = (m_P2PHandler->objChannelInfoMap).begin();
			//while (itr != (m_P2PHandler->objChannelInfoMap).end())
			//{   //�ϱ�һ�ξͿ�����
			//	CDeviceInfo* objDeviceInfo = itr->first;
			//   (m_P2PHandler->ReportDevNatType)(( const_cast<char*>(objDeviceInfo->getSessionId())),const_cast<char*>(objDeviceInfo->getUUID()),(m_P2PHandler->m_szNatType) );
			//	itr++;
			//}
			//m_P2PHandler->bNatTypeIsChanged = false;
		 //}
		 //������յ��µ�·����Ϣ
		 if (true == m_P2PHandler->bRouterInfoParamIsChanged)
		 {
			//����P2PRequest
			 char szStep[8] ;
			 int nStep = (m_P2PHandler->m_objRouterInfo).getPortChangStep();
			 int2str(nStep,szStep);
			 char szStreamInfo[8];
			int2str((m_P2PHandler->objSrcUACInfo).getStreamInfo(),szStreamInfo);
			m_P2PHandler->requestRemoteAddrToP2PSvr(m_P2PHandler->objSrcUACInfo,m_P2PHandler->m_szNatType,szStep,szStreamInfo);
			m_P2PHandler->bRouterInfoParamIsChanged = false;
		 }
		 //����򶴳ɹ�������P2PResponse2
		 if (m_P2PHandler->bNatBeatHoleResult)
		 {
			int nRet = m_P2PHandler->ReportP2PResponse2();
			if (nRet > 0)
			{
				m_P2PHandler->bNatBeatHoleResult = false;
			}
		 }
		}
		 ////˯��200ms
		 // struct timeval timeout;
		 // timeout.tv_sec =0;
		 // timeout.tv_usec =200;
		 // CIOHandler::setTimerBySelect(timeout);

		  return 0;
//	}
}

void P2PHandler::Update(CUACInfo objUACInfo,RouterInfo objRouterInfo)
{
	objSrcUACInfo = objUACInfo;
	m_objRouterInfo = objRouterInfo;
	setDevNatType(objRouterInfo.getNatType());
	bRouterInfoParamIsChanged = true;
	//�򶴴��ڻ�ȡ��NAT��ַ�����ͽ׶Σ���һ����Ҫ��ʼ����P2PRequest
	if(1 == m_objRouterInfo.getNatBeatHoleResult())
	{
		bNatBeatHoleResult = true;
	}
	else
	{
		bNatBeatHoleResult = false;
	}


}

void P2PHandler::Update()
{
	m_pobjNatCrossHelper->getP2PInfo();
   //����P2PRequest2����
	char* szMsgType = m_pobjNatCrossHelper->GetMsgType();
	if (NULL == szMsgType)
	{
		return;
	}
	if(!strcmp(szMsgType,"P2PRequest2"))
	{
		CUACInfo srcUACInfo = m_pobjNatCrossHelper->GetUACInfo();
		notifyOtherPeerByP2PSvr(srcUACInfo);
	}
}

void P2PHandler::p2pParseHandler(CBaseEntityClass<char*> objBaseEntity,char* szMsgType)
{
  char* szSessionId = objBaseEntity.getParamValue("sessionId");
  std::map<CDeviceInfo*,int>::iterator itr = (m_P2PHandler->objChannelInfoMap).begin();
  while(itr != (m_P2PHandler->objChannelInfoMap).end())
  {
	 const char* tmpSessionId = (itr->first)->getSessionId();
	 if( 0 ==strcmp(szSessionId,tmpSessionId) )
	{
	 
	  if( 0 ==strcmp(szMsgType,"MsgResponse") || 0 ==strcmp(szMsgType,"P2pHeartbeat")  || 0 ==strcmp(szMsgType,"Heartbeat"))
	  {
		(m_P2PHandler->objP2PHearBeat)->setHearBeatStatus(1);
		//�����ظ�
/*		char* buf = NULL;
		(m_P2PHandler->pobjCommunicationMsgOperater)->assemble_ProtocolInfo(objBaseEntity,buf);
		if (NULL == buf)
		{
			return;
		}
		char* sessionId = objBaseEntity.getParamValue("sessionId");	
		(m_P2PHandler->sendData)(sessionId,buf);	
*/
	  }
     //����MsgResponse1 ��Ӧ�ϴ��豸��Ϣ 
	 if( 0 ==strcmp(szMsgType,"MsgResponse1") )
	  {
		 (m_P2PHandler->objP2PHearBeat)->setHearBeatStatus(1);
		::domainNameToIPAddress(objBaseEntity.getParamValue("relayIP"),m_P2PHandler->m_szRelayIP);
		m_P2PHandler->m_nRelayPort = atoi(objBaseEntity.getParamValue("relayPort"));
		//����Relay��IP,port
		m_P2PHandler->m_pobjNatCrossHelper->setStunSvrAddr(m_P2PHandler->m_szRelayIP, m_P2PHandler->m_nRelayPort);
		//�ϱ�ʧ�����·���
		if ( 0 != atoi(objBaseEntity.getParamValue("statusCode")) )
		{
			m_P2PHandler->ReportDeviceInfoToP2PServer(szSessionId,objBaseEntity.getParamValue("uuid"),objBaseEntity.getParamValue("devType"));
		}
	  }
	 //����MsgResponse2 ��ӦnatType�ϱ�
     if ( 0 ==strcmp(szMsgType,"MsgResponse2"))
      {
		 (m_P2PHandler->objP2PHearBeat)->setHearBeatStatus(1);
		 //�ϱ�ʧ�����·���,û�п��Ǵ������������޸�
		if ( 0 != atoi(objBaseEntity.getParamValue("statusCode")) )
		{
			(m_P2PHandler->ReportDevNatType)(objBaseEntity);
		}
      }
	 if ( 0 ==strcmp(szMsgType,"P2pRequest") )
	 {
         (m_P2PHandler->objP2PHearBeat)->setHearBeatStatus(1);
		 //֪ͨNatģ�飬�Է��豸�Ĵ���Ϣ
		 CUACInfo objUACInfo;
		 objUACInfo.setP2PSessionId(szSessionId);
		 m_P2PHandler->setDstUACInfoValue(objBaseEntity,objUACInfo);
		
		 m_P2PHandler->m_pobjNatCrossHelper->SetDstUACInfo(objUACInfo);//��ǰ��Ŀ���û���Ϣ
		 m_P2PHandler->m_pobjSessionManager->setUAC2InfoVec(objUACInfo,objBaseEntity.getParamValue("destUUID"));//�洢����Ŀ���û���Ϣ

         RouterInfo objRouterInfo;
		 m_P2PHandler->setDstRouterInfoValue(objBaseEntity,objRouterInfo);
		
		 m_P2PHandler->m_pobjNatCrossHelper->SetDstRouterInfo(objRouterInfo);
		 m_P2PHandler->m_pobjNatCrossHelper->SetMsgType("P2pRequest");
		 m_P2PHandler->m_pobjSessionManager->setUAC2RouterInfoVec(objRouterInfo,objBaseEntity.getParamValue("destUUID"));
	}
	 //����P2pResponse1��P2pResponse2
	 if ( 0 ==strcmp(szMsgType,"P2pResponse1") )
	  {
		  (m_P2PHandler->objP2PHearBeat)->setHearBeatStatus(1);
		  //�����Ϊ0����ʾ������豸�����߻���������
		 int nRet =atoi(objBaseEntity.getParamValue("statusCode"));
		 if ( 0 != nRet )
		 {
			//todo ֪ͨNatģ�飬�Է��豸������
             m_P2PHandler->m_pobjNatCrossHelper->SetMsgType("P2pRequest");
			 m_P2PHandler->m_pobjNatCrossHelper->SetStatus(nRet);
		 }
		  (m_P2PHandler->objP2PHearBeat)->setHearBeatStatus(1);
	  }
	 //
	 if( 0 ==strcmp(szMsgType,"P2pResponse2") )
	 {
		 (m_P2PHandler->objP2PHearBeat)->setHearBeatStatus(1); 
		 //�����Ϊ0��
		 if ( 0 != atoi(objBaseEntity.getParamValue("statusCode")) )
		 {
			 //todo ֪ͨNatģ�飬��֪�ĸ�·�ɵĴ򶴽��
			 m_P2PHandler->m_pobjNatCrossHelper->SetMsgType("P2pResponse2");
			 int nRet =atoi(objBaseEntity.getParamValue("statusCode"));
			 m_P2PHandler->m_pobjNatCrossHelper->SetStatus(nRet);	

			 CUACInfo objUACInfo;
			 objUACInfo.setP2PSessionId(szSessionId);
			 m_P2PHandler->setDstUACInfoValue(objBaseEntity,objUACInfo);
			 m_P2PHandler->m_pobjNatCrossHelper->SetDstUACInfo(objUACInfo);//��ǰ��Ŀ���û���Ϣ

			 RouterInfo objRouterInfo;
			 objRouterInfo.setNatBeatHoleResult(nRet);
			 m_P2PHandler->setDstRouterInfoValue(objBaseEntity,objRouterInfo);
			 m_P2PHandler->m_pobjNatCrossHelper->SetDstRouterInfo(objRouterInfo);
		 }

	 }
	 //����P2pClose
	 if ( 0 ==strcmp(szMsgType,"P2pClose"))
	  {
		//�ر�����
		 (m_P2PHandler->pobjIOHandler)->clearIOFd(const_cast<char*>(tmpSessionId));
		 //����P2P����
		 (m_P2PHandler->objP2PHearBeat)->setHearBeatStatus(0);
	  }
	
   }	
   itr++;
 }
}

void P2PHandler::setDstUACInfoValue(CBaseEntityClass<char*> objBaseEntity,CUACInfo& objUACInfo)
{
	objUACInfo.setSrcUUID(objBaseEntity.getParamValue("srcUUID"));
	objUACInfo.setDstUUID(objBaseEntity.getParamValue("destUUID"));
	objUACInfo.setLocalIP(objBaseEntity.getParamValue("localIP"));
	objUACInfo.setLocalPort((int)objBaseEntity.getParamValue("localPort"));
	objUACInfo.setRemoteIP(objBaseEntity.getParamValue("remoteIP"));
	objUACInfo.setRemotePort((int)objBaseEntity.getParamValue("remotePort"));
	objUACInfo.setStreamInfo((int)objBaseEntity.getParamValue("streamInfo"));
}

void P2PHandler::setDstRouterInfoValue(CBaseEntityClass<char*> objBaseEntity,RouterInfo& objRouterInfo)
{
	objRouterInfo.setSrcUUIDVec(objBaseEntity.getParamValue("srcUUID"));
	objRouterInfo.setNatType((int)objBaseEntity.getParamValue("natType"));
	objRouterInfo.setPortChangStep((int)objBaseEntity.getParamValue("step"));
	objRouterInfo.setLocalIPAddr(objBaseEntity.getParamValue("localIP"));
	objRouterInfo.setLocalPortAddr((int)objBaseEntity.getParamValue("localPort"));
	objRouterInfo.setPubIPAddr(objBaseEntity.getParamValue("remoteIP"));
	objRouterInfo.setPubPortAddr((int)objBaseEntity.getParamValue("remotePort"));
}
//relay������ relayQuest
void P2PHandler::requestRelayStream(CUACInfo srcUACInfo)
{
	//��װ�豸��ϢXML����
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("RelayRequest",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("destUUID",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("srcUUID",paramType(4)));
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//��дxml�ڵ�����
	//objBaseEntity.setParamValue("sessionId",const_cast<char*>(srcUACInfo.getSessionId()[0]));
	objBaseEntity.setParamValue("destUUID",const_cast<char*>(srcUACInfo.getDstUUID()));
	objBaseEntity.setParamValue("srcUUID",const_cast<char*>(srcUACInfo.getSrcUUID()));
	//��������
	std::vector<char*> obj = srcUACInfo.getSessionId();
	for (int i=0;i<obj.size();i++)
	{
		objBaseEntity.setParamValue("sessionId",const_cast<char*>(obj[i]));
		sendData(objBaseEntity);	
	}
}

void P2PHandler::requestRemoteAddrToP2PSvr(CUACInfo srcUACInfo,char* szNatType,char* szStep,char* szStreamInfo)
{
	//��װ�豸��ϢXML����
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("P2pRequest",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("destUUID",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("srcUUID",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("localIP",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("localPort",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("remoteIP",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("remotePort",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("natType",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("step",paramType(4)));//̽�ⲽ��	
	templateList.insert(std::pair<char*,paramType>("streamInfo",paramType(4)));//��������ֱ���
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//��дxml�ڵ�����
	//objBaseEntity.setParamValue("sessionId",const_cast<char*>(srcUACInfo.getSessionId()[0]));
	objBaseEntity.setParamValue("destUUID",const_cast<char*>(srcUACInfo.getDstUUID()));
	objBaseEntity.setParamValue("srcUUID",const_cast<char*>(srcUACInfo.getSrcUUID()));
	objBaseEntity.setParamValue("localIP",const_cast<char*>(srcUACInfo.getLocalIP()));
	char temSzLocalPort[8];
	int2str(srcUACInfo.getLocalPort(),temSzLocalPort);
	objBaseEntity.setParamValue("localPort",temSzLocalPort);
	objBaseEntity.setParamValue("remoteIP",const_cast<char*>(srcUACInfo.getRemoteIP()));
    char temSzRemotePort[8];
    int2str(srcUACInfo.getRemotePort(),temSzRemotePort);
	objBaseEntity.setParamValue("remotePort",temSzRemotePort);
	objBaseEntity.setParamValue("natType",szNatType);
	objBaseEntity.setParamValue("step",szStep);
	objBaseEntity.setParamValue("streamInfo",szStreamInfo);
	//��������
	std::vector<char*> obj = srcUACInfo.getP2PSessionId();
//	for (int i=0;i<obj.size();i++)
//	{
		objBaseEntity.setParamValue("sessionId",const_cast<char*>(obj[0]));
		sendData(objBaseEntity);	
//	}
}

//�豸������Ϣ֪ͨ�Զ� p2prequest2
void P2PHandler::notifyOtherPeerByP2PSvr(CUACInfo srcUACInfo)
{
	//��װ�豸��ϢXML����
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("P2pRequest2",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("destUUID",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("srcUUID",paramType(4)));
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//��дxml�ڵ�����
	//objBaseEntity.setParamValue("sessionId",const_cast<char*>(srcUACInfo.getSessionId()[0]));
	objBaseEntity.setParamValue("destUUID",const_cast<char*>(srcUACInfo.getDstUUID()));
	objBaseEntity.setParamValue("srcUUID",const_cast<char*>(srcUACInfo.getSrcUUID()));
	//��������
	std::vector<char*> obj = srcUACInfo.getSessionId();
	for (int i=0;i<obj.size();i++)
	{
		objBaseEntity.setParamValue("sessionId",const_cast<char*>(obj[i]));
		sendData(objBaseEntity);	
	}
}

void P2PHandler::setSessionManager(CSessionManager* pobjSessionManager)
{
	m_pobjSessionManager = pobjSessionManager;
}