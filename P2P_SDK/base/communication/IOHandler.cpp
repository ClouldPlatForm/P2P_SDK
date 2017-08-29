#include "IOHandler.h"
#include "../../common/protocol/CommunicationMsgOperater.h"
#include "../../common/UnifiedDispatchManager.h"
CIOHandler* CIOHandler::m_pIOHandler = NULL;
//std::map<int,char*>  CIOHandler::objMapSessionId_fd;

CIOHandler::CIOHandler(void)
{
	init();
	
	memset(recvDataBuff,0,1024*128);
	
	pobjUnifiedDispatchManager= CUnifiedDispatchManager::getInstance();
}

CIOHandler::~CIOHandler(void)
{
	//清理TCP资源
	for(int i = 0;i<_readSet.fd_count;i++)
	{
		//清理socket连接
		CIClientBaseCommunication* pobjCommunicationInstance = findCommunicationInstance(_readSet.fd_array[i]);		
		if(NULL != pobjCommunicationInstance)
		{
			pobjCommunicationInstance->closeSocket(_readSet.fd_array[i]);
		}
		//清理map
		clearSessionId_fdMap(_readSet.fd_array[i]);
		clearClientBaseCommunicationMap(_readSet.fd_array[i]);
	}
 
}
void CIOHandler::clearSessionId_fdMap(int fd)
{
	std::map<int,char*>::iterator itr = objMapSessionId_fd.begin();
	while(itr != objMapSessionId_fd.end())
	{
		if (itr->first == fd)
		{
			printf("FD_CLR  %d\n", fd);
			FD_CLR(fd, &_readSet);
			objMapSessionId_fd.erase(itr++);
		}
		else
			itr++;
	}  	
}

void CIOHandler::clearClientBaseCommunicationMap(int fd)
{
	char* szSessionId = findSessionIdByFd(fd);
	std::map<char*,CIClientBaseCommunication*>::iterator itr = objClientBaseCommunicationMap.begin();
	while(itr != objClientBaseCommunicationMap.end())
	{
		if (!strcmp(szSessionId,itr->first))
		{
			if (NULL != itr->second)
			{printf("clearClientBaseCommunicationMap\n");
				delete (itr->second);
				itr->second = NULL;
			}
			objClientBaseCommunicationMap.erase(itr++);
		}
		else
			itr++;
	}
}

void CIOHandler::clearIOFd(char* sessionId)
{
	std::map<int,char*>::iterator itr = objMapSessionId_fd.begin();
	while(itr != objMapSessionId_fd.end())
	{
		if(strcmp(itr->second,sessionId)==0)
		{
			clearIOFd(itr->first);
			break;
		}
		itr++;
	}	
}

void CIOHandler::clearIOFd(int fd)
{
	for(int i = 0;i<_readSet.fd_count;i++)
	{
		if(fd == _readSet.fd_array[i])
		{
		    CIClientBaseCommunication* objCommunicationInstance = findCommunicationInstance(fd);		
			objCommunicationInstance->closeSocket(fd);
			//清理map	
			clearClientBaseCommunicationMap(fd);
			clearSessionId_fdMap(fd);

			break;
		}
	}
}

void CIOHandler::init()
{//初始化	
	FD_ZERO(&_readSet);
	FD_ZERO(&_writeSet);
	_timeOut.tv_sec=2;
	_timeOut.tv_usec=0;
	fdReadSetSize = 0;
	fdWriteSetSize = 0;
	_readHandler = NULL;
	_writerHandler = NULL; 
	objClientBaseCommunicationMap.clear();
}

void CIOHandler::setClientCommunication(CIClientBaseCommunication* tmpClientBaseCommunication,const char* sessionId)
{
	objClientBaseCommunicationMap.insert(std::make_pair(const_cast<char*>(sessionId),tmpClientBaseCommunication));
}

