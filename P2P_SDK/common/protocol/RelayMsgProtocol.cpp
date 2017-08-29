#include "RelayMsgProtocol.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
RelayMsgHandler::RelayMsgHandler()
{

}

RelayMsgHandler::~RelayMsgHandler()
{

}

int RelayMsgHandler::findRelayMagicPoint(char *buf)
{
	int nSize = strlen(buf);
	if ( nSize < 8 /*RELAY_PROTOCOL_HEADLENGTH*/ )
	{
		return -1;
	}
	int nStartPoint = -1;
	isMagic(buf,nStartPoint);
    
	return nStartPoint;
}

bool RelayMsgHandler::isMagic(char *buf,int& nStartPoint)
{
	for (int i=0;i<strlen(buf);i++)
	{	
		if ( (*(buf+i) == 0x34) && (*(buf+i+1) == 0x12) )
		{
			 nStartPoint = i;
			 return true;
		}
	}
	return false;
}

void RelayMsgHandler::assembleRelayMsg(_reqData_t_ relayQuestMsg, char *buf)
{
    relay_protocol_header_t *relayReqHead;
	relayReqHead = (relay_protocol_header_t *)buf;
	memset(relayReqHead, 0x00, sizeof(relay_protocol_header_t));
	memset(buf, 0x00, sizeof(buf));
	relayReqHead->magic = MAGIC;
	relayReqHead->cmd = relayQuestMsg.reqHeader.cmd;
	relayReqHead->len = strlen((char*)relayQuestMsg.pMsgBody);
	relayReqHead->code = 0;
	strcpy( (buf+8),(char*)(relayQuestMsg.pMsgBody) );
}


void RelayMsgHandler::parserRelayMsg(char *buf, _respData_t_ &relayResponseMsg)
{
   //nStartPoint´Ó0¿ªÊ¼Ëã
   int nStartPoint = findRelayMagicPoint(buf);
   if ((strlen(buf)-nStartPoint) < 8)
   {
	   return;
   }
   memset(&relayResponseMsg,0,1024*8);
   memcpy(&relayResponseMsg,buf+nStartPoint,(strlen(buf)-nStartPoint-1) ); 
}

