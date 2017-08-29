#include "HearBeat.h"
#include "NatConst.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace natconst;
CHearBeat::CHearBeat(void)
{
	m_nHearBeatIntervalTime = 60;//默认间隔60秒
	memset(m_nHearBeatSessionID,0,10);
	memset(m_nDstUUID,0,32);
	memset(m_nSrcUUID,0,32);
	m_nHearBeatStatus = -1;
}

CHearBeat::~CHearBeat(void)
{
}

//设置心跳间隔
void CHearBeat::setHearBeatInterval(int nHearBeatInterval)
{
	m_nHearBeatIntervalTime = nHearBeatInterval;
}

int  CHearBeat::getHearBeatInterval()
{
	return m_nHearBeatIntervalTime;
}

//设置会话ID
void CHearBeat::setHearBeatSessionID(const char* szHearBeatSessionID)
{
	strcpy(m_nHearBeatSessionID,szHearBeatSessionID);
}

char* CHearBeat::getHearBeatSessionID()
{
	return m_nHearBeatSessionID;
}

//设置被检测端的UUID
void CHearBeat::setDstUUID(const char* szDstUUID)
{
	strcpy(m_nDstUUID,szDstUUID);
}
char*  CHearBeat::getDstUUID()
{
	return m_nDstUUID;
}

//设置检测端的UUID
void CHearBeat::setSrcUUID(const char* szSrcUUID)
{
	printf("%d\n",strlen(szSrcUUID));
	strcpy(m_nSrcUUID,szSrcUUID);
}
char*  CHearBeat::getSrcUUID()
{
	return m_nSrcUUID;
}
//设置监测状态
void CHearBeat::setHearBeatStatus(int nStatus)
{
	m_nHearBeatStatus = nStatus;
}

int  CHearBeat::getHearBeatStatus()
{
	return m_nHearBeatStatus;
}