#pragma once
#include <iostream>
#include <stdio.h>
#include <vector>

class RouterInfo
{
public:
	RouterInfo(void);
	~RouterInfo();
public:
	void setSrcUUIDVec(char* szUUID)
	{
		m_szUUIDVec.push_back(szUUID);
	};
	std::vector<char*> getUUIDVec()
	{
		return m_szUUIDVec;
	};
	void setPubIPAddr(char* pubIP);
	char* getPubIPAddr();
    void setLocalIPAddr(char* localIP);
	char* getLocalIPAddr();
	void setPubPortAddr(int pubPort);
	int getPubPortAddr();
	void setLocalPortAddr(int localPort);
	int getLocalPortAddr();

    int getNatType();
	void setNatType(int ntype);

	std::string getNatTypeString(int ntype);
	//����port�ı䲽��
	void setPortChangStep(int nstep);
	int getPortChangStep();

	//����·�ɴ򶴲���
	void setNatDetectionStep(int nNatDetectionStep);
	int getNatDetectionStep();

	void setNatBeatHoleResult(int nNatBeatHoleRet);
	int getNatBeatHoleResult();
	std::string getNatBeatHoleString();

	void Reset();
private:
	std::vector<char*>  m_szUUIDVec;
	char  m_szPubIPAddr[16];
	int m_nPubPort;
	char  m_szLocalIPAddr[16];
	int m_nLocalPort;
    int m_nRouterNatType;
	char  m_szRouterNatType[32];
	int m_nChangeStep;
	std::string m_szRouterName;
	int m_nNatDetectionStep;
	int m_nNatBeatHoleRet;
};