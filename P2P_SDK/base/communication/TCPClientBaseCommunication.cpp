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
	_cli_sockfd =socket(AF_INET,SOCK_STREAM,0);/*�������ӵ�SOCKET */
    if(_cli_sockfd<0)
    {/*����ʧ�� */
        fprintf(stderr,"socker Error:%s\n",strerror(errno));
        return errno;
    }
    /* ��ʼ���ͻ��˵�ַ*/
    int addrlen=sizeof(struct sockaddr_in);
    bzero(&_cli_addr,addrlen);
	_cli_addr.sin_family=AF_INET;
    _cli_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    _cli_addr.sin_port=_localPort;
	 
	setsockopt(_cli_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int));
	
	if(bind(_cli_sockfd,(struct sockaddr*)&_cli_addr,addrlen)<0)
    { 
        /*��ʧ�� */
        fprintf(stderr,"Bind Error:%s\n",strerror(errno));
        return errno;
    }

	setnonblocking(_cli_sockfd);
	setsockopt(_cli_sockfd, SOL_SOCKET, SO_SNDBUF, (char*)&tcpsndbuf,sizeof(int));
	setsockopt(_cli_sockfd, SOL_SOCKET, SO_RCVBUF, (char*) &tcprecvBuf,sizeof(int));

	/* ��ʼ����������ַ*/
    addrlen=sizeof(struct sockaddr_in);
    bzero(&_ser_addr,addrlen);
    _ser_addr.sin_family=AF_INET;
    _ser_addr.sin_addr.s_addr=inet_addr(serverAddrIp);
    _ser_addr.sin_port=htons(port);
    if(connect(_cli_sockfd,(struct sockaddr*)&_ser_addr,addrlen)!=0)/*��������*/
    {
        /*����ʧ�� */
        fprintf(stderr,"Connect Error:%s\n",strerror(errno));     
		closeSocket(_cli_sockfd);   
		return errno;
    } 


#else
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,0),&wsa);	//��ʼ��WS2_32.DLL
	//�����׽���
	if((_cli_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
	{
		printf("�׽���socket����ʧ��!\n");
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
        /*��ʧ�� */
        fprintf(stderr,"Bind Error:%s\n",strerror(errno));
        closeSocket(_cli_sockfd);
		return errno;
    }

	setsockopt(_cli_sockfd, SOL_SOCKET, SO_SNDBUF, (char*)&tcpsndbuf,sizeof(int));
	setsockopt(_cli_sockfd, SOL_SOCKET, SO_RCVBUF, (char*) &tcprecvBuf,sizeof(int));

	_ser_addr.sin_family = AF_INET;
	_ser_addr.sin_port = htons(port);
	_ser_addr.sin_addr.S_un.S_addr = inet_addr(serverAddrIp);
    
	//��������
	printf("����������...\n");
	if(connect(_cli_sockfd, (SOCKADDR *)&_ser_addr, sizeof(_ser_addr)) != 0)
	{
		printf("����ʧ��!\n");
		return -1;
	}
	printf("���ӳɹ�!\n");
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
	
	return send(_cli_sockfd,sendBuffer,len,0);/*��������*/ 
}

int CTCPBaseCommunication::recvData(char* recvBuff)
{
	 int len = recv(_cli_sockfd,recvBuff,MAX_MSG_SIZE,0); /* ��������*/
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
