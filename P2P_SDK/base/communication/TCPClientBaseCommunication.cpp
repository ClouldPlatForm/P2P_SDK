#include "TCPClientBaseCommunication.h"
#include "NatConst.h"

//#ifdef WIN32
//#include <winsock2.h> 
//#pragma comment(lib,"ws2_32.lib") 
//#endif

CTCPBaseCommunication::CTCPBaseCommunication(const char* serverAddrIp, const int port,const int nLocalPort)
{
	if(nLocalPort > 0)
		_localPort = nLocalPort;
	else
		_localPort = LOCALPORT;
	init(serverAddrIp,port);
}
CTCPBaseCommunication::CTCPBaseCommunication()
{
	_localPort = LOCALPORT;
}

CTCPBaseCommunication::~CTCPBaseCommunication(void)
{
	closeSocket(_cli_sockfd);		
}

void CTCPBaseCommunication::setLocalPort(int nport)
{
	_localPort = nport;
}

int CTCPBaseCommunication::init(const char* serverAddrIp, const int port)
{
	int tcpsndbuf = 1024*64;
	int tcprecvBuf = 1024*184;
	int yes = 1;
#ifndef WIN32
	_cli_sockfd =socket(AF_INET,SOCK_STREAM,0);/*创建连接的SOCKET */
    if(_cli_sockfd<0)
    {/*创建失败 */
        fprintf(stderr,"socker Error:%s\n",strerror(errno));
        return errno;
    }
    /* 初始化客户端地址*/
    int addrlen=sizeof(struct sockaddr_in);
    bzero(&_cli_addr,addrlen);
	_cli_addr.sin_family=AF_INET;
    _cli_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    _cli_addr.sin_port=_localPort;
	 
	setsockopt(_cli_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int));
	
	if(bind(_cli_sockfd,(struct sockaddr*)&_cli_addr,addrlen)<0)
    { 
        /*绑定失败 */
        fprintf(stderr,"Bind Error:%s\n",strerror(errno));
        return errno;
    }

	setnonblocking(_cli_sockfd);
	setsockopt(_cli_sockfd, SOL_SOCKET, SO_SNDBUF, (char*)&tcpsndbuf,sizeof(int));
	setsockopt(_cli_sockfd, SOL_SOCKET, SO_RCVBUF, (char*) &tcprecvBuf,sizeof(int));

	/* 初始化服务器地址*/
    addrlen=sizeof(struct sockaddr_in);
    bzero(&_ser_addr,addrlen);
    _ser_addr.sin_family=AF_INET;
    _ser_addr.sin_addr.s_addr=inet_addr(serverAddrIp);
    _ser_addr.sin_port=htons(port);
    if(connect(_cli_sockfd,(struct sockaddr*)&_ser_addr,addrlen)!=0)/*请求连接*/
    {
        /*连接失败 */
        fprintf(stderr,"Connect Error:%s\n",strerror(errno));     
		closeSocket(_cli_sockfd);   
		return errno;
    } 


#else
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,0),&wsa);	//初始化WS2_32.DLL
	//创建套接字
	if((_cli_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
	{
		printf("套接字socket创建失败!\n");
		return -1;
	}

	int addrlen=sizeof(struct sockaddr_in);
    memset(&_cli_addr,0,addrlen);
	_cli_addr.sin_family=AF_INET;
    _cli_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    _cli_addr.sin_port=_localPort;
    
	setsockopt(_cli_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int));

	if(bind(_cli_sockfd,(struct sockaddr*)&_cli_addr,addrlen)<0)
    { 
        /*绑定失败 */
        fprintf(stderr,"Bind Error:%s\n",strerror(errno));
        closeSocket(_cli_sockfd);
		return errno;
    }

	setsockopt(_cli_sockfd, SOL_SOCKET, SO_SNDBUF, (char*)&tcpsndbuf,sizeof(int));
	setsockopt(_cli_sockfd, SOL_SOCKET, SO_RCVBUF, (char*) &tcprecvBuf,sizeof(int));

	_ser_addr.sin_family = AF_INET;
	_ser_addr.sin_port = htons(port);
	_ser_addr.sin_addr.S_un.S_addr = inet_addr(serverAddrIp);
    
	//请求连接
	printf("尝试连接中...\n");
	if(connect(_cli_sockfd, (SOCKADDR *)&_ser_addr, sizeof(_ser_addr)) != 0)
	{
		printf("连接失败!\n");
		return -1;
	}
	printf("连接成功!\n");
#endif
	return RET_SUCCESS;
}

int CTCPBaseCommunication::getSockFd()
{
	return _cli_sockfd;
}

int CTCPBaseCommunication::sendData(const char* sendBuffer,const int len)
{
	if(len<0)
	{
		printf("Error!");
		return RET_FAILED;
	}
	
	return send(_cli_sockfd,sendBuffer,len,0);/*发送数据*/ 
}

int CTCPBaseCommunication::recvData(char* recvBuff)
{
	 int len = recv(_cli_sockfd,recvBuff,MAX_MSG_SIZE,0); /* 接受数据*/
	 if(len < 0 )
	{
		return RET_FAILED;
	}
	else
		return len;
}

void CTCPBaseCommunication::closeSocket(int connetedFd)
{
#ifndef WIN32
	shutdown(connetedFd,SHUT_RDWR);
	close(connetedFd);
#else
	closesocket(connetedFd);
	WSACleanup(); 
#endif	
}
