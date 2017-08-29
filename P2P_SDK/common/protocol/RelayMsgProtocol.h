#ifndef _P2P_RELAY_MSG_H_
#define _P2P_RELAY_MSG_H_

/**
* 
* <strong>
* relay head：
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
* relay payload：
* <pre>
*  0                   1                   2                   3
*  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* |                            payload...                           
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* </pre></strong>
* <strong>
* <strong>
* @Description relay head 头信息
* <br>1、magic      ：协议识别码，为一个固定值，请求及响应均相同  0x1234；
* <br>2、length     ：消息体长度，0表示没有消息体；
* <br>3、type       ：指令号；
* <br>4、ext(code)  ：返回码；
* <br>5、payload      ：负载消息体字节；
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

#define RELAY_PROTOCOL_HEADLENGTH  (8); //协议头长8字节
#define MAGIC                      (0x1234);//协议头魔数

typedef struct
{
	uint16_t magic; 							//协议识别码，为一个固定值，请求及响应均相同。
	uint16_t len;        						//负载长度，0表示没有消息体。
	uint16_t cmd;     							//指令号。
	uint16_t code; 								//保留字节，功能未定义。
}relay_protocol_header_t;

typedef struct _reqData_t_
{
	relay_protocol_header_t reqHeader;
	char pMsgBody[1024];//消息体的内容通过换行符\n来区别
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


//其中指令号暂定：
#define CMD_LOGIN                      	0x02  //注册
#define CMD_LOGIN_RSP                   0x0102  //注册响应
#define CMD_RELAY_STREAM         	    0x03 // relay流
#define CMD_RELAY_STREAM_RSP			0x0103 // relay流响应
#define CMD_RELAY_CMD                   0x04 // relay指令
#define CMD_RELAY_CMD_RSP               0x0104 // relay指令响应

//relay命令负载命令
#define CMD_RELAY_CMD_EXIT_RELAY        0X02    //退出relay
#define CMD_RELAY_CMD_OPEN_STREAM       0X01

/* 返回码 relay_protocol_header_t.code
暂定如下：
*/
#define ERRNO_OK                        0x00 	/* 成功 */
#define ERRNO_SYSTEM                  	0x01 	/* 系统错误    所有接口 */
#define ERRNO_REQ_INVALID         		0x02 	/* 请求参数错误。 所有接口 */
#define EUNLOGINSRC                     0x03 	/* 未注册源客户端 */
#define EUNLOGINDST                     0x04 	/* 未注册目标 */
#define EUNFOUNDDST                   	0x05 	/* 目标不在线 */
#define EAPPFIRSTLOGIN                 	0x08 	/* APP先上线，relaysvr会直接返回中转命令，而不必等待CMD_RELAY_CMD */

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