#include "NatCrossHelper.h"
#include "NatDetectionHelper.h"
#include "../common/SessionManager.h"
#include "../common/protocol/CommunicationMsgOperater.h"
#include "../base/communication/IOHandler.h"
#include "../common/UnifiedDispatchManager.h"
#include "../base/Timer.h"
#include "../common/const/NatConst.h"

#define RAND_PORT_OFFSET (1000)
#define PORT_OFFSET_WIGHT (20)
#define MAX_PORT (65534)
#define MIN_PORT (1024)
#define PORT_OFFSET (400)
#define PORT_OFFSET_STEP (100)
using namespace natconst;

NatCrossHelper*   NatCrossHelper::m_NatCrossHelper;

NatCrossHelper::NatCrossHelper(CSessionManager* pobjSessionManager)
:m_nbeatHoleStep(NATINFOISINVALID)
,m_nCurTime(0)
,m_ndetectionType(natconst::detectionType::FULLMODE)
{
	m_pobjSessionManager = pobjSessionManager;
	pobjTimer = NULL;
	m_szStunSvrIP = NULL;
	m_SessionFdMap.clear();
	m_udpConnectedChannelMap.clear();
	m_pobjIOHandler = CIOHandler::getInstance();
	m_NatCrossHelper = this;
	init();
}

NatCrossHelper::~NatCrossHelper()
{
	
	if (NULL != pobjPThreadProcess)
	{
		delete pobjPThreadProcess;
		pobjPThreadProcess = NULL;
	}
    if (NULL != pobjTimer)
    {
		delete pobjTimer;
		pobjTimer = NULL;
    }
}

void NatCrossHelper::setStunSvrAddr(char* hostName,int nPort)
{
	m_szStunSvrIP = hostName;
	m_nStunSvrPort = nPort;
}

void NatCrossHelper::init()
{
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20;
	m_pobjIOHandler->setUdpTimeout(timeout);

	//�˴����Ӵ���ص�����
	pobjUnifiedDispatchManager = CUnifiedDispatchManager::getInstance();
	pobjUnifiedDispatchManager->setStunHandler(udpHandler);
	//��ʼ��ʱ��
	pobjTimer = new CTimer();
	pobjTimer->setIntervalTime(25);
	pobjTimer->setTimerProc(timeProFun);
	pobjTimer->initTimer();

	m_nCurTime = pobjTimer->getCurTime();

	//��ʼ���߳�
	pobjPThreadProcess = new CPThreadProcess();
	pobjPThreadProcess->setThreadFun(run,NULL);
	pobjPThreadProcess->createThread();

}
void NatCrossHelper::uninit()
{
	m_SessionFdMap.clear();
//	(m_pobjSessionManager->getUAC1Info()).clearP2PSessionId();

	m_objUUIDTimeMap.clear();

	m_udpConnStatusMap.clear();

	std::map<int,char*>::iterator itr= m_udpConnectedChannelMap.begin();
	while (itr != m_udpConnectedChannelMap.end())
	{
		m_pobjIOHandler->clearIOFd(itr->first);
		m_udpConnectedChannelMap.erase(itr);
		itr++;
	}

}

int NatCrossHelper::initNet()
{
//��ʼ������Ϣ
	if (NULL == m_pobjNatDetectionHelper)
	{
		m_pobjNatDetectionHelper = new CNatDetectionHelper();
	}
	
	m_UACInfo = m_pobjSessionManager->getUAC1Info();
	std::vector<char*> objSessionIdVec = m_UACInfo.getP2PSessionId();
	for (int i=0;i<objSessionIdVec.size();i++)
	{
		m_SessionFdMap.insert(std::make_pair(-(i+1),objSessionIdVec[i]));
	}

	if (m_SessionFdMap.size()>0)
	{
		return 0;
	}
	else
		return -1;
}

void NatCrossHelper::updateUDPConnStatus(char* szUUID,int nStatus)
{
	std::map<char*,int>::iterator itr = m_udpConnStatusMap.find(szUUID);
	if(itr != m_udpConnStatusMap.end())
	{
		itr->second = nStatus;
	}
	else
		m_udpConnStatusMap.insert(std::make_pair(szUUID,-2));//-2��ʾ״̬δ֪
}

