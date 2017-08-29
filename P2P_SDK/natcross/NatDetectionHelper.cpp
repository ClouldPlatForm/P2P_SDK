#include "NatDetectionHelper.h"



#define DEFAULT_WIGHT_PORT	0

CNatDetectionHelper::CNatDetectionHelper(void)
{

}

CNatDetectionHelper::~CNatDetectionHelper(void)
{
}


NatType CNatDetectionHelper::stunNatType_ext( StunAddress4& dest,bool verbose,\
						bool* preservePort,bool* hairpin,\
						int port,StunAddress4* sAddr,\
						StunAddress4* test1Addrs,StunAddress4* test12Addrs)
{
   if ( hairpin )
   {
      *hairpin = false;
   }

   if ( port == 0 )
   {
      port = stunRandomPort();
   }
   UInt32 interfaceIp=0;
   if (sAddr)
   {
      interfaceIp = sAddr->addr;
   }
   Socket myFd1 = openPort(port,interfaceIp,verbose);
   Socket myFd2 = openPort(port+1,interfaceIp,verbose);

   if ( ( myFd1 == INVALID_SOCKET) || ( myFd2 == INVALID_SOCKET) )
   {
	   return StunTypeFailure;
   }

   bool respTestI=false;
   bool isNat=true;
  
   StunAddress4 testImappedAddr;    
   bool respTestI2=false;
   
   bool mappedIpSame = false;
   StunAddress4 testI2mappedAddr;
   StunAddress4 testI2dest=dest;
   bool respTestII=false;
   bool respTestIII=false;
   bool respTestPreservePort=false;
   bool ipChange = false;
   memset(&testImappedAddr,0,sizeof(testImappedAddr));

   StunAtrString username;
   StunAtrString password;

   username.sizeValue = 0;
   password.sizeValue = 0;


   int count=0;
   struct timeval tv;
   while ( count < 7)
   {
      fd_set fdSet;
      int fdSetSize;
      FD_ZERO(&fdSet); fdSetSize=0;
      FD_SET(myFd1,&fdSet); fdSetSize = (myFd1+1>fdSetSize) ? myFd1+1 : fdSetSize;
      FD_SET(myFd2,&fdSet); fdSetSize = (myFd2+1>fdSetSize) ? myFd2+1 : fdSetSize;
     
      get_stunReSendTime(count, &tv);

      printf("test:count = %d \n\n\n",count);
      int  err = select(fdSetSize, &fdSet, NULL, NULL, &tv);
      if ( err == SOCKET_ERROR )
      {
        return StunTypeFailure;
     }
      else if ( err == 0 )
      {
         count++;

         if ( !respTestI )
         {
            stunSendTest( myFd1, dest, username, password, 1 ,verbose );
         }

         if ( (!respTestI2) && respTestI )
         {
            // check the address to send to if valid
            if (  ( testI2dest.addr != 0 ) &&
                  ( testI2dest.port != 0 ) )
            {
               stunSendTest( myFd1, testI2dest, username, password, 10  ,verbose);
            }
         }

         if ( !respTestII )
         {
            stunSendTest( myFd2, dest, username, password, 2 ,verbose );
         }

         if ( !respTestIII )
         {
            stunSendTest( myFd2, dest, username, password, 3 ,verbose );
         }
      }
      else
      {
         for ( int i=0; i<2; i++)
         {
            Socket myFd;
            if ( i==0 )
            {
               myFd=myFd1;
            }
            else
            {
               myFd=myFd2;
            }

            if ( myFd!=INVALID_SOCKET )
            {
               if ( FD_ISSET(myFd,&fdSet) )
               {
                  char msg[STUN_MAX_MESSAGE_SIZE];
                  int msgLen = sizeof(msg);

                  StunAddress4 from;

                  getMessage( myFd,
                              msg,
                              &msgLen,
                              &from.addr,
                              &from.port,verbose );

                  StunMessage resp;
                  memset(&resp, 0, sizeof(StunMessage));

                  stunParseMessage( msg,msgLen, resp,verbose );

                  switch( resp.msgHdr.id.octet[0] )
                  {
                     case 1:
                     {
                        if ( !respTestI )
                        {                        
                           testImappedAddr.addr = resp.mappedAddress.ipv4.addr;
                           testImappedAddr.port = resp.mappedAddress.ipv4.port;
                           test1Addrs->addr = resp.mappedAddress.ipv4.addr;
                           test1Addrs->port = resp.mappedAddress.ipv4.port;


                           printf("\n\n respTestI STUN TEST: mappedAddress port =%d \n",resp.mappedAddress.ipv4.port);

                           respTestPreservePort = ( testImappedAddr.port == port );
                           if ( preservePort )
                           {
                              *preservePort = respTestPreservePort;
                           }

                           testI2dest.addr = resp.changedAddress.ipv4.addr;

                           if (sAddr)
                           {
                              sAddr->port = testImappedAddr.port;
                              sAddr->addr = testImappedAddr.addr;
                           }

                           count = 0;
                        }
                        respTestI=true;
                     }
                     break;
                     case 2:
                     {
                        respTestII=true;
                     }
                     break;
                     case 3:
                     {
                        respTestIII=true;
                     }
                     break;
                     case 10:
                     {
                        if ( !respTestI2 )
                        {
                           testI2mappedAddr.addr = resp.mappedAddress.ipv4.addr;
                           testI2mappedAddr.port = resp.mappedAddress.ipv4.port;
                           test12Addrs->addr = resp.mappedAddress.ipv4.addr;
                           test12Addrs->port = resp.mappedAddress.ipv4.port;

                           printf("\n\n respTestI2 STUN TEST: mappedAddress port=%d \n",resp.mappedAddress.ipv4.port);

                           mappedIpSame = false;
                           if ((testI2mappedAddr.addr  == testImappedAddr.addr ) &&
                                (testI2mappedAddr.port == testImappedAddr.port ))
                           {
                              mappedIpSame = true;
                           }
                           else if(testI2mappedAddr.addr != testImappedAddr.addr )
                           {
                        	   ipChange = true;
                           }
                           else
                           {
                        	   mappedIpSame = false;
                           }
                        }
                        respTestI2=true;
                     }
                     break;
                     case 11:
                     {
                        if ( hairpin )
                        {
                           *hairpin = true;
                        }                        
					 }
                     break;
                  }
               }
            }
         }
      }
   }

   // see if we can bind to this address
   Socket s = openPort( 0/*use ephemeral*/, testImappedAddr.addr, false );
   if ( s != INVALID_SOCKET )
   {
    //  closesocket(s);
     closeSocket(s);
	   isNat = false;
     
   }
   else
   {
      isNat = true;
    }
   if (myFd1 != INVALID_SOCKET)
   {
	   printf("close smyFd1=%d ", myFd1);
	 //  closesocket(myFd1);
	   closeSocket(myFd1);
   }
   if (myFd2 != INVALID_SOCKET)
	{
	   printf("close smyFd2=%d ", myFd2);
//	   closesocket(myFd2);
           closeSocket(myFd2);
	}
   if (verbose)
   {
   }

   if ( respTestI ) // not blocked
   {
      if ( isNat )
      {
         if ( mappedIpSame )
         {
            if (respTestII)
            {
               return StunTypeIndependentFilter;
            }
            else
            {
               if ( respTestIII )
               {
                  return StunTypeDependentFilter;
               }
               else
               {
                  return StunTypePortDependedFilter;
               }
            }
         }
         else // mappedIp is not same
         {
        	 if(ipChange)
        	 {
        		 return StunTypeIpChange;
        	 }
        	 else
        	 {
        		 return StunTypeDependentMapping;
        	 }
         }
      }
      else  // isNat is false
      {
         if (respTestII)
         {
            return StunTypeOpen;
         }
         else
         {
            return StunTypeFirewall;
         }
      }
   }
   else
   {
      return StunTypeBlocked;
   }
   return StunTypeUnknown;
}

