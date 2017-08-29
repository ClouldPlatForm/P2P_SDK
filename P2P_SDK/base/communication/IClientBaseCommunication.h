#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <errno.h>

class CIClientBaseCommunication
{
public:
	CIClientBaseCommunication(void){};
	virtual ~CIClientBaseCommunication(void){};
public:
	virtual int sendData(const char* sendBuffer,const int len)=0;
	virtual int recvData(char* recvBuff)=0;
    virtual void closeSocket(int connetedFd)=0;
	inline void setnonblocking(int sock)
	{
#ifndef WIN32
		int opts;
		opts=fcntl(sock,F_GETFL);
		if(opts<0)
		{
			perror("fcntl(sock,GETFL)");
			exit(1);
		}
		opts = opts|O_NONBLOCK;
		if(fcntl(sock,F_SETFL,opts)<0)
		{
			 perror("fcntl(sock,SETFL,opts)");
			 exit(1);
		}
#endif
	};

//todo 还需要增加处理函数
	
};