void NatCrossHelper::assembleHearBeatResponse(char* szSrcSessionId,char* szDstUUID,char* buf,int nType)
{
	//��װ�豸��ϢXML����
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("Heartbeat",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	if(nType == 2)
	{
		templateList.insert(std::pair<char*,paramType>("Status",paramType(-1)));
		templateList.insert(std::pair<char*,paramType>("statusCode",paramType(9)));
		templateList.insert(std::pair<char*,paramType>("statusStr",paramType(9)));
	}
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//��дxml�ڵ�����
	objBaseEntity.setParamValue("sessionId",szSrcSessionId);//obj.getParamValue("sessionId"));//�����ߵ�p2p��sessionId
	objBaseEntity.setParamValue("uuid",szDstUUID);//obj.getParamValue("uuid"));//Ŀ��˵�uuid
	if(nType == 2)
	{
		objBaseEntity.setParamValue("statusCode","0");
		objBaseEntity.setParamValue("statusStr","ok");
	}
	CCommunicationMsgOperater objCommunicationMsgOperater;
	objCommunicationMsgOperater.assemble_ProtocolInfo(objBaseEntity,buf);
}

int NatCrossHelper::findFdFromUDPChannel(char* szRecvUUID)
{
	std::map<int,char*>::iterator itr = m_udpConnectedChannelMap.begin();
    while(itr != m_udpConnectedChannelMap.end())
	{
      if (!strcmp(itr->second,szRecvUUID))
      {
		  return itr->first;
      }
	  itr++;
	}
	return -1;
}

void NatCrossHelper::udpHandler(CBaseEntityClass<char*> obj,char* szMsgType,int currentFd)
{
	char* szSessionId = obj.getParamValue("sessionId");//�����ߵ�P2PSessionId
	char* szRecvUUID = obj.getParamValue("uuid");//�����ߵ�uuid
	CUACInfo objSrcUAC = m_NatCrossHelper->m_pobjSessionManager->getUAC1Info();
	char* szSrcUUID = objSrcUAC.getSrcUUID();

	//���Ƿ������豸�Ķ����գ����ǵĶ�����
	if (strcmp(szRecvUUID,szSrcUUID) != 0)
	{
		return;
	}

	if (!strcmp(szMsgType,"Heartbeat"))
	{
		//���յ���Ϣ�͸�������ʱ��
		long nCurrentTime = m_NatCrossHelper->pobjTimer->getCurTime();
		m_NatCrossHelper->updateUUIDTimeMap(szRecvUUID,nCurrentTime);  

		//������������
		int nRet = (int)obj.getParamValue("statusCode");
		if(nRet != 0)
		{
            //��������
            m_NatCrossHelper->updateUDPConnStatus(szRecvUUID,-1);
			m_NatCrossHelper->m_ndetectionType = DEVICEISOFFLINE;
			return;
		}
		else
		{
			////IPC�ظ�����
			if (strcmp(objSrcUAC.getDevType(),"APP") || strcmp(objSrcUAC.getDevType(),"Requster"))
			{
				char buf[1024*8];
				memset(buf,0,1024*8);
				m_NatCrossHelper->assembleHearBeatResponse(szSessionId,szRecvUUID,buf,2);
				int fd = m_NatCrossHelper->findFdFromUDPChannel(szRecvUUID);
				if(-1 == fd)
					return;
				CUDPClientBaseCommunication* pobjUDPClientBaseCommunication =(CUDPClientBaseCommunication*)m_NatCrossHelper->m_pobjIOHandler->findUDPCommunicationInstance(fd);
				pobjUDPClientBaseCommunication->sendData(buf,strlen(buf));
			}
			//����״̬
			m_NatCrossHelper->updateUDPConnStatus(szRecvUUID,0);
			m_NatCrossHelper->m_objRouterInfo.setNatBeatHoleResult(3);//3:��ʾά������
			m_NatCrossHelper->m_ndetectionType = KEEPALIVE;	
			return;
		}
	}
	if (!strcmp(szMsgType,"UdpConnectResp"))
	{
		//֪ͨP2PSVr����P2PResponse2
		RouterInfo SrcRouterInfo = m_NatCrossHelper->GetRouterInfo(); 
		SrcRouterInfo.setNatBeatHoleResult(1);
		m_NatCrossHelper->SetRouterInfo(SrcRouterInfo);
		m_NatCrossHelper->Notify2();
		//���ӳɹ���map��������Բ���������
        m_NatCrossHelper->cleanTestFd(currentFd);

		m_NatCrossHelper->updateUDPConnStatus(szRecvUUID,1);
		m_NatCrossHelper->m_ndetectionType = UDPCONNECT;
	}
	if (!strcmp(szMsgType,"UdpConnectReq"))
	{
        //��ʼUdp���ӻظ�
		//��װUdpConnectRespЭ��
		char* buf = NULL;
        m_NatCrossHelper->assembleUdpConnectResponse(buf,szRecvUUID,szSessionId);
		//udp ����
        RouterInfo *objDstRouterInfo = m_NatCrossHelper->m_pobjSessionManager->getUAC2RouterInfoFromVec(szRecvUUID);
		CUDPClientBaseCommunication* pobjUDPClientBaseCommunication = (CUDPClientBaseCommunication *)m_NatCrossHelper->m_pobjIOHandler->findUDPCommunicationInstance(currentFd);
		pobjUDPClientBaseCommunication->init(objDstRouterInfo->getPubIPAddr(),objDstRouterInfo->getPubPortAddr());
		if (strlen(buf)>0)
		{
			pobjUDPClientBaseCommunication->sendData(buf,strlen(buf));
		}
		
		int fd = pobjUDPClientBaseCommunication->getSocketFd();
		m_NatCrossHelper->m_pobjIOHandler->addUdpFd(fd);
		m_NatCrossHelper->m_pobjIOHandler->setUdpClientCommunication(fd,(CIClientBaseCommunication *)pobjUDPClientBaseCommunication);	
		m_NatCrossHelper->updateUDPConnStatus(szRecvUUID,-2);
	}
	
}

void NatCrossHelper::getNatType()
{
	int stepPort = 0;
	AddrInfo srvAddr,localAddr,remoteAddr;
	strcpy(srvAddr.ip,m_szStunSvrIP);
	srvAddr.port = m_nStunSvrPort;
	memset(&localAddr, 0x00, sizeof(AddrInfo));
	memset(&remoteAddr, 0x00, sizeof(AddrInfo));

	int nNatType = m_pobjNatDetectionHelper->detectionNatType(m_szStunSvrIP,\
														     m_nStunSvrPort,\
															stepPort,&srvAddr,\
															&localAddr,&remoteAddr);
	m_RouterInfo.setLocalIPAddr(localAddr.ip);
	m_RouterInfo.setLocalPortAddr(localAddr.port);
	m_RouterInfo.setPubIPAddr(remoteAddr.ip);
	m_RouterInfo.setPubPortAddr(remoteAddr.port);
	m_RouterInfo.setPortChangStep(stepPort);
	m_RouterInfo.setNatType(nNatType);
//-100����ʼ��״̬�׶Σ�-1����ʧ�ܣ�1����ȡ��net���ͺ͵�ַ�׶Σ�
//2����ʼ�򶴽׶Σ�0���򶴳ɹ���3:ά�������׶�
	m_RouterInfo.setNatBeatHoleResult(1);
	SetRouterInfo(m_RouterInfo);

	m_UACInfo.setLocalIP(localAddr.ip);
	m_UACInfo.setLocalPort(localAddr.port);
	m_UACInfo.setRemoteIP(remoteAddr.ip);
	m_UACInfo.setRemotePort(remoteAddr.port);
	SetUACInfo(m_UACInfo);
}

bool NatCrossHelper::judgeNatValid()
{
	if (m_nbeatHoleStep == BEATHOLEFAILED)//��ʧ�ܵ���Ҫ���»�ȡnetType
	{
		return false;
	}
	long now = pobjTimer->getCurTime();
    if( (now - m_nCurTime) > 30*60)// ÿ����Сʱ���»�ȡһ��nat��Ϣ
    {
		m_nCurTime = now;
		return false;
    }
	if(m_ndetectionType == FULLMODE)//ȫģʽ��Ҫ����̽��
	{
       return false;
	}

	return true;
}
void NatCrossHelper::cleanTestFd(int currentFd)
{
	std::map<char*,int>::iterator itr = m_NatCrossHelper->m_udpConnStatusMap.begin();
	while(itr != m_NatCrossHelper->m_udpConnStatusMap.end()) 
	{
		std::map<int,char*>::iterator itr1 = m_NatCrossHelper->m_udpConnectedChannelMap.begin();
		while(itr1 != m_NatCrossHelper->m_udpConnectedChannelMap.end())
		{
			if(currentFd==itr1->first)
				break;
			if(!strcmp(itr->first,itr1->second))
			{
				//�ر��������UUIDһ�µ�socket
				m_NatCrossHelper->m_pobjIOHandler->clearUdpFd(itr1->first);
				m_NatCrossHelper->m_udpConnectedChannelMap.erase(itr1++);
			}
			else
			{
				itr1++;
				continue;
			}
		}
		char* szSessionId = NULL;
		std::map<int,char*>::iterator itr2 = m_NatCrossHelper->m_SessionFdMap.find(currentFd);
		if(itr2 != m_NatCrossHelper->m_SessionFdMap.end())
		{
			//����ӳ���ϵ
			szSessionId = itr2->second;
			m_NatCrossHelper->updateSessionFdMap(szSessionId,-1);
		}    
		itr++;
	}
}

void NatCrossHelper::cleanChooosedFd(int currentFd)
{
	std::map<int,char*>::iterator itr1 = m_udpConnectedChannelMap.find(currentFd);
	if(itr1 != m_udpConnectedChannelMap.end())
	{
        CUDPClientBaseCommunication* objUDPClientComm = (CUDPClientBaseCommunication*)m_pobjIOHandler->findUDPCommunicationInstance(currentFd);
		objUDPClientComm->closeSocket(currentFd);
		m_pobjIOHandler->clearUdpFd(currentFd);
        
		//����map��Դ
		updateUDPConnStatus(itr1->second,-1);//�豸����
		m_udpConnectedChannelMap.erase(itr1);
	}

	//����m_SessionFdMap
	char* szSessionId = NULL;
	std::map<int,char*>::iterator sessionFdMapItr = m_SessionFdMap.find(currentFd);
	if(sessionFdMapItr != m_SessionFdMap.end())
	{
		szSessionId = sessionFdMapItr->second;
		updateSessionFdMap(szSessionId,-1);
	}
	
}

void NatCrossHelper::cleanOfflineDev()   
{
	std::map<char*,int>::iterator itr = m_udpConnStatusMap.begin();
	while(itr != m_udpConnStatusMap.end()) 
	{
		//ֻ��������״̬��Fd������
		if(itr->second != -1)
		{
			itr++;
			continue;
		}
		std::map<int,char*>::iterator itr1 = m_udpConnectedChannelMap.begin();
		while(itr1 != m_udpConnectedChannelMap.end())
		{
			char* szSessionId = NULL;
			std::map<int,char*>::iterator sessionFdMapItr = m_SessionFdMap.find(itr1->first);
			if(sessionFdMapItr != m_SessionFdMap.end())
			{
				szSessionId = sessionFdMapItr->second;
				updateSessionFdMap(szSessionId,-1);
			}
			if(!strcmp(itr->first,itr1->second))
			{
				//�ر��������UUIDһ�µ�socket
				m_pobjIOHandler->clearUdpFd(itr1->first);
				m_udpConnectedChannelMap.erase(itr1++);	
			}
			else
			{
				continue;
				itr1++;
			}
		}
		itr++;
	}
}


void NatCrossHelper::udpConnect()
{
	std::map<char*,int>::iterator itr = m_udpConnStatusMap.begin();
	while(itr != m_udpConnStatusMap.end())
	{
		//��������״̬,�÷�֧������
		if(itr->second != 1)
			return;
		std::map<int,char*>::iterator channelItr =  m_udpConnectedChannelMap.begin();
		while(channelItr!=m_udpConnectedChannelMap.end() && !strcmp(channelItr->second,itr->first))
		{
			//��ʼ��װ����
            CUACInfo* pobjDstUACInfo = m_pobjSessionManager->getUAC2InfoFromVec(itr->first);
		    char* szSesssionId = findSessionIdByFd(channelItr->first);
			if(NULL == szSesssionId)
		       return;
	
			char buf[512];
			memset(buf,0,512);
			assembleHearBeatResponse(szSesssionId,pobjDstUACInfo->getSrcUUID(),buf,1);
			if (NULL == buf)
			{
				return;
			}
            //��ʼ��������
           CUDPClientBaseCommunication* pobjUDPClientCommunication =(CUDPClientBaseCommunication*)m_pobjIOHandler->findUDPCommunicationInstance(channelItr->first);
           pobjUDPClientCommunication->sendData(buf,strlen(buf));
		   //���ӵ���ʱ��map��
		   long nCurrentTime = pobjTimer->getCurTime();
		   updateUUIDTimeMap(itr->first,nCurrentTime);  
		   updateUDPConnStatus(itr->first,0);
		   channelItr++;
		} 
		   itr++;
	}	  
}

void NatCrossHelper::updateUUIDTimeMap(char* szUUID,long nCurrentTime)
{
	std::map<char*,long>::iterator itr = m_objUUIDTimeMap.find(szUUID);
	if(itr != m_objUUIDTimeMap.end())
	{
		itr->second = nCurrentTime;
	}
	else
		m_objUUIDTimeMap.insert(std::make_pair(szUUID,nCurrentTime));
}

void NatCrossHelper::timeProFun()
{
	if (m_NatCrossHelper->m_objUUIDTimeMap.empty())
	{
		return;
	}
	std::map<char*,long>::iterator itr = m_NatCrossHelper->m_objUUIDTimeMap.begin();
	while(itr != m_NatCrossHelper->m_objUUIDTimeMap.end())
	{
		long nCurrentTime = m_NatCrossHelper->pobjTimer->getCurTime();
		if((nCurrentTime - itr->second) > 35)
		{
          //����35s�Ϳ�ʼ�ر�ͨ��
          std::map<int,char*>::iterator channelItr =  m_NatCrossHelper->m_udpConnectedChannelMap.begin();
		  while(channelItr != m_NatCrossHelper->m_udpConnectedChannelMap.end())
		  {
              if (!strcmp((channelItr++)->second,itr->first))
              {
				   m_NatCrossHelper->cleanChooosedFd(channelItr->first);
              }
			  channelItr++;
		  }
		}
		itr++;
	}

	//APP�˶�ʱ��������
	char buf[1024*8];
	memset(buf,0,1024*8);
	CUACInfo objSrcUACInfo = (m_NatCrossHelper->m_pobjSessionManager)->getUAC1Info();
	if (!strcmp(objSrcUACInfo.getDevType(),"Requster") || !strcmp(objSrcUACInfo.getDevType(),"APP"))
	{
		char* szSessionId = objSrcUACInfo.getP2PSessionId()[0];
		char* szRecvUUID = objSrcUACInfo.getDstUUID();
		m_NatCrossHelper->assembleHearBeatResponse(szSessionId,szRecvUUID,buf,2);
		int fd = m_NatCrossHelper->findFdFromUDPChannel(szRecvUUID);
		if(-1 == fd)
			return;
		CUDPClientBaseCommunication* pobjUDPClientBaseCommunication =(CUDPClientBaseCommunication*)m_NatCrossHelper->m_pobjIOHandler->findUDPCommunicationInstance(fd);
		pobjUDPClientBaseCommunication->sendData(buf,strlen(buf));
	}
	
}

int NatCrossHelper::run(void* lparam)
{
		//û��SessionId����ͣ����ҵ�񣬿��ܡ�
		int nSize = m_NatCrossHelper->m_SessionFdMap.size();
		if (nSize != 0)
		{ 
			bool bRet = m_NatCrossHelper->m_pobjIOHandler->udpPluse();
			if (!bRet)
			{
				return -1;
			}
			switch(m_NatCrossHelper->m_nbeatHoleStep)
			{
			case NATINFOISINVALID:
				//�ж�Nat��Ϣ�Ƿ����ı�
				{
					bool bNatValid =  m_NatCrossHelper->judgeNatValid();
					if (bNatValid)
					{
						m_NatCrossHelper->m_nbeatHoleStep = STARTBEATHOLE;
					}
					else
						m_NatCrossHelper->m_nbeatHoleStep = GETNATTYPE;
					break;
				}
			case GETNATTYPE:
				{
					m_NatCrossHelper->getNatType();
					m_NatCrossHelper->Notify2();//֪ͨP2PSvr����P2PRequest
					m_NatCrossHelper->m_nbeatHoleStep = GETREMOTEADDR;
					break;
				}
			case DEVICEISOFFLINE:
				{
					//todo �����豸���������,������Դ
					m_NatCrossHelper->cleanOfflineDev();
					m_NatCrossHelper->m_nbeatHoleStep = BEATEND; 
					 break;
				}
			case GETREMOTEADDR:
				{
					m_NatCrossHelper->Notify1();
					break;
				}
			case STARTBEATHOLE:
				{
					//TODO �򶴲��裬���ݽ��ת����ͬ��֧��
					m_NatCrossHelper->beatHole();
					m_NatCrossHelper->Notify1();
					break;
				}
			case BEATHOLEFAILED:
				{
					//��ʧ�ܵĴ���
					RouterInfo SrcRouterInfo = m_NatCrossHelper->GetRouterInfo(); 
					SrcRouterInfo.setNatBeatHoleResult(-1);
					m_NatCrossHelper->SetRouterInfo(SrcRouterInfo);
					m_NatCrossHelper->Notify2();//֪ͨP2PSvr������p2PResponse2
					//���¿�ʼ������
					m_NatCrossHelper->m_nbeatHoleStep = GETNATTYPE;
					break;
				}
			case UDPCONNECT:
				{
					//����udp����
					m_NatCrossHelper->udpConnect();
					m_NatCrossHelper->m_nbeatHoleStep = HEARTBEAT;
					break;
				}
			case HEARTBEAT:
			case KEEPALIVE:
				{
					//ÿ30s����һ�������������쳣��ֹ����
					m_NatCrossHelper->m_nbeatHoleStep = KEEPALIVE;
					break;
				}
			 
			}
			(m_NatCrossHelper->pobjTimer)->run();
		}

  return 0;
}

char* NatCrossHelper::findSessionIdByFd(int fd)
{
	std::map<int,char*>::iterator Itr = m_SessionFdMap.find(fd);
    if(Itr != m_SessionFdMap.end())
	{
		return Itr->second;
	}
	return NULL;
}

int NatCrossHelper::findFdBySessionId(char* szSessionId)
{
	std::map<int,char*>::iterator Itr = m_SessionFdMap.begin();
	while (Itr != m_SessionFdMap.end())
	{
		if (!strcmp(Itr->second,szSessionId))
		{
			return Itr->first;
		}
		Itr++;
	}
	return -1;
}

void NatCrossHelper::updateSessionFdMap(char* szSessionId,int fd)
{
	if(fd<=0)
	{
		std::map<int,char*>::iterator itr = m_SessionFdMap.find(fd);
		if(itr == m_SessionFdMap.end())
		{
			m_SessionFdMap.insert(std::make_pair(fd,szSessionId));
		}
		else
		{
			fd-=1;
			updateSessionFdMap(szSessionId,fd);
		}
	}
	else
		m_SessionFdMap.insert(std::make_pair(fd,szSessionId));
}

void NatCrossHelper::updateDstUUID_FdMap(char* szUUID,int fd)
{
	m_udpConnectedChannelMap.insert(std::make_pair(fd,szUUID));
}

void NatCrossHelper::sendUdpConnectMsgToOtherPeer(RouterInfo objDstRouterInfo,CUACInfo objSrcUAC)
{
	//Ŀǰֻ����һ��sessionId�����
	CUDPClientBaseCommunication objUDPClientBaseCommunication;
	objUDPClientBaseCommunication.init(objDstRouterInfo.getPubIPAddr(),objDstRouterInfo.getPubPortAddr());
	int fd = objUDPClientBaseCommunication.getSocketFd();
	if (fd<0)
		return;
	m_pobjIOHandler->setUdpClientCommunication(fd,(CIClientBaseCommunication *)&objUDPClientBaseCommunication);
	//����fd��select
	m_pobjIOHandler->addUdpFd(fd);
	//��װUDPConnnectRequest����
	char* buf = NULL;
	assembleUdpConnectRequest(buf,objSrcUAC);
	if (NULL == buf)
	{
		return;
	}
	objUDPClientBaseCommunication.sendData(buf,strlen(buf));
	//Ŀǰֻ�򶴵�һ�������ں�����չ
	updateSessionFdMap(objSrcUAC.getP2PSessionId()[0],fd);
    updateDstUUID_FdMap(objSrcUAC.getDstUUID(),fd);

	if(!strcmp(objSrcUAC.getDevType(),"Requester"))
	{
		//֪ͨP2PSvr����P2PRequest2
        SetMsgType("P2PRequest2");
		SetUACInfo(objSrcUAC);
		Notify1();
	}	
}

void NatCrossHelper::beatHole()
{
	RouterInfo objSrcRouterInfo = m_pobjSessionManager->getUAC1RouterInfo();
	std::vector<RouterInfo> objDstRouterInfoVec = m_pobjSessionManager->getUAC2RouterInfoVec();
	CUACInfo   objSrcUAC = m_pobjSessionManager->getUAC1Info();
	std::vector<CUACInfo> objDstUACVec = m_pobjSessionManager->getUAC2InfoVec();
	
	if(0 == objDstRouterInfoVec.size())
	{
		return;
	}
	for(int i=0;i<objDstRouterInfoVec.size();i++)
	{
		if(strcmp(objDstUACVec[i].getDevType(),"Requester")==0 )
		{
		  ReceiverBeatHole(objSrcRouterInfo,objDstRouterInfoVec[i],objSrcUAC,objDstUACVec[i]);//IPC�򶴲���
		}
		else 
		{
		  RequesterBeatHole(objSrcRouterInfo,objDstRouterInfoVec[i],objSrcUAC,objDstUACVec[i]);//APP�򶴲���
		}
	}
}

void NatCrossHelper::RequesterBeatHole(RouterInfo objSrcRouterInfo,\
									   RouterInfo objDstRouterInfo,\
									   CUACInfo   objSrcUAC,\
									   CUACInfo   objDstUAC)
{
	//APP��IPC��,APP���ȴ�
	//UAC1��UAC2��ͬһ����������
	if(strcmp(objSrcRouterInfo.getPubIPAddr(),objDstRouterInfo.getPubIPAddr()) == 0 )
	{
		sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC);
	}	
	//IPC(UAC2)��Nat������FC��RC,APP(UAC1)��Nat���Ͳ�����ʲô��ִ��[APP->IPC]
	if ( objDstRouterInfo.getNatType() == NATTYPE_RESTRICTED_CONE \
		|| objDstRouterInfo.getNatType() ==  NATTYPE_FULL_CONE )
	{
		sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC);
	}
	PCorSN_SNof0Port(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC);	
	PC_SNBeatHole(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC);	
	SN_SNBeatHole(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC);
	SN_PCBeatHole(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC);
	NoSN_NoSNBeatHole(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC); 
}

