#include "RouterInfo.h"
#include "const/NatConst.h"
#include "stun.h"
using namespace natconst;

RouterInfo::RouterInfo()
{
	memset(m_szPubIPAddr,0,16);
	m_nPubPort=-1;
	memset(m_szLocalIPAddr,0,16);
	m_nLocalPort=-1;
	m_nRouterNatType= NATTYPE_UNKNOW;
	memset(m_szRouterNatType,0,32);
	strcpy(m_szRouterNatType,"unKnown Type");
	m_nChangeStep=0;
	m_nNatDetectionStep = NatDetectionStep::NatDetectionStep_INIT;
	m_szRouterName="";
}

RouterInfo::~RouterInfo()
{
}

void RouterInfo::Reset()
{
	m_nRouterNatType= NATTYPE_UNKNOW;
	strcpy(m_szRouterNatType,"unKnown Type");
	m_nChangeStep=0;
	m_nNatDetectionStep = NatDetectionStep_INIT;
	m_szUUIDVec.clear();
}

void RouterInfo::setPubIPAddr(char* pubIP)
{
	strcpy(m_szPubIPAddr,pubIP);
}

char* RouterInfo::getPubIPAddr()
{
	return m_szPubIPAddr;
}

void RouterInfo::setLocalIPAddr(char* pubIP)
{
	strcpy(m_szLocalIPAddr,pubIP);
}
char* RouterInfo::getLocalIPAddr()
{
	return m_szLocalIPAddr;
}

void RouterInfo::setPubPortAddr(int pubPort)
{
	m_nPubPort = pubPort;
}
int RouterInfo::getPubPortAddr()
{
	return m_nPubPort;
}

void RouterInfo::setLocalPortAddr(int pubPort)
{
	m_nLocalPort = pubPort;
}

int RouterInfo::getLocalPortAddr()
{
	return m_nLocalPort;
}

int RouterInfo::getNatType()
{
	return m_nRouterNatType;
}

void RouterInfo::setNatType(int ntype)
{
	m_nRouterNatType = ntype;
}

std::string RouterInfo::getNatTypeString(int ntype)
{
	switch(ntype)
	{
	case NATTYPE_UNKNOW:
		strcpy(m_szRouterNatType,"unKnown Type");
		break;
	//case NATTYPE_FULL_CONE:
	case StunTypeIndependentFilter:
		strcpy(m_szRouterNatType,"Nat Type:FC");
		break;
	//case NATTYPE_RESTRICTED_CONE:
	case StunTypeDependentFilter:
		strcpy(m_szRouterNatType,"Nat Type:RC");
		break;
	//case NATTYPE_PORT_RESTRICTED_CONE:
	case StunTypePortDependedFilter:
		strcpy(m_szRouterNatType,"Nat Type:PC");
		break;
	//case NATTYPE_SYMMETRIC:
	case StunTypeDependentMapping:
		strcpy(m_szRouterNatType,"Nat Type:SN");
		break;
	//case NATTYPE_SYMMETRIC_FIREWALL:
		strcpy(m_szRouterNatType,"Nat Type:FW");
		break;
	case StunTypeIpChange:
		strcpy(m_szRouterNatType,"Nat Type:SN,IP changed");
	default:
		strcpy(m_szRouterNatType,"Nat Type:other SN");
		break;
	}
	return m_szRouterNatType;
}
	
void RouterInfo::setPortChangStep(int nstep)
{
	m_nChangeStep = nstep;
}

int RouterInfo::getPortChangStep()
{
	return m_nChangeStep;
}

void RouterInfo::setNatDetectionStep(int nNatDetectionStep)
{
	m_nNatDetectionStep = nNatDetectionStep;
}

int RouterInfo::getNatDetectionStep()
{
	return m_nNatDetectionStep;
}

void RouterInfo::setNatBeatHoleResult(int nNatBeatHoleRet)
{
	m_nNatBeatHoleRet = nNatBeatHoleRet;
}

int RouterInfo::getNatBeatHoleResult()
{
	return m_nNatBeatHoleRet;
}

std::string RouterInfo::getNatBeatHoleString()
{
	if(m_nNatBeatHoleRet == -1)
	{
      return "Nat Beat Hole Failed!\n";
	}
	else if(m_nNatBeatHoleRet >= 0)
	{
	  return "Nat Beat Hole success!\n";
	}

	return "Nat Beat Hole Result does not defined!\n";
}