CIClientBaseCommunication* CIOHandler::findCommunicationInstance(char* sessionId)
{
	std::map<char*,CIClientBaseCommunication*>::iterator itr = objClientBaseCommunicationMap.begin();
	while(itr != objClientBaseCommunicationMap.end())
	{
		if (!strcmp(itr->first,sessionId))
		{
			return itr->second;
		}
		itr++;
	}
	return NULL;

}
CIClientBaseCommunication* CIOHandler::findUDPCommunicationInstance(int fd)
{
	std::map<int,CIClientBaseCommunication*>::iterator itr = objUDPClientBaseCommunicationMap.begin();
	while(itr != objUDPClientBaseCommunicationMap.end())
	{
		if (itr->first == fd)
		{
			return itr->second;
		}
		itr++;
	}
	return NULL;
}
CIClientBaseCommunication* CIOHandler::findCommunicationInstance(int fd)
{
	//TODO 为什么map里面的值都清空了？？？？
	char* szSessionId = findSessionIdByFd(fd);
	if(szSessionId == NULL)
	{
		return NULL;
	}
	CIClientBaseCommunication* pobjCommunicationInstance = findCommunicationInstance(szSessionId);
	
	return pobjCommunicationInstance;
}

char* CIOHandler::findSessionIdByFd(int fd)
{
	std::map<int,char*>::iterator itr = objMapSessionId_fd.begin();
	while(itr != objMapSessionId_fd.end())
	{
		if (itr->first == fd)
		{
			return itr->second;
		}
		itr++;
	}
	return NULL;
}

int CIOHandler::findFdBySession(char* sessionId)
{
	std::map<int,char*>::iterator itr = objMapSessionId_fd.begin();
	while(itr != objMapSessionId_fd.end())
	{
		if(strcmp(itr->second,sessionId)==0)
		{
			return itr->first;
		}
	}
	return -1;
}

void CIOHandler::addWriteSockFd(int fd)
{
	sockFd = fd;
	FD_SET(fd, &_writeSet);
	++fdReadSetSize;
}

void CIOHandler::addReadSockFd(char* sessionId,int fd)
{
	printf("addReadSockFd  %d\n",fd);
	sockFd = fd;
	FD_SET(fd, &_readSet);
	fd_A[fdWriteSetSize++] = fd;
	objMapSessionId_fd.insert(std::make_pair(fd,sessionId));
}

bool CIOHandler::pulse()
{
	if(objMapSessionId_fd.size() == 0 )
	{
		return true;
	}

	FD_ZERO(&_readSet);
	std::map<int,char*>::iterator itr = objMapSessionId_fd.begin();
   while (itr != objMapSessionId_fd.end())
   {
       FD_SET(itr->first,&_readSet);
	   itr++;
   }

	int nFd = ((--objMapSessionId_fd.end())->first);
	int count = select(nFd+1,&_readSet,NULL,NULL,&_timeOut);
	if(count < 0)
	{
		if (errno == EINTR)
			return true;
		printf("Unable to do select: %d", errno);
		return false;
	}
	else if(0 == count)
	{
		//超时处理：需要知道哪个连接的哪个业务发生了超时
		return true;
	}
	else
	{
		for(int i=0;i<fdReadSetSize;i++)
		{
			if(fd_A[i] != INVALID_SOCKET &&  FD_ISSET(fd_A[i],&_readSet))
			{
				PTHREAD_MUTEX_LOCK(&IOHandlerLock);
				//根据不同的连接对象发送和接收数据
				CIClientBaseCommunication* _IClientBaseCommunication = findCommunicationInstance(fd_A[i]);
				if(NULL == _IClientBaseCommunication)
					return false;
				_IClientBaseCommunication->setnonblocking(fd_A[i]);
				memset(recvDataBuff,0,1024*128);
				int nRet = _IClientBaseCommunication->recvData(recvDataBuff);
				if(nRet < 0)
				{
					return false;
				}
				if (nRet == 0)
				{
					return true;
				}
				//消息处理
				//p2p消息处理realy消息处理
				if(NULL == _readHandler)
					pobjUnifiedDispatchManager->parseDispatcher(recvDataBuff,fd_A[i],1);
				else //其他处理
				    (*_readHandler)(recvDataBuff,nRet);
			}
			/*if(fd_A[i] != INVALID_SOCKET &&  FD_ISSET(fd_A[i],&_writeSet))
			{
				PTHREAD_MUTEX_LOCK(&IOHandlerLock);
				_IClientBaseCommunication->setnonblocking(fd_A[i]);
				int nRet = _IClientBaseCommunication->sendData(sendDataBuff,strlen(sendDataBuff));
				if(nRet < 0)
				{
					return false;
				}
				(*_writerHandler)(sendDataBuff,nRet);
			}*/
		}
		return true;
	}
}