void NatCrossHelper::ReceiverBeatHole(RouterInfo objSrcRouterInfo,\
									 RouterInfo objDstRouterInfo,\
									 CUACInfo   objSrcUAC,\
									 CUACInfo   objDstUAC)
{
		//UAC1��UAC2��ͬһ����������[IPC->APP]
		if(strcmp(objSrcRouterInfo.getPubIPAddr(),objDstRouterInfo.getPubIPAddr()) == 0)
		{
			sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC);
		}	
		//���APP�����Nat������FC��RC�ͣ�IPC���ն���PC��SN�ͣ�IPC���ն˾�ֱ������APP�����
		if( (objDstRouterInfo.getNatType() == NATTYPE_RESTRICTED_CONE \
			|| objDstRouterInfo.getNatType() ==  NATTYPE_FULL_CONE)\
			&& ( objSrcRouterInfo.getNatType() == NATTYPE_PORT_RESTRICTED_CONE \
			|| objSrcRouterInfo.getNatType() ==  NATTYPE_SYMMETRIC ) )
		{
			sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC); //��IPC->APP��
		}
		//�������˲���Ϊ0��SN�����ն���PC����SN
		PCorSN_SNof0Port(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC);
		//IPCΪPC��,�ֱ�������100+appRemotePort��10*2*wightPort+appRemotePor������������
		PC_SNBeatHole(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC);
		//APP��IPC����SN��
		SN_SNBeatHole(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC);
		//APP��PC��,IPC��SN��
		SN_PCBeatHole(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC);
        //APP��IPC������SN��
		NoSN_NoSNBeatHole(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC); 

}

