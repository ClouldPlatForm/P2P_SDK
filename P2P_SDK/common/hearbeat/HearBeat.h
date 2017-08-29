#pragma once

class CHearBeat
{
public:
	CHearBeat(void);
	virtual ~CHearBeat(void);
public:
	//�����������
	void setHearBeatInterval(int);
	int  getHearBeatInterval();
	//���ûỰID
	void setHearBeatSessionID(const char* szHearBeatSessionID);
	char* getHearBeatSessionID();
	//���ñ����˵�UUID
	void setDstUUID(const char* szDstUUID);
	char*  getDstUUID();
	//���ü��˵�UUID
	void setSrcUUID(const char* szSrcUUID);
	char*  getSrcUUID();
	//���ü��״̬
	void setHearBeatStatus(int);
	int  getHearBeatStatus();
public:
	int    m_nHearBeatIntervalTime;
	char   m_nHearBeatSessionID[10];
	char   m_nDstUUID[32];
	char   m_nSrcUUID[32];
	int    m_nHearBeatStatus;
};
