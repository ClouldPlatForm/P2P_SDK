#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../base/stund/udp.h"
#include "../base/stund/stun.h"
#include "StunUtil.h"
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
#else
#include <windows.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif

class CNatDetectionHelper
{
public:
	CNatDetectionHelper(void);
	~CNatDetectionHelper(void);
public:
	void stunSendTest( Socket myFd, StunAddress4& dest,\
                       const StunAtrString& username, \
					   const StunAtrString& password,\
                       int testNum, bool verbose );

    NatType stunNatType_ext( StunAddress4& dest,\
             bool verbose,bool* preservePort,\
             bool* hairpin,int port,StunAddress4* sAddr,\
			 StunAddress4* test1Addrs,StunAddress4* test12Addrs);

	NatType detectionNatType(char *hostname, unsigned int port,\
						 int& stepPort, AddrInfo *srvAddr, \
						 AddrInfo *localAddr, AddrInfo *remoteAddr);
	int GetNatWightPort(int step1, int step2);
	int getMapAddr(int type, AddrInfo *srvAddr,\
		           AddrInfo *localAddr, AddrInfo *remoteAddr);
};

#ifdef  __cplusplus
}
#endif
