#pragma once
#include "../../base/basexmlhelper.h"

class CCommunicationMsgOperater
{
public:
	CCommunicationMsgOperater(void);
	~CCommunicationMsgOperater(void);
public:
 /**************************************************************************
 * 函数名称：int parse_MsgResponse(const char*buf, CBaseEntityClass<char*> &obj)
 * 功能描述：生成MsgResponse xml 结构
 * 输入参数：buf - xml 数据
 * 输出参数：MsgResponse对象结构
 * 返 回 值    0 成功| 其他失败
 * 其它说明：xml 格式
 * <?xml version="1.0" encoding="UTF-8"?>
 * <MsgResponse version="1.0">
 * 	<sessionId>0</sessionId>
 *	<Status>
 *		<statusCode>0</statusCode>
 *		<statusStr>String</statusStr>
 * 	</Status>
 * </MsgResponse>
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/7	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_MsgResponse(const char*buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_MsgResponse1(const char*buf, CBaseEntityClass<char*> &obj)
 * 功能描述：生成MsgResponse1 xml 结构
 * 输入参数：buf - xml 数据
 * 输出参数：MsgResponse1对象结构
 * 返 回 值    0 成功| 其他失败
 * 其它说明：xml 格式
 * <?xml version="1.0" encoding="UTF-8"?>
 * <MsgResponse1 version="1.0">
 * 	<sessionId>0</sessionId>
 *	<relayIP>192.168.21.34</relayIP>
 *	<relayPort>7767</relayPort>
 *	<Status>
 *		<statusCode>0</statusCode>
 *		<statusStr>String</statusStr>
 * 	</Status>
 * </MsgResponse1>
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_MsgResponse1(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_UpdateDevInfo(const char *buf, CBaseEntityClass<char*> &obj)
 * 功能描述：解析 UpdateDevInfo xml  消息
 * 输入参数：buf - xml 数据
 * 输出参数：updateDevInfo对象结构
 * 返 回 值：   0 成功| 其他失败
 * 其它说明：xml 格式
 * <?xml version="1.0" encoding="UTF-8"?>
 * <UpdateDevInfo version="1.0">
 *	<sessionId>0</sessionId>
 *	<devType>APP</devType>
 *	<uuid>00000000-0000-0000-0000-000000000000</uuid>
 * </UpdateDevInfo>
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_UpdateDevInfo(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_UpdateNatType(const char *buf, CBaseEntityClass<char*> &obj)
 * 功能描述：解析 UpdateNatType xml  消息
 * 输入参数：buf - xml 数据
 * 输出参数：UpdateNatType对象结构
 * 返 回 值：   0 成功| 其他失败
 * 其它说明：xml 格式
 * <?xml version="1.0" encoding="UTF-8"?>
 * <UpdateNatType version="1.0">
 *	<sessionId>0</sessionId>
 *  <uuid>0000</uuid>
 *	<natType>APP</natType>
 * </UpdateNatType>
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_UpdateNatType(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_UdpConnectReq(const char *buf, CBaseEntityClass<char*> &obj)
 * 功能描述：解析 UdpConnectReq xml  消息
 * 输入参数：buf - xml 数据
 * 输出参数：UdpConnectReq对象结构
 * 返 回 值    0成功| 其他失败
 * 其它说明：xml 格式
 * <?xml version="1.0" encoding="UTF-8"?>
 * <UdpConnectReq version="String">
 * 	<sessionId>0</sessionId>
 *	<uuid>00000000-0000-0000-0000-000000000000</uuid>
 * </UdpConnectReq>
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_UdpConnectReq(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_UdpConnectResp(const char *buf,UdpConnectResp *ConnectResp)
 * 功能描述：解析UdpConnectResp xml 消息
 * 输入参数：buf - xml 数据
 * 输出参数：UdpConnectResp结构
 * 返 回 值    0 成功| 其他失败
 * 其它说明：xml 格式
 * <?xml version="1.0" encoding="UTF-8"?>
 * <UdpConnectResp version="String">
 * <sessionId>0</sessionId>
 * <uuid>00000000-0000-0000-0000-000000000000</uuid>
 * </UdpConnectResp>
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_UdpConnectResp(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_Heartbeat(const char *buf, CBaseEntityClass<char*> &obj)
 * 功能描述：解析 Heartbeat xml  消息
 * 输入参数：buf - xml 数据
 * 输出参数：Heartbeat  对象结构
 * 返 回 值    0 成功| 其他失败
 * 其它说明：xml 格式
 * <?xml version="1.0" encoding="UTF-8"?>
 * <Heartbeat version="String">
 * 	<sessionId>0</sessionId>
 *	<uuid>00000000-0000-0000-0000-000000000000</uuid>
 *	<Status>
 *		<statusCode>0</statusCode>
 *		<statusStr>String</statusStr>
 * 	</Status>
 * </Heartbeat>
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_Heartbeat(const char *buf, CBaseEntityClass<char*> &obj);
int parse_P2pHeartbeat(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_P2pResponse2(const  char *buf, CBaseEntityClass<char*> &obj)
 * 功能描述：解析P2pResponse2 xml 消息
 * 输入参数：buf - xml 数据
 * 输出参数：P2pResponse2 对象结构
 * 返 回 值   0 成功| 其他失败
 * 其它说明：xml 格式
 * <?xml version="1.0" encoding="UTF-8"?>
 * <P2pResponse2 version="String">
 * 	<sessionId>0</sessionId>
 *	<destUUID>00000000-0000-0000-0000-000000000000</destUUID>
 *	<srcUUID>00000000-0000-0000-0000-000000000000</srcUUID>
 *	<localIP>String</localIP>
 *	<localPort>0</localPort>
 *	<remoteIP>String</remoteIP>
 *	<remotePort>0</remotePort>
 *	<Status>
 *		<statusCode>0</statusCode>
 *		<statusStr>String</statusStr>
 *	</Status>
</P2pResponse2>
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_P2pResponse2(const  char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_P2pResponse1(const char *buf, CBaseEntityClass<char*> &obj)
 * 功能描述：解析P2pResponse1 xml 消息
 * 输入参数：buf - xml 数据
 * 输出参数：P2pResponse1 结构
 * 返 回 值    P2P_OK 成功| 其他失败
 * 其它说明：xml 格式
 * <?xml version="1.0" encoding="UTF-8"?>
 * <P2pResponse1 version="1.0">
 * 	<sessionId>0</sessionId>
 *	<Status>
 *		<statusCode>0</statusCode>
 *		<statusStr>String</statusStr>
 * 	</Status>
 * </P2pResponse1>
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_P2pResponse1(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_P2pRequest(const  char *buf,CBaseEntityClass<char*> &obj)
 * 功能描述：解析P2pRequest xml 消息
 * 输入参数：buf - xml 数据
 * 输出参数：P2pResponse2 结构
 * 返 回 值    0 成功| 其他失败
 * 其它说明：xml 格式
 * <?xml version="1.0" encoding="UTF-8"?>
 * <P2pRequest version="String">
 * 	<sessionId>0</sessionId>
 *	<destUUID>00000000-0000-0000-0000-000000000000</destUUID>
 *	<srcUUID>00000000-0000-0000-0000-000000000000</srcUUID>
 *	<localIP>String</localIP>
 *	<localPort>0</localPort>
 *	<remoteIP>String</remoteIP>
 *	<remotePort>0</remotePort>
 *  <natType>1</natType>
 *  <step>1</step>
 *	<streamInfo>2</streamInfo>
 * </P2pRequest>
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_P2pRequest(const  char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_P2pRequest2(const char *buf, CBaseEntityClass<char*> &obj)
 * 功能描述：生成P2pRequest2 xml 消息
 * 输入参数：P2pRequest2结构
 * 输出参数：buf
 * 返 回 值    0 成功| 其他失败
 * 其它说明：xml 格式
 *<?xml version="1.0" encoding="UTF-8"?>
 *  <P2pRequest2 version="1.0">
 *    <sessionId>0</sessionId>
 *    <destUUID>00000000-0000-0000-0000-000000000000</destUUID>
 *    <srcUUID>00000000-0000-0000-0000-000000000000</srcUUID>
 *  </ P2pRequest2>
 * 修改日期    版本号     修改人      修改内容
 * -----------------------------------------------
 * 2017/7/10      V1.0     XXXX       XXXX
 **************************************************************************/
int parse_P2pRequest2(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_RelayRequest(const  char *buf,CBaseEntityClass<char*> &obj)
 * 功能描述：解析RelayRequest xml 消息
 * 输入参数：buf - xml 数据
 * 输出参数：RelayRequest 对象结构
 * 返 回 值    0 成功| 其他失败
 * 其它说明：xml 格式
 * <?xml version="1.0" encoding="UTF-8"?>
 * <RelayRequest version="String">
 * 	<sessionId>0</sessionId>
 *	<destUUID>00000000-0000-0000-0000-000000000000</destUUID>
 *	<srcUUID>00000000-0000-0000-0000-000000000000</srcUUID>
 *	<enable>String</enable>
 *	<streamInfo>0</streamInfo>
 *	<relayIP>String</relayIP>
 *	<relayPort>0</relayPort>
 * </RelayRequest>
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_RelayRequest(const  char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
*  函数名称： parse_RelayResponse(const  char *buf, CBaseEntityClass<char*> &obj)
*  功能描述： 解释RelayResponse的XML结构
*  输入参数： RelayResponse的XML结构的buf
*  输出参数： CBaseEntityClass对象。
*  返 回 值：    0 成功| 其他失败
*  其它说明： xml 格式
* <?xml version="1.0" encoding="UTF-8"?>
* <RelayResponse  version="String">
* 	<sessionId>0</sessionId>
*	<destUUID>00000000-0000-0000-0000-000000000000</destUUID>
*	<srcUUID>00000000-0000-0000-0000-000000000000</srcUUID>
*	<localIP>String</localIP>
*	<localPort>0</localPort>
*	<remoteIP>String</remoteIP>
*	<remotePort>0</remotePort>
*	<Status>
*		<statusCode>0</statusCode>
*		<statusStr>String</statusStr>
*	</Status>
</RelayResponse >
* 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
**************************************************************************/
int parse_RelayResponse(const  char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_UpdateP2pRet(const char *buf,CBaseEntityClass<char*> &obj)
 * 功能描述：解析UpdateP2pRet xml 消息
 * 输入参数：buf - xml 数据?
 * 输出参数：UpdateP2pRet结构
 * 返 回 值    0成功| 其他失败
 * 其它说明：xml 格式
 * <?xml version="1.0" encoding="UTF-8"?>
 * <UpdateP2pRet version="1.0">
 *	<Status>
 * 	<sessionId>0</sessionId>
 *		<statusCode>0</statusCode>
 *		<statusStr>String</statusStr>
 * 	</Status>
 * </UpdateP2pRet>
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_UpdateP2pRet(const char *buf,CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_P2pClose(const char *buf,CBaseEntityClass<char*> &obj)
 * 功能描述：生成P2pClose xml 消息
 * 输入参数：P2pClose结构
 * 输出参数：buf
 * 返 回 值    0成功| 其他失败
 * 其它说明：xml 格式
 *<?xml version="1.0" encoding="UTF-8"?>
 *< P2pClose version="1.0">
 *  <sessionId>0</sessionId>
 *  <uuid>00000000-0000-0000-0000-000000000000</uuid>
 *</ P2pClose >
 * 修改日期    版本号     修改人      修改内容
 * -----------------------------------------------
 * 2017/7/10      V1.0     XXXX       XXXX
 **************************************************************************/
int parse_P2pClose(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * 函数名称：int parse_MainParam(const char *FileName, CBaseEntityClass<char*> &obj)
 * 功能描述：生成MainParam xml 对象结构
 * 输入参数：xml 文件名
 * 输出参数：MainParam对象结构
 * 返 回 值    0 成功| 其他失败
 * 其它说明：xml 格式
 * 修改日期    版本号     修改人	     修改内容
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_MainParam(const char *FileName, CBaseEntityClass<char*> &obj);

//xml格式私有协议的解释
void assemble_ProtocolInfo(const CBaseEntityClass<char*> &obj, char * buf);


/************************************************* 
Function:   	int parse_MsgHead(char *buf, Msg_head *msg_head)
Description:    解析消息头
parameters:
        buf:       传入字符串
        msg_head: 返回结构
Return:	       返回0 成功，其他失败
*************************************************/
private:
static int parse_MsgHead(char *buf, Msg_head *msg_head)
{     
    int ret = sscanf(buf, XML_HEAD2, msg_head->len, msg_head->type);
    /* 读取2个参数*/
    if (2 != ret ) 
    {
        ret = sscanf(buf, XML_HEAD3, msg_head->len, msg_head->type);
		if (2 != ret)
		   return -1;
    }
    return 0;
}
public:
static int process_checkHead(char *buf, Msg_head *msg_head)
{
    memset(msg_head, 0, sizeof(Msg_head));
    // 消息头部解析
    int nRet = parse_MsgHead(buf, msg_head);
    if (nRet != 0)
    {
        return -1;
    }
    return 0;
}

public:
static char* processMsgHead(char *buf)
{
    // 判断消息内容开始部分
    char* pxml= strstr(buf, XML_DATA_HEAD);
    if(NULL != pxml) 
    {
      return pxml;
    }
	return NULL;   
}
private:
	CBaseXMLHelper  _CBaseXMLHelper;
};
