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

	//此处增加处理回调函数
	pobjUnifiedDispatchManager = CUnifiedDispatchManager::getInstance();
	pobjUnifiedDispatchManager->setStunHandler(udpHandler);
	//初始化时钟
	pobjTimer = new CTimer();
	pobjTimer->setIntervalTime(25);
	pobjTimer->setTimerProc(timeProFun);
	pobjTimer->initTimer();

	m_nCurTime = pobjTimer->getCurTime();

	//初始化线程
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
//初始基本信息
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
		m_udpConnStatusMap.insert(std::make_pair(szUUID,-2));//-2表示状态未知
}

void NatCrossHelper::assembleHearBeatResponse(char* szSrcSessionId,char* szDstUUID,char* buf,int nType)
{
	//组装设备信息XML对象
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
	//填写xml节点内容
	objBaseEntity.setParamValue("sessionId",szSrcSessionId);//obj.getParamValue("sessionId"));//请求者的p2p的sessionId
	objBaseEntity.setParamValue("uuid",szDstUUID);//obj.getParamValue("uuid"));//目标端的uuid
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
	char* szSessionId = obj.getParamValue("sessionId");//发送者的P2PSessionId
	char* szRecvUUID = obj.getParamValue("uuid");//接收者的uuid
	CUACInfo objSrcUAC = m_NatCrossHelper->m_pobjSessionManager->getUAC1Info();
	char* szSrcUUID = objSrcUAC.getSrcUUID();

	//凡是发给本设备的都接收，不是的都跳出
	if (strcmp(szRecvUUID,szSrcUUID) != 0)
	{
		return;
	}

	if (!strcmp(szMsgType,"Heartbeat"))
	{
		//接收到消息就更新心跳时间
		long nCurrentTime = m_NatCrossHelper->pobjTimer->getCurTime();
		m_NatCrossHelper->updateUUIDTimeMap(szRecvUUID,nCurrentTime);  

		//增加心跳处理
		int nRet = (int)obj.getParamValue("statusCode");
		if(nRet != 0)
		{
            //处理离线
            m_NatCrossHelper->updateUDPConnStatus(szRecvUUID,-1);
			m_NatCrossHelper->m_ndetectionType = DEVICEISOFFLINE;
			return;
		}
		else
		{
			////IPC回复心跳
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
			//设置状态
			m_NatCrossHelper->updateUDPConnStatus(szRecvUUID,0);
			m_NatCrossHelper->m_objRouterInfo.setNatBeatHoleResult(3);//3:表示维持心跳
			m_NatCrossHelper->m_ndetectionType = KEEPALIVE;	
			return;
		}
	}
	if (!strcmp(szMsgType,"UdpConnectResp"))
	{
		//通知P2PSVr发送P2PResponse2
		RouterInfo SrcRouterInfo = m_NatCrossHelper->GetRouterInfo(); 
		SrcRouterInfo.setNatBeatHoleResult(1);
		m_NatCrossHelper->SetRouterInfo(SrcRouterInfo);
		m_NatCrossHelper->Notify2();
		//增加成功的map，清理测试产生的连接
        m_NatCrossHelper->cleanTestFd(currentFd);

		m_NatCrossHelper->updateUDPConnStatus(szRecvUUID,1);
		m_NatCrossHelper->m_ndetectionType = UDPCONNECT;
	}
	if (!strcmp(szMsgType,"UdpConnectReq"))
	{
        //开始Udp连接回复
		//组装UdpConnectResp协议
		char* buf = NULL;
        m_NatCrossHelper->assembleUdpConnectResponse(buf,szRecvUUID,szSessionId);
		//udp 发送
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
//-100：初始化状态阶段；-1：打洞失败；1：获取到net类型和地址阶段；
//2：开始打洞阶段；0：打洞成功；3:维持心跳阶段
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
	if (m_nbeatHoleStep == BEATHOLEFAILED)//打洞失败的需要重新获取netType
	{
		return false;
	}
	long now = pobjTimer->getCurTime();
    if( (now - m_nCurTime) > 30*60)// 每隔半小时重新获取一下nat信息
    {
		m_nCurTime = now;
		return false;
    }
	if(m_ndetectionType == FULLMODE)//全模式需要重新探测
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
				//关闭所有与该UUID一致的socket
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
			//更新映射关系
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
        
		//清理map资源
		updateUDPConnStatus(itr1->second,-1);//设备离线
		m_udpConnectedChannelMap.erase(itr1);
	}

	//更新m_SessionFdMap
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
		//只处理离线状态下Fd的清理
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
				//关闭所有与该UUID一致的socket
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
		//不是上线状态,该分支不处理
		if(itr->second != 1)
			return;
		std::map<int,char*>::iterator channelItr =  m_udpConnectedChannelMap.begin();
		while(channelItr!=m_udpConnectedChannelMap.end() && !strcmp(channelItr->second,itr->first))
		{
			//开始组装心跳
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
            //开始发送心跳
           CUDPClientBaseCommunication* pobjUDPClientCommunication =(CUDPClientBaseCommunication*)m_pobjIOHandler->findUDPCommunicationInstance(channelItr->first);
           pobjUDPClientCommunication->sendData(buf,strlen(buf));
		   //增加到定时器map中
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
          //超过35s就开始关闭通道
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

	//APP端定时发送心跳
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
		//没有SessionId就暂停处理业务，空跑。
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
				//判断Nat信息是否发生改变
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
					m_NatCrossHelper->Notify2();//通知P2PSvr发送P2PRequest
					m_NatCrossHelper->m_nbeatHoleStep = GETREMOTEADDR;
					break;
				}
			case DEVICEISOFFLINE:
				{
					//todo 处理设备不在线情况,清理资源
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
					//TODO 打洞步骤，根据结果转到不同分支。
					m_NatCrossHelper->beatHole();
					m_NatCrossHelper->Notify1();
					break;
				}
			case BEATHOLEFAILED:
				{
					//打洞失败的处理
					RouterInfo SrcRouterInfo = m_NatCrossHelper->GetRouterInfo(); 
					SrcRouterInfo.setNatBeatHoleResult(-1);
					m_NatCrossHelper->SetRouterInfo(SrcRouterInfo);
					m_NatCrossHelper->Notify2();//通知P2PSvr，发送p2PResponse2
					//重新开始打洞流程
					m_NatCrossHelper->m_nbeatHoleStep = GETNATTYPE;
					break;
				}
			case UDPCONNECT:
				{
					//建立udp连接
					m_NatCrossHelper->udpConnect();
					m_NatCrossHelper->m_nbeatHoleStep = HEARTBEAT;
					break;
				}
			case HEARTBEAT:
			case KEEPALIVE:
				{
					//每30s发送一次心跳，心跳异常终止连接
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
	//目前只考虑一个sessionId的情况
	CUDPClientBaseCommunication objUDPClientBaseCommunication;
	objUDPClientBaseCommunication.init(objDstRouterInfo.getPubIPAddr(),objDstRouterInfo.getPubPortAddr());
	int fd = objUDPClientBaseCommunication.getSocketFd();
	if (fd<0)
		return;
	m_pobjIOHandler->setUdpClientCommunication(fd,(CIClientBaseCommunication *)&objUDPClientBaseCommunication);
	//增加fd到select
	m_pobjIOHandler->addUdpFd(fd);
	//组装UDPConnnectRequest报文
	char* buf = NULL;
	assembleUdpConnectRequest(buf,objSrcUAC);
	if (NULL == buf)
	{
		return;
	}
	objUDPClientBaseCommunication.sendData(buf,strlen(buf));
	//目前只打洞第一个，便于后续扩展
	updateSessionFdMap(objSrcUAC.getP2PSessionId()[0],fd);
    updateDstUUID_FdMap(objSrcUAC.getDstUUID(),fd);

	if(!strcmp(objSrcUAC.getDevType(),"Requester"))
	{
		//通知P2PSvr发送P2PRequest2
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
		  ReceiverBeatHole(objSrcRouterInfo,objDstRouterInfoVec[i],objSrcUAC,objDstUACVec[i]);//IPC打洞策略
		}
		else 
		{
		  RequesterBeatHole(objSrcRouterInfo,objDstRouterInfoVec[i],objSrcUAC,objDstUACVec[i]);//APP打洞策略
		}
	}
}