NatType CNatDetectionHelper::detectionNatType(char *hostname, unsigned int port,\
						 int& stepPort, AddrInfo *srvAddr, \
						 AddrInfo *localAddr, AddrInfo *remoteAddr)
{
	// 判断参数是否为空
	int srcPort = 0,step1 = 0, step2 = 0, verbose =0;
	NatType p2pNatType;
	unsigned int ip = 0;
	struct hostent* host = NULL;
	in_addr sin_addr;
	StunAddress4 serverinfo;
	StunAddress4 retaddr1;
	StunAddress4 retaddr2;
	// ip地址转换，域名--> IP
	host = gethostbyname(hostname);
	if (host == NULL)
	{
		printf("gethostbyname error! ");
		return StunTypeFailure;
	}
	else
	{
		sin_addr = *(struct in_addr*)host->h_addr;
		ip = ntohl(sin_addr.s_addr);
	}

	// 产生0x4000~0x7fff-200 之间的随机端口号
	srcPort = stunRandomPort();

	// 探测NAT类型
	serverinfo.addr = ip;
	serverinfo.port = port;
	/*int debugMode = 1;
	if(debugMode == 5)
	{
		verbose = 1;
	}
	else
	{*/
		verbose = 0;
	//}
	p2pNatType = stunNatType_ext(serverinfo, verbose, NULL, NULL, srcPort, NULL, &retaddr1, &retaddr2);

	// 获取外网地址,计算步长
	localAddr->port = srcPort+2;
	getMapAddr(1, srvAddr, localAddr, remoteAddr);
	if(p2pNatType  == 7)
	{
		step1 = retaddr2.port - retaddr1.port;
		step2 = remoteAddr->port - retaddr2.port;
		stepPort  = GetNatWightPort(step1, step2);
	}
	return p2pNatType;
}

