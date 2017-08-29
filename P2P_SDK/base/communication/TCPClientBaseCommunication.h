#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#else
#include <winsock2.h> 
#endif
#include "IClientBaseCommunication.h"
#include "../../common/const/NatConst.h"

class CTCPBaseCommunication:public CIClientBaseCommunication
{
public:
	CTCPBaseCommunication(const char* serverAddrIp, const int port,const int nLocalPort);
	CTCPBaseCommunication();
	~CTCPBaseCommunication(void);

public:
	void setLocalPort(int nport);
	int init(const char* serverAddrIp, const int port);
	int getSockFd();
	int sendData(const char* sendBuffer,const int len);
	int recvData(char* recvBuff);
    void closeSocket(int connetedFd); 
private:
	int _cli_sockfd;
	struct sockaddr_in _cli_addr;
	struct sockaddr_in _ser_addr;
	int		_localPort;
};
