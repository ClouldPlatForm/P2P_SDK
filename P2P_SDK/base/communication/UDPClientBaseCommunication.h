#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include "IClientBaseCommunication.h"
//linux下的头文件
#ifdef WIN32
#include <winsock2.h>	
#else
#include<unistd.h> 
#include<netinet/in.h>
#include<errno.h> 
#include<stdarg.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif



class CUDPClientBaseCommunication:CIClientBaseCommunication
{
public:
	CUDPClientBaseCommunication(void);
	~CUDPClientBaseCommunication(void);
public:
	void init(char* dstAddr,int dstport);
	int sendData(const char* sendBuffer,const int len);
	int recvData(char* recvBuffer);
    int getSocketFd();
	char* getDstIP();
	int getDstPort();
	void closeSocket(int connetedFd);
private:
	bool creatUdpSocket();
	void setServerAddrIP(char* szIP);
	void setServerAddrPort(int nPort);
    void setSockAddr_in();

private:
	int _cli_sockfd;
	char _serverAddr[15];
	struct sockaddr_in _cli_addr;
	int _serverPort;
};

