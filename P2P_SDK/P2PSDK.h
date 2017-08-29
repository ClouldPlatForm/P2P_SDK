#ifndef _P2PSDK_H
#define _P2PSDK_H
#include "relayserver/RelayHandler.h"
#include "p2pServer/P2PHandler.h"
#include "natcross/NatCrossHelper.h"
#include "common/SessionManager.h"
#include "common/RouterInfo.h"
#include "common/UACInfo.h"
#include "base/communication/IOHandler.h"
typedef int (*LISTENERFUN)(char* buffer,int& len);

class P2PDevSDK
{
public:
	P2PDevSDK(int nMode);
	~P2PDevSDK();
//设置源设备端基本信息
  void setDevInfo(const char* szDevType,const char* szUUID,const char* szIP,const int nPort,const int nStreamInfo);
//设置服务器地址
  void setP2PSvrAddr(char* szIp,int nPort);
  void setStunSvrAddr(char* szIp,int nPort);
  void setTurnSvrAddr(char* szIp,int nPort);
//设置参数配置文件的路径
  void setConfigPath(char* szPath); 
//初始化配置
  void init();
//开始运行
  int start();
//循环运行线程
  int run();
//停止本次
  void stop();
//发送数据
  int sendMediaData(char* szDstUUID,char* buf,int nSize);
//设置media数据接收监听函数
  void setTCPRecvMediaFun(LISTENERFUN objRecvListener);
  void setUDPRecvMediaFun(LISTENERFUN objRecvListener);
private:
  //接收数据
  static int recvMediaData(char* buf,int& nSize);
  void getParamFromLocal();
  void setSrcUACInfo(CUACInfo objSrcUACInfo);
  //设置信息
  void setDevType(const char* szDevType);
  void setDevUUID(const char* szUUID);
  void setDevStreamInfo(const int nStreamInfo);
  void setDevLocalIP(const char* szIP);
  void setDevLocalPort(const int nPort);
  //清理资源
  void cleanUp();
  int sendMediaDataByP2P(char* szDstUUID,char* buf,int nSize);
  int sendMediaDataByRelay(char* szDstUUID,char* buf,int nSize);

private:
	static LISTENERFUN		m_objTCPRecvListener;
	static LISTENERFUN		m_objUDPRecvListener;
	static P2PDevSDK*       m_objP2PDevSDK;
	RelayHandler*			m_objRelayHandler;
	CSessionManager*		m_objSessionManager;
    P2PHandler*				m_objP2PHandler;
	NatCrossHelper*			m_objNatCrossHelper;
	CIOHandler*             m_pobjIOHandler;
    char*                   m_szConfigPath;
	char					m_szDevType[16];
	char                    m_szUUID[32];
	char                    m_szlocalIp[15];
	int                     m_nPort;
    int                     m_nStreamInfo;
	char                    m_szP2PSvrIP[15];
    int                     m_nP2PSvrPort;
	char                    m_szStunSvrIP[15];
	int                     m_nStunSvrPort;
    int						m_nMode;//三种模式：1只有P2P;2只有Relay;3既有P2P又有Relay
};

#endif