void NatCrossHelper::NoSN_NoSNBeatHole(RouterInfo objSrcRouterInfo,\
									   RouterInfo objDstRouterInfo,\
									   CUACInfo   objSrcUAC,\
									   CUACInfo   objDstUAC)
{
	if(objSrcRouterInfo.getNatType() != NATTYPE_SYMMETRIC && objDstRouterInfo.getNatType() != NATTYPE_SYMMETRIC)
	{
		for (int i=0;i<4;i++)
		{
			sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC);
		}
	}
}

void NatCrossHelper::PCorSN_SNof0Port(RouterInfo objSrcRouterInfo,\
									  RouterInfo objDstRouterInfo,\
									  CUACInfo   objSrcUAC,\
									  CUACInfo   objDstUAC)
{
	if ((objSrcRouterInfo.getNatType() == NATTYPE_PORT_RESTRICTED_CONE \
		||objSrcRouterInfo.getNatType() == NATTYPE_SYMMETRIC)\
		&& objDstRouterInfo.getPortChangStep() == 0 \
		&& objDstRouterInfo.getNatType() == NATTYPE_SYMMETRIC )
	{
		int objDstUACPort = objDstRouterInfo.getPubPortAddr();
		for(int i=0; i<RAND_PORT_OFFSET+1; i++)
		{
			// Խ���½�
			if(objDstUACPort-RAND_PORT_OFFSET < 1024)
			{
				objDstRouterInfo.setPubPortAddr(1024 + i);				
			}
			// Խ������
			else if(objDstUACPort+RAND_PORT_OFFSET > 65534)
			{
				objDstRouterInfo.setPubPortAddr(65534 - i);					
			}
			// ������(RAND_PORT_OFFSET=1000)������
			else
			{
				objDstRouterInfo.setPubPortAddr(objDstUACPort-RAND_PORT_OFFSET/2+i);
			}
			sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC);//���͸�APP requster
			if(0 == i%20)
			{
				onSleep(0,20);	
			}
		}
	}
}

