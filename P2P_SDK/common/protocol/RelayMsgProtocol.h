#ifndef _P2P_RELAY_MSG_H_
#define _P2P_RELAY_MSG_H_

/**
* 
* <strong>
* relay head��
* <pre>
*  0                   1                   2                   3
*  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* |             magic             |        payload length         |
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* |           type(cmd)           |           ext(code)           |
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* </pre></strong>
* <strong>
* relay payload��
* <pre>
*  0                   1                   2                   3
*  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* |                            payload...                           
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* </pre></strong>
* <strong>
* <strong>
* @Description relay head ͷ��Ϣ
* <br>1��magic      ��Э��ʶ���룬Ϊһ���̶�ֵ��������Ӧ����ͬ  0x1234��
* <br>2��length     ����Ϣ�峤�ȣ�0��ʾû����Ϣ�壻
* <br>3��type       ��ָ��ţ�
* <br>4��ext(code)  �������룻
* <br>5��payload      ��������Ϣ���ֽڣ�
* @author luobofa
* @time 2017-7-18 
*/

#ifdef _MSC_VER  
typedef __int32 int32_t; 
typedef unsigned __int32 uint32_t; 
typedef __int64 int64_t; 
typedef unsigned __int64 uint64_t;  
typedef unsigned short   uint16_t;
#else 
#include <stdint.h> 
#endif

#define RELAY_PROTOCOL_HEADLENGTH  (8); //Э��ͷ��8�ֽ�
#define MAGIC                      (0x1234);//Э��ͷħ��

typedef struct
{
	uint16_t magic; 							//Э��ʶ���룬Ϊһ���̶�ֵ��������Ӧ����ͬ��
	uint16_t len;        						//���س��ȣ�0��ʾû����Ϣ�塣
	uint16_t cmd;     							//ָ��š�
	uint16_t code; 								//�����ֽڣ�����δ���塣
}relay_protocol_header_t;

typedef struct _reqData_t_
{
	relay_protocol_header_t reqHeader;
	char pMsgBody[1024];//��Ϣ�������ͨ�����з�\n������
}reqData, *pReqData;

typedef struct _respData_t_
{
	relay_protocol_header_t respHeader;
	char*  pMsgBody;
}respData, *pRespData;

typedef struct _MsgBody_ 
{
	uint16_t type;
	uint16_t len;
	char* pContend;
}MsgBody;


//����ָ����ݶ���
#define CMD_LOGIN                      	0x02  //ע��
#define CMD_LOGIN_RSP                   0x0102  //ע����Ӧ
#define CMD_RELAY_STREAM         	    0x03 // relay��
#define CMD_RELAY_STREAM_RSP			0x0103 // relay����Ӧ
#define CMD_RELAY_CMD                   0x04 // relayָ��
#define CMD_RELAY_CMD_RSP               0x0104 // relayָ����Ӧ

//relay���������
#define CMD_RELAY_CMD_EXIT_RELAY        0X02    //�˳�relay
#define CMD_RELAY_CMD_OPEN_STREAM       0X01

/* ������ relay_protocol_header_t.code
�ݶ����£�
*/
#define ERRNO_OK                        0x00 	/* �ɹ� */
#define ERRNO_SYSTEM                  	0x01 	/* ϵͳ����    ���нӿ� */
#define ERRNO_REQ_INVALID         		0x02 	/* ����������� ���нӿ� */
#define EUNLOGINSRC                     0x03 	/* δע��Դ�ͻ��� */
#define EUNLOGINDST                     0x04 	/* δע��Ŀ�� */
#define EUNFOUNDDST                   	0x05 	/* Ŀ�겻���� */
#define EAPPFIRSTLOGIN                 	0x08 	/* APP�����ߣ�relaysvr��ֱ�ӷ�����ת��������صȴ�CMD_RELAY_CMD */

class RelayMsgHandler
{
public:
	RelayMsgHandler();
	~RelayMsgHandler();
public:
	int  findRelayMagicPoint(char* buf);
	void parserRelayMsg(char* buf,_respData_t_&  relayResponseMsg);
	void assembleRelayMsg(_reqData_t_  relayResponseMsg,char* buf);
private:
	bool isMagic(char* buf,int& nStartPoint);
};
#endif