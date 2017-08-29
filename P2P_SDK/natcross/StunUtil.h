#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
//#include "../base/stund/stun.h"
//#include "../base/stund/udp.h"
#include "../common/const/NatConst.h"

#ifndef WIN32
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#endif

typedef struct  _AddrInfo_
{
	unsigned int port;						// �˿���Ϣ
	char ip[20];						// ip
}AddrInfo;

typedef struct _NetInfo_
{
	int nettype;							// ������������TCP = 1 UDP = 0
	unsigned int port;						// �˿ں�
	AddrInfo address;						// ��ַ��Ϣ, ��AddrInfo
}NetInfo;

static int chartoIP(char *host, unsigned int *IP)
{
    // �жϲ����Ƿ�Ϊ��
    if((host == NULL) || (IP == NULL))
		return -1;
    
    *IP = ntohl( inet_addr( host ) );

    return RET_SUCCESS;
}

 static int GetLocalIP(AddrInfo *localIP, unsigned int *size)
{    
    // �жϲ����Ƿ�Ϊ��
    if(localIP == NULL)
		return RET_FAILED;
#ifndef WIN32  
    int fd;
    int ifconf_len = 0;
    unsigned int loop = 0;

    struct ifreq buf[10];
    struct ifconf ifc;	

    //  ����IPv4��UDP�׽���fd
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket failed ! \n");
        return -1;
    }

    // ��ʼ��ifconf �ṹ
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t)buf;

    //  ��ýӿ��б�
    if (ioctl(fd, SIOCGIFCONF, (char *) &ifc) == -1)
    {
        printf("SIOCGIFCONF ioct failed !\nl");
        return -1;
    }

    ifconf_len = ifc.ifc_len /sizeof(struct ifreq);
    while (ifconf_len--> 0)
    {
        // ��ýӿڱ�־
        if (!(ioctl(fd, SIOCGIFFLAGS, (char *) &buf[ifconf_len])))
        {
            //  �ӿ�״̬
            if (buf[ifconf_len].ifr_flags & IFF_UP)
            {
                printf("status Up\n");
            }
            else
            {
                continue;
            }

        }
        else
        {
            printf("SIOCGIFFLAGS ioct failed !\nl");
        }

        // ��һ����eth* ƥ���IP
        printf("inter=%s  \n",  (char *) &buf[ifconf_len]);
        if (!(ioctl(fd, SIOCGIFADDR, (char *) &buf[ifconf_len])))
        {
            int ret = strcmp("(char *) &buf[ifconf_len])", "wlan");
            if(ret < 0)
            {
                loop ++;
                strcpy(localIP->ip, (char*)inet_ntoa(((struct sockaddr_in*) (&buf[ifconf_len].ifr_addr))->sin_addr));
                printf("ip=%s \n", localIP->ip);
                break;
            }
        }
        else if (!(ioctl(fd, SIOCGIFADDR, (char *) &buf[ifconf_len])))
		{
			int ret = strcmp("(char *) &buf[ifconf_len])", "eth");
			if(ret < 0)
			{
				loop ++;
				strcpy(localIP->ip, (char*)inet_ntoa(((struct sockaddr_in*) (&buf[ifconf_len].ifr_addr))->sin_addr));
				printf("ip=%s \n", localIP->ip);
				break;
			}
		}
        else
        {
            printf("SIOCGIFADDR ioct failed !\nl");
        }
    }
	*size = loop;
	close(fd);
#else
	//�ú������ر����� IP ��ַ
	std::string Result = "";
    WSADATA wsaData;
    char Name[255];
    hostent* hostinfo;
    WORD wVersionRequested = MAKEWORD( 2, 2 );
    if(WSAStartup(wVersionRequested,&wsaData) == 0){
        if(gethostname(Name,sizeof(Name))==0){
            hostinfo = gethostbyname(Name);
            if(hostinfo != NULL)
                Result = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
        }
        WSACleanup();   
    }
	strcpy(localIP->ip, Result.c_str());
	localIP->port = 6004;
	unsigned int n = sizeof(AddrInfo);
	size = &n;
#endif
	return RET_SUCCESS;
}

static int oujilide(int a,int b)
{
	if(a<b)
	{
		int temp;
		temp=a;
		a=b;
		b=temp;
	}
	if(0==b)
	{
		return a;
	}
	return oujilide(b,a%b);
}

static int get_stunReSendTime(int count, struct timeval *timeout)
{
	int ms = 0,loop = 0;
	
	if(count < 4)
	{	
		loop = count;
	       ms = pow(2.0, loop) * 100 - 100;
	}
	else
	{
		loop = 4;
	       ms = pow(2.0, loop) * 100 - 100;
	       ms = ms + 1600 * (count - loop);
	}
	timeout->tv_sec = ms/1000;
	timeout->tv_usec = ms % 1000*1000;
       
	return 0;
}

//������ת����IP��ַ
static int domainNameToIPAddress(char *hostname, char *ip)
{
	struct hostent* p2p_host = NULL;
    struct in_addr sin_addr;
    
    p2p_host = gethostbyname(hostname);
    if (p2p_host == NULL)
    {
        return RET_FAILED;
    }
    else
    {
        memset(&sin_addr, 0, sizeof(struct in_addr));
        sin_addr = *(struct in_addr*)p2p_host->h_addr;
        strcpy(ip,  inet_ntoa(sin_addr));
    }
    return RET_SUCCESS;
}

/************************************************* 
Function:   	static int get_ReSend_Time(int count, struct timeval *timeout)
Description:    �ȴ�ʱ��
parameters:
count:     �ۼӴ���
timeout: ����ʱ�䣬timeval���ͽṹ��
Return:	        ����0�ɹ�������ʧ��
*************************************************/
static int get_ReSend_Time(int count, struct timeval *timeout)
{
	int ms;
	if( count<3 ) 
	{
		ms = 1000 + 1000*count;
	} 
	else 
	{
		ms = 30 * 1000;
	}
	timeout->tv_sec = ms/1000;
	timeout->tv_usec = ms % 1000*1000;

	return 0;
}