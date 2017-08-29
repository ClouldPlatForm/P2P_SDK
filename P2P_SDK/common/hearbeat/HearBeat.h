#pragma once

class CHearBeat
{
public:
	CHearBeat(void);
	virtual ~CHearBeat(void);
public:
	//设置心跳间隔
	void setHearBeatInterval(int);
	int  getHearBeatInterval();
	//设置会话ID
	void setHearBeatSessionID(const char* szHearBeatSessionID);
	char* getHearBeatSessionID();
	//设置被检测端的UUID
	void setDstUUID(const char* szDstUUID);
	char*  getDstUUID();
	//设置检测端的UUID
	void setSrcUUID(const char* szSrcUUID);
	char*  getSrcUUID();
	//设置监测状态
	void setHearBeatStatus(int);
	int  getHearBeatStatus();
public:
	int    m_nHearBeatIntervalTime;
	char   m_nHearBeatSessionID[10];
	char   m_nDstUUID[32];
	char   m_nSrcUUID[32];
	int    m_nHearBeatStatus;
};
