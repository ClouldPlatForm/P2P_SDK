#ifndef __MSGCONST_H__
#define __MSGCONST_H__

// status结构
typedef struct _status_
{
    char statusCode[12];
    char statusStr[24];
}Status;

// MsgResponse 消息成员
typedef struct _msgResponse_
{
    char sessionId[12];
    Status status;
    char relaySrvIP[32];
    char relaySrvPort[12];
}MsgResponse;





#endif