int CNatDetectionHelper::GetNatWightPort(int step1, int step2)
{
	int gWightPort = 0, oujilidetemp = 0, tempPort1 = 0,tempPort2 = 0;
	tempPort1 = step1;
	printf( "tempPort1  port =%d \n", tempPort1);
	tempPort2 = step2;
	printf( "tempPort2  port =%d \n", tempPort2);

	// 计算步长
	if(tempPort1 == tempPort2)
	{
		// 非SN随机,若tempPort1不等于0，则说明是有规律的SN NAT，否则为非SN NAT
		if(tempPort1 != 0)
		{
			gWightPort = tempPort1;
		}
		else
		{
			gWightPort = DEFAULT_WIGHT_PORT;
		}
	}
	else
	{
		// 判断是否能被整除，如果能被整除 则是非SN随机NAT
		if((abs(tempPort1) > abs(tempPort2)) && (abs(tempPort2) > 0))
		{
			if(abs(tempPort1)%(abs(tempPort2)) == 0)
			{
				if((tempPort1 < 0) && (tempPort2 < 0))
				{
					gWightPort = -tempPort2;
				}
				else
				{
					gWightPort = tempPort2;
				}
			}
			else
			{
				gWightPort = DEFAULT_WIGHT_PORT;
			}
		}
		else if((abs(tempPort2) > abs(tempPort1)) && (abs(tempPort1) > 0))
		{
			if(abs(tempPort2)%(abs(tempPort1)) == 0)
			{
				if((tempPort1 < 0) && (tempPort2 < 0))
				{
					gWightPort = -tempPort1;
				}
				else
				{
					gWightPort = tempPort1;
				}
			}
			else
			{
				gWightPort = DEFAULT_WIGHT_PORT;
			}
		}
		else
		{
			gWightPort = DEFAULT_WIGHT_PORT;
		}

		// 若步长为随机，再用欧几里得求最大公约数
		if(gWightPort == DEFAULT_WIGHT_PORT)
		{
			if((tempPort1 != 0)&&(tempPort2 != 0))
			{
				oujilidetemp = oujilide(abs(tempPort1), abs(tempPort2));
				printf( "oujilidetemp  port =%d \n", oujilidetemp);

				// 若最大公约数为1/2/3 ，则认为是随机对称型
				if((1 == oujilidetemp) || (2 == oujilidetemp) || (3 == oujilidetemp))
				{
					gWightPort = DEFAULT_WIGHT_PORT;
				}
				else
				{
					// 区别递增递减
					if((oujilidetemp*20 < abs(tempPort1)) && (oujilidetemp*20 < abs(tempPort2)))
					{
						gWightPort = DEFAULT_WIGHT_PORT;
					}
					else
					{
						if((tempPort1 < 0) && (tempPort2 < 0))
						{
							gWightPort = -oujilidetemp;
						}
						else
						{
							gWightPort = oujilidetemp;
						}
					}
				}
			}
		}
	}
	return gWightPort;
}