void NatCrossHelper::PC_SNBeatHole(RouterInfo objSrcRouterInfo,\
								   RouterInfo objDstRouterInfo,\
								   CUACInfo   objSrcUAC,\
								   CUACInfo   objDstUAC)
{//PC��һ����SN��һ�˴�
	if (objDstRouterInfo.getNatType() ==NATTYPE_PORT_RESTRICTED_CONE \
		|| objSrcRouterInfo.getNatType() == NATTYPE_SYMMETRIC)
	{
		int wightPort = objDstRouterInfo.getPortChangStep();
		int objDstPort = objDstRouterInfo.getPubPortAddr();
		for(int i=0; i<PORT_OFFSET_WIGHT+1; i++)
		{
			// Խ�紦��
			if((wightPort > 0) && (objDstPort+wightPort*PORT_OFFSET_WIGHT > MAX_PORT))
			{
				objDstRouterInfo.setPubPortAddr(MIN_PORT + i*wightPort);
			}
			else if((wightPort < 0) && (objDstPort+wightPort*PORT_OFFSET_WIGHT < MIN_PORT))
			{
				objDstRouterInfo.setPubPortAddr(MAX_PORT + i*wightPort);
			}
			else
			{
				objDstRouterInfo.setPubPortAddr(objDstPort+i*wightPort);
			}
			sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC);
		}
		onSleep(0,20);

		for(int i=0; i<PORT_OFFSET/2+1; i++)
		{
			objDstRouterInfo.setPubPortAddr(objDstPort-PORT_OFFSET/4+i);
			sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC);			
			onSleep(0,20);
		}
	}
}

