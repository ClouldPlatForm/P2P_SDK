#include <stdio.h>
#include <iostream>
#include <map>
#include "P2PSDK.h"

int DemoRecvData(char* buffer, int &len)
{
	return 1;
}

int  main()
{
	P2PDevSDK P2pTestDemo(1);
	P2pTestDemo.setDevInfo("IPC","QWERTYUIOP","192.168.141.30",1935,1);
	P2pTestDemo.setP2PSvrAddr("54.222.177.236", 8200);
	P2pTestDemo.setStunSvrAddr("54.222.177.236", 3478);

	P2pTestDemo.setTCPRecvMediaFun(DemoRecvData);
	P2pTestDemo.setUDPRecvMediaFun(DemoRecvData);

	int nRet = P2pTestDemo.start();
    P2pTestDemo.stop();
   return 0;
}