int CNatDetectionHelper::getMapAddr(int type, AddrInfo *srvAddr,\
									AddrInfo *localAddr, AddrInfo *remoteAddr)
{
        // 判断参数是否为空
    if((srvAddr == NULL) || (localAddr == NULL) || (remoteAddr == NULL))
	return -2;
        
    StunAddress4 Mapadd;
    int msgLen = 128, len = 512;
    unsigned int interfaceIp = 0, IP;
    char msg[128] = {0}, buf[512]={0};

    Socket sockfd;
    StunAddress4 from;
    StunMessage resp;
    StunAtrString username;
    StunAtrString password;
    username.sizeValue = 0;
    password.sizeValue = 0;

    // 暂时只支持UDP
    type = type;
    int verbose = 0;
    
    if(localAddr->port== 0)
    {
        localAddr->port = stunRandomPort();
    }
    // end change
    //  监听端口
    sockfd = openPort(localAddr->port , interfaceIp, verbose);
    if(sockfd == -1)
    {
        printf("openPort failed ! \n");
    }
    StunMessage req;
    memset(&req, 0, sizeof(StunMessage));
    stunBuildReqSimple(&req, username, 0, 0, verbose);
    len = stunEncodeMessage( req, buf, len, password, verbose);

    // 发送消息给stun服务器，如发送失败，则按规则递增发送,次数不能大于9
    int ret = false;
    int reSendCount = 0;
    struct timeval timeout;
    int r;
    fd_set fset;
    chartoIP(srvAddr->ip, &IP);
    while(reSendCount < RESEND_STUN_COUNT)
    {
        get_stunReSendTime(reSendCount++, &timeout);
        sendMessage(sockfd, buf, len, IP, srvAddr->port, verbose);
        FD_ZERO(&fset);
        FD_SET(sockfd, &fset);
        if((r = select(sockfd+1, &fset, NULL, NULL, &timeout)) == 0) 
        {
            continue;
        } 
        else 
        {
            if (r < 0)
            {
                if(errno == EINTR)
                {
                    continue;
                }
                return RET_FAILED;
            }
        }
        ret = getMessage(sockfd, msg, &msgLen, &from.addr,  &from.port, verbose);
        if(!ret) 
        {
#ifndef WIN32
			sleep(timeout.tv_sec);
            usleep(timeout.tv_usec *1000);  
#else
			Sleep(timeout.tv_sec);
#endif
            printf("GetMapAddr \n");
        }
        else 
        {
            // 解析响应消息
            printf("read successful  \n");
            memset(&resp, 0, sizeof(StunMessage));
            ret = stunParseMessage(msg, msgLen, resp, verbose);
            if(ret)
            {
			break;
            }
            else
            {
			continue;
            }
	 }
    }
    if(!ret) 
    {
		closeSocket(sockfd);
		return RET_FAILED;
    }

    // 获取映射ip 和port
    Mapadd.port = resp.mappedAddress.ipv4.port;
    Mapadd.addr = htonl(resp.mappedAddress.ipv4.addr);
    strcpy( remoteAddr->ip,  (char*)inet_ntoa(*(struct in_addr *)&Mapadd.addr) );
    remoteAddr->port = Mapadd.port ;

    // 获取本地ip
    unsigned int count = 0;
    GetLocalIP(localAddr, &count);

	closeSocket(sockfd);

    return RET_SUCCESS;
}

void CNatDetectionHelper::stunSendTest( Socket myFd, StunAddress4& dest,\
                          const StunAtrString& username, const StunAtrString& password,\
                          int testNum, bool verbose )
{
   bool changePort=false;
   bool changeIP=false;

   switch (testNum) 
   {
      case 1:
      case 10:
      case 11:
         break;
      case 2:
         changeIP=true;
         break;
      case 3:
         changePort=true;
         break;
      case 4:
         changeIP=true;
         break;
      case 5:
         break;
      default:
    	  break;
   }

   StunMessage req;
   memset(&req, 0, sizeof(StunMessage));

   stunBuildReqSimple( &req, username,
                       changePort , changeIP ,
                       testNum );

   char buf[STUN_MAX_MESSAGE_SIZE];
   int len = STUN_MAX_MESSAGE_SIZE;

   len = stunEncodeMessage( req, buf, len, password,verbose );

   if ( verbose )
   {//todo
   }

   sendMessage( myFd, buf, len, dest.addr, dest.port, verbose );
#ifndef WIN32
   usleep(10*1000);
#endif
}
