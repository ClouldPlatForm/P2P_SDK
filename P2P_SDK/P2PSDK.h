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
//����Դ�豸�˻�����Ϣ
  void setDevInfo(const char* szDevType,const char* szUUID,const char* szIP,const int nPort,const int nStreamInfo);
//���÷�������ַ
  void setP2PSvrAddr(char* szIp,int nPort);
  void setStunSvrAddr(char* szIp,int nPort);
  void setTurnSvrAddr(char* szIp,int nPort);
//���ò��������ļ���·��
  void setConfigPath(char* szPath); 
//��ʼ������
  void init();
//��ʼ����
  int start();
//ѭ�������߳�
  int run();
//ֹͣ����
  void stop();
//��������
  int sendMediaData(char* szDstUUID,char* buf,int nSize);
//����media���ݽ��ռ�������
  void setTCPRecvMediaFun(LISTENERFUN objRecvListener);
  void setUDPRecvMediaFun(LISTENERFUN objRecvListener);
private:
  //��������
  static int recvMediaData(char* buf,int& nSize);
  void getParamFromLocal();
  void setSrcUACInfo(CUACInfo objSrcUACInfo);
  //������Ϣ
  void setDevType(const char* szDevType);
  void setDevUUID(const char* szUUID);
  void setDevStreamInfo(const int nStreamInfo);
  void setDevLocalIP(const char* szIP);
  void setDevLocalPort(const int nPort);
  //������Դ
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
    int						m_nMode;//����ģʽ��1ֻ��P2P;2ֻ��Relay;3����P2P����Relay
};

#endif