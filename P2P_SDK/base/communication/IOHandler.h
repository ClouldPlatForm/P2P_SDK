#ifndef _IOHANDLER_H_
#define _IOHANDLER_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h> 
#include <stdarg.h>
#include <map>
#include "UDPClientBaseCommunication.h"
#include "TCPClientBaseCommunication.h"
#include "NatConst.h"
#include "IClientBaseCommunication.h"

//linux下的头文件
#ifdef WIN32
#include <winsock2.h> 
#else
#include<unistd.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#endif
#include "../PthreadMutexLock.h"

#define MAX_SELECT_NUM (1024)
typedef int (*Handler)(char* buffer,int& len);

class CUnifiedDispatchManager;

class CIOHandler
{
private:
	CIOHandler(void);
public:
	~CIOHandler(void);
	static CIOHandler* getInstance();
	//TCP select
	void addWriteSockFd(int fd);
	void addReadSockFd(char* sessionId,int fd);
	bool pulse();
	void addReadDataListener(Handler* readhandler);
	void removeReadDataListener();
	void addWriteDataListener(Handler* writehandler);
	static void setTimerBySelect(struct timeval timeout);
	void setClientCommunication(CIClientBaseCommunication* tmpClientBaseCommunication,const char* sessionId);
	void setTimeout(struct timeval timeout);
	void clearIOFd(int fd);
	void clearIOFd(char* sessionId);
	//sendDatabuffer没用上去
	inline void setSendbuffer(char* sendbuf)
	{
		sendDataBuff = sendbuf;
	};
	CIClientBaseCommunication* findCommunicationInstance(char* sessionId);
	CIClientBaseCommunication* findCommunicationInstance(int fd);
	char* findSessionIdByFd(int fd);
    int findFdBySession(char* sessionId);
	//UDP select
	void addUdpReadDataListener(Handler udpReadhandler);
	void removeUdpReadDataListener();
	CIClientBaseCommunication* findUDPCommunicationInstance(int fd);
    void addUdpFd(int fd);
	void clearUdpFd(int fd);
    void setUdpClientCommunication(int fd,CIClientBaseCommunication* tmpClientBaseCommunication);
	bool udpPluse();
	void setUdpTimeout(struct timeval timeout);
private:
	void init();
	void clearSessionId_fdMap(int fd);
	void clearClientBaseCommunicationMap(int fd);
private:
	//UDP
	fd_set udpReadSet;
	std::map<int,CIClientBaseCommunication*>	objUDPClientBaseCommunicationMap;
	struct timeval _udpTimeOut;
	Handler _udpReadHandler;
	//TCP
	fd_set _readSet;
	fd_set _writeSet;
	struct timeval _timeOut;
	int fdReadSetSize;
	int fdWriteSetSize;
	Handler* _readHandler;
	Handler* _writerHandler;
	int sockFd;
	int fd_A[1024];
	char* sendDataBuff;
	char  recvDataBuff[1024*128];
	CThreadLock IOHandlerLock;
	static CIOHandler* m_pIOHandler;
	CUnifiedDispatchManager*  pobjUnifiedDispatchManager;
	std::map<char*,CIClientBaseCommunication*>   objClientBaseCommunicationMap; //存储不同会话的链接对象
	std::map<int,char*>  objMapSessionId_fd;
	
};

#endif
