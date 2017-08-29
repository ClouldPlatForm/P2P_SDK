#pragma once
#include "../../base/basexmlhelper.h"

class CCommunicationMsgOperater
{
public:
	CCommunicationMsgOperater(void);
	~CCommunicationMsgOperater(void);
public:
 /**************************************************************************
 * �������ƣ�int parse_MsgResponse(const char*buf, CBaseEntityClass<char*> &obj)
 * ��������������MsgResponse xml �ṹ
 * ���������buf - xml ����
 * ���������MsgResponse����ṹ
 * �� �� ֵ    0 �ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
 * <?xml version="1.0" encoding="UTF-8"?>
 * <MsgResponse version="1.0">
 * 	<sessionId>0</sessionId>
 *	<Status>
 *		<statusCode>0</statusCode>
 *		<statusStr>String</statusStr>
 * 	</Status>
 * </MsgResponse>
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/7	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_MsgResponse(const char*buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_MsgResponse1(const char*buf, CBaseEntityClass<char*> &obj)
 * ��������������MsgResponse1 xml �ṹ
 * ���������buf - xml ����
 * ���������MsgResponse1����ṹ
 * �� �� ֵ    0 �ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
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
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_MsgResponse1(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_UpdateDevInfo(const char *buf, CBaseEntityClass<char*> &obj)
 * �������������� UpdateDevInfo xml  ��Ϣ
 * ���������buf - xml ����
 * ���������updateDevInfo����ṹ
 * �� �� ֵ��   0 �ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
 * <?xml version="1.0" encoding="UTF-8"?>
 * <UpdateDevInfo version="1.0">
 *	<sessionId>0</sessionId>
 *	<devType>APP</devType>
 *	<uuid>00000000-0000-0000-0000-000000000000</uuid>
 * </UpdateDevInfo>
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_UpdateDevInfo(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_UpdateNatType(const char *buf, CBaseEntityClass<char*> &obj)
 * �������������� UpdateNatType xml  ��Ϣ
 * ���������buf - xml ����
 * ���������UpdateNatType����ṹ
 * �� �� ֵ��   0 �ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
 * <?xml version="1.0" encoding="UTF-8"?>
 * <UpdateNatType version="1.0">
 *	<sessionId>0</sessionId>
 *  <uuid>0000</uuid>
 *	<natType>APP</natType>
 * </UpdateNatType>
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_UpdateNatType(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_UdpConnectReq(const char *buf, CBaseEntityClass<char*> &obj)
 * �������������� UdpConnectReq xml  ��Ϣ
 * ���������buf - xml ����
 * ���������UdpConnectReq����ṹ
 * �� �� ֵ    0�ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
 * <?xml version="1.0" encoding="UTF-8"?>
 * <UdpConnectReq version="String">
 * 	<sessionId>0</sessionId>
 *	<uuid>00000000-0000-0000-0000-000000000000</uuid>
 * </UdpConnectReq>
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_UdpConnectReq(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_UdpConnectResp(const char *buf,UdpConnectResp *ConnectResp)
 * ��������������UdpConnectResp xml ��Ϣ
 * ���������buf - xml ����
 * ���������UdpConnectResp�ṹ
 * �� �� ֵ    0 �ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
 * <?xml version="1.0" encoding="UTF-8"?>
 * <UdpConnectResp version="String">
 * <sessionId>0</sessionId>
 * <uuid>00000000-0000-0000-0000-000000000000</uuid>
 * </UdpConnectResp>
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_UdpConnectResp(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_Heartbeat(const char *buf, CBaseEntityClass<char*> &obj)
 * �������������� Heartbeat xml  ��Ϣ
 * ���������buf - xml ����
 * ���������Heartbeat  ����ṹ
 * �� �� ֵ    0 �ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
 * <?xml version="1.0" encoding="UTF-8"?>
 * <Heartbeat version="String">
 * 	<sessionId>0</sessionId>
 *	<uuid>00000000-0000-0000-0000-000000000000</uuid>
 *	<Status>
 *		<statusCode>0</statusCode>
 *		<statusStr>String</statusStr>
 * 	</Status>
 * </Heartbeat>
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_Heartbeat(const char *buf, CBaseEntityClass<char*> &obj);
int parse_P2pHeartbeat(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_P2pResponse2(const  char *buf, CBaseEntityClass<char*> &obj)
 * ��������������P2pResponse2 xml ��Ϣ
 * ���������buf - xml ����
 * ���������P2pResponse2 ����ṹ
 * �� �� ֵ   0 �ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
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
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_P2pResponse2(const  char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_P2pResponse1(const char *buf, CBaseEntityClass<char*> &obj)
 * ��������������P2pResponse1 xml ��Ϣ
 * ���������buf - xml ����
 * ���������P2pResponse1 �ṹ
 * �� �� ֵ    P2P_OK �ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
 * <?xml version="1.0" encoding="UTF-8"?>
 * <P2pResponse1 version="1.0">
 * 	<sessionId>0</sessionId>
 *	<Status>
 *		<statusCode>0</statusCode>
 *		<statusStr>String</statusStr>
 * 	</Status>
 * </P2pResponse1>
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_P2pResponse1(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_P2pRequest(const  char *buf,CBaseEntityClass<char*> &obj)
 * ��������������P2pRequest xml ��Ϣ
 * ���������buf - xml ����
 * ���������P2pResponse2 �ṹ
 * �� �� ֵ    0 �ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
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
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_P2pRequest(const  char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_P2pRequest2(const char *buf, CBaseEntityClass<char*> &obj)
 * ��������������P2pRequest2 xml ��Ϣ
 * ���������P2pRequest2�ṹ
 * ���������buf
 * �� �� ֵ    0 �ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
 *<?xml version="1.0" encoding="UTF-8"?>
 *  <P2pRequest2 version="1.0">
 *    <sessionId>0</sessionId>
 *    <destUUID>00000000-0000-0000-0000-000000000000</destUUID>
 *    <srcUUID>00000000-0000-0000-0000-000000000000</srcUUID>
 *  </ P2pRequest2>
 * �޸�����    �汾��     �޸���      �޸�����
 * -----------------------------------------------
 * 2017/7/10      V1.0     XXXX       XXXX
 **************************************************************************/
int parse_P2pRequest2(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_RelayRequest(const  char *buf,CBaseEntityClass<char*> &obj)
 * ��������������RelayRequest xml ��Ϣ
 * ���������buf - xml ����
 * ���������RelayRequest ����ṹ
 * �� �� ֵ    0 �ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
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
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_RelayRequest(const  char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
*  �������ƣ� parse_RelayResponse(const  char *buf, CBaseEntityClass<char*> &obj)
*  ���������� ����RelayResponse��XML�ṹ
*  ��������� RelayResponse��XML�ṹ��buf
*  ��������� CBaseEntityClass����
*  �� �� ֵ��    0 �ɹ�| ����ʧ��
*  ����˵���� xml ��ʽ
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
* �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
**************************************************************************/
int parse_RelayResponse(const  char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_UpdateP2pRet(const char *buf,CBaseEntityClass<char*> &obj)
 * ��������������UpdateP2pRet xml ��Ϣ
 * ���������buf - xml ����?
 * ���������UpdateP2pRet�ṹ
 * �� �� ֵ    0�ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
 * <?xml version="1.0" encoding="UTF-8"?>
 * <UpdateP2pRet version="1.0">
 *	<Status>
 * 	<sessionId>0</sessionId>
 *		<statusCode>0</statusCode>
 *		<statusStr>String</statusStr>
 * 	</Status>
 * </UpdateP2pRet>
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_UpdateP2pRet(const char *buf,CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_P2pClose(const char *buf,CBaseEntityClass<char*> &obj)
 * ��������������P2pClose xml ��Ϣ
 * ���������P2pClose�ṹ
 * ���������buf
 * �� �� ֵ    0�ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
 *<?xml version="1.0" encoding="UTF-8"?>
 *< P2pClose version="1.0">
 *  <sessionId>0</sessionId>
 *  <uuid>00000000-0000-0000-0000-000000000000</uuid>
 *</ P2pClose >
 * �޸�����    �汾��     �޸���      �޸�����
 * -----------------------------------------------
 * 2017/7/10      V1.0     XXXX       XXXX
 **************************************************************************/
int parse_P2pClose(const char *buf, CBaseEntityClass<char*> &obj);

/**************************************************************************
 * �������ƣ�int parse_MainParam(const char *FileName, CBaseEntityClass<char*> &obj)
 * ��������������MainParam xml ����ṹ
 * ���������xml �ļ���
 * ���������MainParam����ṹ
 * �� �� ֵ    0 �ɹ�| ����ʧ��
 * ����˵����xml ��ʽ
 * �޸�����    �汾��     �޸���	     �޸�����
 * -----------------------------------------------
 * 2017/7/10	     V1.0	    XXXX	      XXXX
 **************************************************************************/
int parse_MainParam(const char *FileName, CBaseEntityClass<char*> &obj);

//xml��ʽ˽��Э��Ľ���
void assemble_ProtocolInfo(const CBaseEntityClass<char*> &obj, char * buf);


/************************************************* 
Function:   	int parse_MsgHead(char *buf, Msg_head *msg_head)
Description:    ������Ϣͷ
parameters:
        buf:       �����ַ���
        msg_head: ���ؽṹ
Return:	       ����0 �ɹ�������ʧ��
*************************************************/
private:
static int parse_MsgHead(char *buf, Msg_head *msg_head)
{     
    int ret = sscanf(buf, XML_HEAD2, msg_head->len, msg_head->type);
    /* ��ȡ2������*/
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
    // ��Ϣͷ������
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
    // �ж���Ϣ���ݿ�ʼ����
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