void NatCrossHelper::SN_PCBeatHole(RouterInfo objSrcRouterInfo,\
								   RouterInfo objDstRouterInfo,\
								   CUACInfo   objSrcUAC,\
								   CUACInfo   objDstUAC)
{//SN��һ����PC��һ�˴�
	if(objDstRouterInfo.getNatType() == NATTYPE_PORT_RESTRICTED_CONE \
		&& objSrcRouterInfo.getNatType() == NATTYPE_SYMMETRIC)
	{
		int gWightPort = objDstRouterInfo.getPortChangStep();
		int objDstPort = objDstRouterInfo.getPubPortAddr();
		int createPort = 0;
		if(0 == gWightPort)
		{
			createPort = PORT_OFFSET/2; //IPCΪ����Ϊ0��SN
		}
		else
		{
			createPort = PORT_OFFSET_WIGHT; //IPCΪ���������SN
		}
		for(int i=0; i<createPort; i++)
		{
			objDstRouterInfo.setPubPortAddr(createPort);
			sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC);
			if(0== i%20)
			{
				onSleep(0,20);
			}
		}
	}
}

void NatCrossHelper::SN_SNBeatHole(RouterInfo objSrcRouterInfo,\
								   RouterInfo objDstRouterInfo,\
								   CUACInfo   objSrcUAC,\
								   CUACInfo   objDstUAC)
{//SN-SN,���˶���SN�Ĵ򶴲���
	if (objDstRouterInfo.getNatType() == NATTYPE_SYMMETRIC \
		&& objSrcRouterInfo.getNatType() == NATTYPE_SYMMETRIC)
	{
		int wightPort = objDstRouterInfo.getPortChangStep();
		int objDstPort = objDstRouterInfo.getPubPortAddr();
		for(int i=0; i<PORT_OFFSET_STEP+1; i++)
		{
			if((wightPort > 0) && (objDstPort+2*wightPort*PORT_OFFSET_STEP > MAX_PORT))
			{
				if(objDstPort+2*i*wightPort < MAX_PORT)
				{
					objDstRouterInfo.setPubPortAddr(objDstPort+2*i*wightPort);
				}
				else
				{
					objDstRouterInfo.setPubPortAddr(MIN_PORT + 2*i*wightPort);
				}
			}
			else if((wightPort < 0) && (objDstPort+2*wightPort*PORT_OFFSET_STEP < MIN_PORT))
			{
				if(objDstPort+2*i*wightPort > MIN_PORT)
				{
					objDstRouterInfo.setPubPortAddr(objDstPort+2*i*wightPort);
				}
				else
				{
					objDstRouterInfo.setPubPortAddr(MAX_PORT + 2*i*wightPort);
				}
			}
			else
			{
				objDstRouterInfo.setPubPortAddr(objDstPort + 2*i*wightPort);
			}
			sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC);
		}
		onSleep(0,20);
	}
}