void CIOHandler::setTimeout(struct timeval timeout)
{
	_timeOut.tv_sec = timeout.tv_sec;
	_timeOut.tv_usec = timeout.tv_usec;
}

void CIOHandler::addReadDataListener(Handler* readhandler)
{
	_readHandler = readhandler;
}
void CIOHandler::removeReadDataListener()
{
	_readHandler = NULL;
}

void CIOHandler::removeUdpReadDataListener()
{
	_udpReadHandler = NULL;
}
void CIOHandler::addUdpReadDataListener(Handler udpReadhandler)
{
	_udpReadHandler = udpReadhandler;
}

void CIOHandler::addWriteDataListener(Handler* writehandler)
{
	_writerHandler = writehandler;
}

void CIOHandler::setTimerBySelect(struct timeval timeout)
{
    int err;
    do{
       err=select(0,NULL,NULL,NULL,&timeout);
    }while(err<0 && errno==EINTR);
}

CIOHandler* CIOHandler::getInstance()
{
	if(NULL == m_pIOHandler)
		m_pIOHandler = new CIOHandler(); 
	return m_pIOHandler;
}

void CIOHandler::setUdpClientCommunication(int fd,CIClientBaseCommunication* tmpClientBaseCommunication)
{
	objUDPClientBaseCommunicationMap.insert(std::make_pair(fd,tmpClientBaseCommunication));
}

bool CIOHandler::udpPluse()
{
	if(objUDPClientBaseCommunicationMap.size() == 0)
	{
		return true;
	}
	int nFd = ((--objUDPClientBaseCommunicationMap.end())->first);
	int count = select(nFd+1,&udpReadSet,NULL,NULL,&_udpTimeOut);
	if(count < 0)
	{
		if (errno == EINTR)
			return true;
		printf("Unable to do select: %d", errno);
		return false;
	}
	else if(0 == count)
	{
		//超时处理：需要知道哪个连接的哪个业务发生了超时
		return true;
	}
	else
	{
		for(int i=0;i<udpReadSet.fd_count;i++)
		{
			int fd = udpReadSet.fd_array[i];
			if(fd != INVALID_SOCKET &&  FD_ISSET(fd,&udpReadSet))
			{
				PTHREAD_MUTEX_LOCK(&IOHandlerLock);
				//根据不同的连接对象发送和接收数据
				CIClientBaseCommunication* _IClientBaseCommunication = findUDPCommunicationInstance(fd);
				if(NULL == _IClientBaseCommunication)
					return false;
				_IClientBaseCommunication->setnonblocking(fd);
				int nRet = _IClientBaseCommunication->recvData(recvDataBuff);
				if(nRet <= 0)
				{
					return false;
				}
				if (NULL != _udpReadHandler)
				{
					int nSize = strlen(recvDataBuff);
					(*_udpReadHandler)(recvDataBuff,nSize);
				}
				else
					pobjUnifiedDispatchManager->parseDispatcher(recvDataBuff,fd,2);
			}
		}
	}
	return true;
}

void CIOHandler::clearUdpFd(int fd)
{
	std::map<int,CIClientBaseCommunication*>::iterator Itr = objUDPClientBaseCommunicationMap.begin();
	while (Itr != objUDPClientBaseCommunicationMap.end())
	{
		if (Itr->first == fd)
		{
			CIClientBaseCommunication* objUdpCommunicationInstance = Itr->second;
			objUdpCommunicationInstance->closeSocket(fd);
			if (NULL != Itr->second)
			{
				delete Itr->second;
				Itr->second = NULL;
			}
			objUDPClientBaseCommunicationMap.erase(Itr++);
			FD_CLR(fd, &udpReadSet);
		}
		else
		{
			Itr++;
		}
	}
}

void CIOHandler::addUdpFd(int fd)
{
	FD_SET(fd, &udpReadSet);
}

void CIOHandler::setUdpTimeout(struct timeval timeout)
{
	_udpTimeOut.tv_sec = timeout.tv_sec;
	_udpTimeOut.tv_usec = timeout.tv_usec;
}