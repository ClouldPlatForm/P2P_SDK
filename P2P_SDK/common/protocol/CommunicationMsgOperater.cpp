#include "CommunicationMsgOperater.h"
#include <map>

CCommunicationMsgOperater::CCommunicationMsgOperater(void)
{
}

CCommunicationMsgOperater::~CCommunicationMsgOperater(void)
{
}


int CCommunicationMsgOperater::parse_MsgResponse(const char*buf, CBaseEntityClass<char*> &obj)
{
    // 判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;	
	templateList.insert(std::pair<char*,paramType>("MsgResponse",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("Status",paramType(-1)));
	templateList.insert(std::pair<char*,paramType>("statusCode",paramType(9)));	
	templateList.insert(std::pair<char*,paramType>("statusStr",paramType(9)));
	
	obj.initList(templateList);
	
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;
}

int CCommunicationMsgOperater::parse_MsgResponse1(const char *buf, CBaseEntityClass<char*> &obj)
{
	// 判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("MsgResponse1",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("relayIP",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("relayPort",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("Status",paramType(-1)));
	templateList.insert(std::pair<char*,paramType>("statusCode",paramType(9)));	
	templateList.insert(std::pair<char*,paramType>("statusStr",paramType(9)));
	obj.initList(templateList);
	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;
}

int CCommunicationMsgOperater::parse_UpdateDevInfo(const char *buf, CBaseEntityClass<char*> &obj)
{
   // 判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("UpdateDevInfo",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("devType",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	obj.initList(templateList);
	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;  
}

int CCommunicationMsgOperater::parse_UpdateNatType(const char *buf,CBaseEntityClass<char*> &obj)
{
    // 判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("UpdateNatType",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("natType",paramType(4)));
	obj.initList(templateList);
	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;  
}

int CCommunicationMsgOperater::parse_UdpConnectReq(const char *buf, CBaseEntityClass<char*> &obj)
{
	 // 判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("UdpConnectReq",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	obj.initList(templateList);
	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;  
}

int CCommunicationMsgOperater::parse_UdpConnectResp(const char *buf,CBaseEntityClass<char*> &obj)
{
	 // 判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("UdpConnectResp",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	obj.initList(templateList);
	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1; 
}

int CCommunicationMsgOperater::parse_Heartbeat(const char *buf, CBaseEntityClass<char*> &obj)
{
	 // 判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("Heartbeat",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
//	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("Status",paramType(-1)));
	templateList.insert(std::pair<char*,paramType>("statusCode",paramType(9)));	
	templateList.insert(std::pair<char*,paramType>("statusStr",paramType(9)));
	obj.initList(templateList);
	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;  
}

int CCommunicationMsgOperater::parse_P2pHeartbeat(const char *buf, CBaseEntityClass<char*> &obj)
{
	 // 判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("P2pHeartbeat",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("Status",paramType(-1)));
	templateList.insert(std::pair<char*,paramType>("statusCode",paramType(9)));	
	templateList.insert(std::pair<char*,paramType>("statusStr",paramType(9)));
	obj.initList(templateList);
	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;  
}

int CCommunicationMsgOperater::parse_P2pResponse2(const  char *buf, CBaseEntityClass<char*> &obj)
{
    //判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("P2pResponse2",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("destUUID",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("srcUUID",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("localIP",paramType(4)));	
	templateList.insert(std::pair<char*,paramType>("localPort",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("remoteIP",paramType(4)));	
	templateList.insert(std::pair<char*,paramType>("remotePort",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("Status",paramType(-1)));
	templateList.insert(std::pair<char*,paramType>("statusCode",paramType(9)));	
	templateList.insert(std::pair<char*,paramType>("statusStr",paramType(9)));

	obj.initList(templateList);
	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;  
}

int CCommunicationMsgOperater::parse_P2pResponse1(const char *buf, CBaseEntityClass<char*> &obj)
{
	 //判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("P2pResponse1",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("Status",paramType(-1)));
	templateList.insert(std::pair<char*,paramType>("statusCode",paramType(9)));	
	templateList.insert(std::pair<char*,paramType>("statusStr",paramType(9)));

	obj.initList(templateList);
	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;
}

int CCommunicationMsgOperater::parse_P2pRequest(const char *buf, CBaseEntityClass<char*> &obj)
{
	 //判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("P2pRequest",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("destUUID",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("srcUUID",paramType(4)));	
	templateList.insert(std::pair<char*,paramType>("localIP",paramType(4)));	
	templateList.insert(std::pair<char*,paramType>("localPort",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("remoteIP",paramType(4)));	
	templateList.insert(std::pair<char*,paramType>("remotePort",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("natType",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("step",paramType(4)));	
	templateList.insert(std::pair<char*,paramType>("streamInfo",paramType(4)));

	obj.initList(templateList);
	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;
}

int CCommunicationMsgOperater::parse_P2pRequest2(const char *buf, CBaseEntityClass<char*> &obj)
{
	
	 //判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("P2pRequest2",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("destUUID",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("srcUUID",paramType(4)));	
	obj.initList(templateList);

	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;
}

int CCommunicationMsgOperater::parse_RelayRequest(const  char *buf, CBaseEntityClass<char*> &obj)
{
	 //判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("RelayRequest",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("destUUID",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("srcUUID",paramType(4)));	
	templateList.insert(std::pair<char*,paramType>("enable",paramType(4)));	
	templateList.insert(std::pair<char*,paramType>("streamInfo",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("relayIP",paramType(4)));	
	templateList.insert(std::pair<char*,paramType>("relayPort",paramType(4)));
	obj.initList(templateList);

	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;
}

int CCommunicationMsgOperater::parse_UpdateP2pRet(const char *buf,CBaseEntityClass<char*> &obj)
{
	//判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("UpdateP2pRet",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("Status",paramType(-1)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(9)));
	templateList.insert(std::pair<char*,paramType>("statusCode",paramType(9)));	
	templateList.insert(std::pair<char*,paramType>("statusStr",paramType(9)));	
	obj.initList(templateList);

	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;
}

int CCommunicationMsgOperater::parse_P2pClose(const char *buf, CBaseEntityClass<char*> &obj)
{
	//判断参数是否为空
    if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("P2pClose",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("uuid",paramType(4)));
	obj.initList(templateList);

	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;
}

int CCommunicationMsgOperater::parse_RelayResponse(const  char *buf, CBaseEntityClass<char*> &obj)
{
	//判断参数是否为空
	if(NULL == buf)
		return -1;
	//初始化参数
	std::map<char*,paramType> templateList;
	templateList.insert(std::pair<char*,paramType>("RelayResponse",paramType(0)));
	templateList.insert(std::pair<char*,paramType>("sessionId",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("destUUID",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("srcUUID",paramType(4)));	
	templateList.insert(std::pair<char*,paramType>("relayIP",paramType(4)));	
	templateList.insert(std::pair<char*,paramType>("relayPort",paramType(4)));
	templateList.insert(std::pair<char*,paramType>("protocolType",paramType(4)));	
	templateList.insert(std::pair<char*,paramType>("Status",paramType(-1)));
	templateList.insert(std::pair<char*,paramType>("statusCode",paramType(9)));	
	templateList.insert(std::pair<char*,paramType>("statusStr",paramType(-9)));
	obj.initList(templateList);

	//解释xml为对象
	bool bRet = _CBaseXMLHelper.parseXML(buf,obj);
	if(bRet)
		return 0;
	else
		return -1;
}

int CCommunicationMsgOperater::parse_MainParam(const char *FileName, CBaseEntityClass<char*> &obj)
{
	_CBaseXMLHelper.getXMLFileParam(FileName,obj);
	return 0;
}

void CCommunicationMsgOperater::assemble_ProtocolInfo(const CBaseEntityClass<char*> &obj, char * buf)
{
	_CBaseXMLHelper.assembleObjToXML(obj,buf);
}