void NatCrossHelper::RequesterBeatHole(RouterInfo objSrcRouterInfo,\
									   RouterInfo objDstRouterInfo,\
									   CUACInfo   objSrcUAC,\
									   CUACInfo   objDstUAC)
{
	//APP向IPC打洞,APP优先打洞
	//UAC1和UAC2在同一个局域网内
	if(strcmp(objSrcRouterInfo.getPubIPAddr(),objDstRouterInfo.getPubIPAddr()) == 0 )
	{
		sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC);
	}	
	//IPC(UAC2)的Nat类型是FC或RC,APP(UAC1)的Nat类型不论是什么都执行[APP->IPC]
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
		//UAC1和UAC2在同一个局域网内[IPC->APP]
		if(strcmp(objSrcRouterInfo.getPubIPAddr(),objDstRouterInfo.getPubIPAddr()) == 0)
		{
			sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC);
		}	
		//如果APP请求端Nat类型是FC或RC型，IPC接收端是PC或SN型，IPC接收端就直接连接APP请求端
		if( (objDstRouterInfo.getNatType() == NATTYPE_RESTRICTED_CONE \
			|| objDstRouterInfo.getNatType() ==  NATTYPE_FULL_CONE)\
			&& ( objSrcRouterInfo.getNatType() == NATTYPE_PORT_RESTRICTED_CONE \
			|| objSrcRouterInfo.getNatType() ==  NATTYPE_SYMMETRIC ) )
		{
			sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC); //【IPC->APP】
		}
		//如果请求端步长为0的SN，接收端是PC或者SN
		PCorSN_SNof0Port(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC);
		//IPC为PC型,分别向正负100+appRemotePort和10*2*wightPort+appRemotePor发送连接请求
		PC_SNBeatHole(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC);
		//APP和IPC都是SN型
		SN_SNBeatHole(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC);
		//APP是PC型,IPC是SN型
		SN_PCBeatHole(objSrcRouterInfo,objDstRouterInfo,objSrcUAC,objDstUAC);
        //APP和IPC都不是SN型
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
			// 越过下界
			if(objDstUACPort-RAND_PORT_OFFSET < 1024)
			{
				objDstRouterInfo.setPubPortAddr(1024 + i);				
			}
			// 越过上限
			else if(objDstUACPort+RAND_PORT_OFFSET > 65534)
			{
				objDstRouterInfo.setPubPortAddr(65534 - i);					
			}
			// 在正负(RAND_PORT_OFFSET=1000)的区间
			else
			{
				objDstRouterInfo.setPubPortAddr(objDstUACPort-RAND_PORT_OFFSET/2+i);
			}
			sendUdpConnectMsgToOtherPeer(objDstRouterInfo,objSrcUAC);//发送给APP requster
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
{//PC型一端向SN的一端打洞
	if (objDstRouterInfo.getNatType() ==NATTYPE_PORT_RESTRICTED_CONE \
		|| objSrcRouterInfo.getNatType() == NATTYPE_SYMMETRIC)
	{
		int wightPort = objDstRouterInfo.getPortChangStep();
		int objDstPort = objDstRouterInfo.getPubPortAddr();
		for(int i=0; i<PORT_OFFSET_WIGHT+1; i++)
		{
			// 越界处理
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
{//SN的一端向PC型一端打洞
	if(objDstRouterInfo.getNatType() == NATTYPE_PORT_RESTRICTED_CONE \
		&& objSrcRouterInfo.getNatType() == NATTYPE_SYMMETRIC)
	{
		int gWightPort = objDstRouterInfo.getPortChangStep();
		int objDstPort = objDstRouterInfo.getPubPortAddr();
		int createPort = 0;
		if(0 == gWightPort)
		{
			createPort = PORT_OFFSET/2; //IPC为步长为0的SN
		}
		else
		{
			createPort = PORT_OFFSET_WIGHT; //IPC为随机步长的SN
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
{//SN-SN,两端都是SN的打洞策略
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
	//组装设备信息XML对象
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("UdpConnectReq",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//填写xml节点内容
	objBaseEntity.setParamValue("sessionId",(objUACInfo.getP2PSessionId())[0]);//请求者的p2p的sessionId
	objBaseEntity.setParamValue("uuid",objUACInfo.getDstUUID());//目标端的uuid

	//组装协议
	CCommunicationMsgOperater objCommunicationMsgOperater;
	objCommunicationMsgOperater.assemble_ProtocolInfo(objBaseEntity,buf); 
}

void NatCrossHelper::assembleUdpConnectResponse(char* buf,char* szDstUUID,char* szSessionId)
{
	//组装设备信息XML对象
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("UdpConnectResp",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	CBaseEntityClass<char*> objBaseEntity;
	objBaseEntity.initList(templateList);
	//填写xml节点内容
	objBaseEntity.setParamValue("sessionId",szSessionId);//与UdpConnectReq请求命令一致
	objBaseEntity.setParamValue("uuid",szDstUUID);//与UdpConnectReq请求命令一致

	//组装协议
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
           //设备不在线，中断打洞
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