void NatCrossHelper::onSleep(int nSecond,int nMs)
{
	struct timeval timeout;
	timeout.tv_sec =nSecond;
	timeout.tv_usec =nMs;
	CIOHandler::setTimerBySelect(timeout); 
}

void NatCrossHelper::assembleUdpConnectRequest(char* buf,CUACInfo objUACInfo)
{
	//��װ�豸��ϢXML����
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("UdpConnectReq",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//��дxml�ڵ�����
	objBaseEntity.setParamValue("sessionId",(objUACInfo.getP2PSessionId())[0]);//�����ߵ�p2p��sessionId
	objBaseEntity.setParamValue("uuid",objUACInfo.getDstUUID());//Ŀ��˵�uuid

	//��װЭ��
	CCommunicationMsgOperater objCommunicationMsgOperater;
	objCommunicationMsgOperater.assemble_ProtocolInfo(objBaseEntity,buf); 
}

void NatCrossHelper::assembleUdpConnectResponse(char* buf,char* szDstUUID,char* szSessionId)
{
	//��װ�豸��ϢXML����
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("UdpConnectResp",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//��дxml�ڵ�����
	objBaseEntity.setParamValue("sessionId",szSessionId);//��UdpConnectReq��������һ��
	objBaseEntity.setParamValue("uuid",szDstUUID);//��UdpConnectReq��������һ��

	//��װЭ��
	CCommunicationMsgOperater objCommunicationMsgOperater;
	objCommunicationMsgOperater.assemble_ProtocolInfo(objBaseEntity,buf); 
}

void NatCrossHelper::getP2PInfo()
{
	char* szMsgType = GetMsgType();
	if (NULL == szMsgType)
	return;

	if (strcmp(szMsgType,"P2pRequest") == 0)
	{
		m_nbeatHoleStep = STARTBEATHOLE;
	}

	if (strcmp(szMsgType,"P2pResponse1") == 0)
	{
		int  nRet = GetStatus();
		if (0 != nRet)
		{
           //�豸�����ߣ��жϴ�
			m_nbeatHoleStep = DEVICEISOFFLINE;
		}
	}

	if (strcmp(szMsgType,"P2pResponse2") == 0)
	{
		int  nRet = GetStatus();
		if (0 != nRet)
		{
			m_nbeatHoleStep = BEATHOLEFAILED;
		}
		else
			m_nbeatHoleStep = UDPCONNECT;
	}
}

int  NatCrossHelper::findConnectStatusByUUID(char* szDstUUID)
{
	if (m_udpConnStatusMap.empty())
	{
		return -1;
	}

	std::map<char*,int>::iterator itr = m_udpConnStatusMap.find(szDstUUID);
	if (itr == m_udpConnStatusMap.end())
	{
		return -1;
	}
	return itr->second;
}