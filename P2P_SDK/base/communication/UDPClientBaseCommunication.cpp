#include "UDPClientBaseCommunication.h"
#include "NatConst.h"

using namespace natconst;

CUDPClientBaseCommunication::CUDPClientBaseCommunication()
{
	_cli_sockfd = -1;
 //_serverAddr = {0};
//	bzero(_sendBuffer,sizeof(_sendBuffer));
//	bzero(_recvBuffer,sizeof(_recvBuffer));
//	init(srcAddr,port);
}

CUDPClientBaseCommunication::~CUDPClientBaseCommunication(void)
{
	closeSocket(_cli_sockfd);
}
void CUDPClientBaseCommunication::init(char* dstAddr,int dstport)
{
	creatUdpSocket();
	setServerAddrIP(dstAddr);
	setServerAddrPort(dstport);
	setSockAddr_in();
}
char* CUDPClientBaseCommunication::getDstIP()
{
	char* dstIP = NULL;
	strcpy(dstIP,_serverAddr);
	if (NULL != dstIP)
	{
		return dstIP;
	}
	return NULL;
}

int CUDPClientBaseCommunication::getDstPort()
{
	return _serverPort;
}

bool CUDPClientBaseCommunication::creatUdpSocket()
{
#ifdef WIN32
	WSADATA wsa;
	WSAStartup(WINSOCK_VERSION,&wsa);	//初始化WS2_32.DLL
#endif
	/* 建立socket*/
	_cli_sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(_cli_sockfd<0)
    {
        fprintf(stderr,"socker Error:%s\n",strerror(errno));
        return false;
    }
	return true;
}

int CUDPClientBaseCommunication::sendData(const char* sendBuffer,const int len)
{
	if(sendBuffer == NULL)
	{
		return -1;
	}
	if(len <= 0)
	{
		printf("Error!");
		return -1;
	}
	else
	{
		return sendto(_cli_sockfd,sendBuffer,len,0,(struct sockaddr*)&_cli_addr,sizeof(struct sockaddr_in));
	}
}
int CUDPClientBaseCommunication::recvData(char* recvBuffer)
{
	if(recvBuffer == NULL)
	{
		recvBuffer = new char[1024*8];//8k
	}
	int addrlen = sizeof(struct sockaddr_in);
	int len=recvfrom(_cli_sockfd,recvBuffer,strlen(recvBuffer),0,(struct sockaddr*)&_cli_addr,&addrlen);
    if(len < 0 )
	{
		return -1;
	}
	else
		return len;

}

int CUDPClientBaseCommunication::getSocketFd()
{
	return _cli_sockfd;
}

void CUDPClientBaseCommunication::setServerAddrIP(char* srcAddrIP)
{
	memset(_serverAddr,0,15);
	memcpy(_serverAddr,srcAddrIP,15);
}

void CUDPClientBaseCommunication::setServerAddrPort(int srcAddrPort)
{
	if(srcAddrPort < 0)
	{
		printf("The udp port is error!\n");
		return;
	}
	_serverPort = srcAddrPort;
}
void CUDPClientBaseCommunication::setSockAddr_in()
{
	/* 填写sockaddr_in*/
    int addrlen=sizeof(struct sockaddr_in); 
#ifdef WIN32
	memset(&_cli_addr,0,sizeof(_cli_addr));
#else
    bzero(&_cli_addr,addrlen);
#endif
    _cli_addr.sin_family=AF_INET;
    _cli_addr.sin_addr.s_addr=inet_addr(_serverAddr);
    //_cli_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    _cli_addr.sin_port=htons(_serverPort);
}

void CUDPClientBaseCommunication::closeSocket(int connetedFd)
{
#ifndef WIN32
	shutdown(connetedFd, SHUT_RDWR);
	close(connetedFd);
#else
	closesocket(connetedFd);
	WSACleanup(); 
#endif	
}
