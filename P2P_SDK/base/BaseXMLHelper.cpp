#include "BaseXMLHelper.h"
#include "../common/commonUtil.h"

CBaseXMLHelper::CBaseXMLHelper(void)
{
}


CBaseXMLHelper::~CBaseXMLHelper(void)
{
}

//XMLDocument* CBaseXMLHelper::getInstance()
//{
//	m_cAutoLock(&_XMLHELPER_LOCK);
//	if(NULL == _doc)
//	{
//		_doc = new XMLDocument();
//	}
//
//	return _doc;
//}

bool CBaseXMLHelper::parseXML(const char* buffer,CBaseEntityClass<char*> &obj )
{
//	PTHREAD_MUTEX_LOCK(&_xmlhelperlock);
	XMLDocument  _doc;
	_doc.Parse(buffer);

	if(_doc.ErrorID()) 
	{
		return false;
	}
	XMLDocument *pDoc = &_doc;

	std::map<char*,int> paramXMLMap; 
	obj.getAllParamXML(paramXMLMap);
	
	vector<PAIR > tVector;
	sortMapByValue(paramXMLMap,tVector);

//	std::map<char*,int>::iterator itr= paramXMLMap.begin();
	std::vector<PAIR>::iterator itr = tVector.begin();
	XMLElement* preNode;
	XMLElement* pFirstNode;
	XMLElement* pSecondNode;
	while(itr != tVector.end())
	{
		XMLElement* pNode; //当前节点
		switch(itr->second)
		{
		case ROOT_TYPE:
			 XML_TRY_FIND_ELEMENT(pDoc, pNode, itr->first);
			 preNode = pNode;
			 pFirstNode = pNode;
			 break;
		case ROOT_INT_TYPE:
		case ROOT_CHAR_TYPE:
		case ROOT_STRING_TYPE:
		case ROOT_PCHAR_TYPE:
		case ROOT_INTP_TYPE:
			{ 
			 XML_TRY_FIND_ELEMENT(pFirstNode, pNode, itr->first);	
			 obj.setParamValue(itr->first,const_cast<char*>(pNode->GetText()));
			 break;
			}
		case SECOND_ROOT_TYPE:
			 XML_TRY_FIND_ELEMENT(preNode, pNode, itr->first);
			 preNode = pNode;
			 pSecondNode = pNode;
             break;
		case SECOND_INT_TYPE:
		case SECOND_CHAR_TYPE:
		case SECOND_STRING_TYPE:
		case SECOND_PCHAR_TYPE:
		case SECOND_INTP_TYPE:
			XML_TRY_FIND_ELEMENT(pSecondNode, pNode, itr->first);
			obj.setParamValue(itr->first,const_cast<char*>(pNode->GetText()));
			break;
		case UNKNOW:
		default:
			break;
		}
		itr++;
	}
	return true;
}

void CBaseXMLHelper::assembleObjToXML(const CBaseEntityClass<char*>& obj, char* buffer)
{
//	PTHREAD_MUTEX_LOCK(&_xmlhelperlock);
    //创建一个XML的文档对象。 
	XMLText *pText;
	 //创建<?xml version="1.0" encoding="UTF-8"?>
	XMLDocument  _doc;
	_doc.InsertEndChild( _doc.NewDeclaration() );

	std::map<char*,int> paramMap = obj.getAllParamXML1();
	//map排序
    vector<PAIR > tVector;
	sortMapByValue(paramMap,tVector);

	XMLElement* preFirstRootElement = NULL;
	XMLElement* preSecondRootElement = NULL;
	char* szFirstRootNode = "";
	char* szValue = NULL;
	for(int i=0;i<tVector.size();i++)
	{
		char* szKey = tVector[i].first;
		int     nValue = tVector[i].second;
		XMLElement *devInfo = _doc.NewElement(szKey);
		switch(nValue)
		{
			case ROOT_TYPE:
				_doc.InsertEndChild(devInfo);
				devInfo->SetAttribute(XML_VERSION, XML_VERSION_VALUE);
				preFirstRootElement = devInfo;
				szFirstRootNode = szKey;
				break;
			case ROOT_INT_TYPE:
			case ROOT_CHAR_TYPE:
			case ROOT_STRING_TYPE:
			case ROOT_PCHAR_TYPE:
			case ROOT_INTP_TYPE:
				preFirstRootElement->InsertEndChild(devInfo);
				szValue =  obj.getParamValue(szKey);
				pText = _doc.NewText(szValue);
				devInfo->InsertEndChild(pText);
				break;
			case SECOND_ROOT_TYPE:
				preFirstRootElement->InsertEndChild(devInfo);	
				preSecondRootElement = devInfo;
				break;
			case SECOND_INT_TYPE:
			case SECOND_CHAR_TYPE:
			case SECOND_STRING_TYPE:
			case SECOND_PCHAR_TYPE:
			case SECOND_INTP_TYPE:
				preSecondRootElement->InsertEndChild(devInfo);	
				pText = _doc.NewText(obj.getParamValue(szKey));
				devInfo->InsertEndChild(pText);
				break;
			case UNKNOW:
			default:
				break;
		}
	}

	assemble_XmlHead(&_doc,szFirstRootNode, buffer);
}

void CBaseXMLHelper::assemble_XmlHead(XMLDocument *doc, const char *headstr, char *buf)
{
 //  PTHREAD_MUTEX_LOCK(&_xmlhelperlock);
	int len = 0;  
    XMLPrinter printer;
 //   XMLDocument  _doc;
//	_doc.Print( &printer);
     doc->Print( &printer);
    //printer.CStrSize() 要比strlen()多出一个字节，作 -1  操作
    len = sprintf(buf, XML_HEAD, printer.CStrSize()-1, headstr);
    strcpy(buf+len, printer.CStr());
}

bool CBaseXMLHelper::parse_MsgHead(char *buf, Msg_head *msg_head)
{
  // PTHREAD_MUTEX_LOCK(&_xmlhelperlock);
	if(NULL == buf || NULL == msg_head)
		return false;
	int ret = sscanf(buf, XML_HEAD2, msg_head->len, msg_head->type);
    /* 读取2个参数*/
    if (2 != ret ) 
    {
        return false;
    }
	return true;
}

bool CBaseXMLHelper::processRequest(char* buffer,CBaseEntityClass<char*> &obj)
{
//	PTHREAD_MUTEX_LOCK(&_xmlhelperlock);
	if(NULL == buffer)
		return false;

	Msg_head msg_head;
	memset(&msg_head, 0, sizeof(msg_head));

    // 消息头部解析
   
    int ret = parse_MsgHead(buffer, &msg_head);
    if (ret != 0) 
    {
       /* LOG_ERROR("ret =%d , recvf buf ===\n%s\n", ret , buf);*/
        return false;
    }
    
	vector<char*> szParamVec = obj.getAllParamName();
    //检验头类型
	if(!strcmp(msg_head.type, szParamVec[0])) 
    {
        return false;
    }
    
     // 解析消息内容
#ifndef WIN32
	char* pszXml= strcasestr(buffer, XML_DATA_HEAD); 
#else
	char* pszXml= strstr(buffer, XML_DATA_HEAD); 
#endif
    if(NULL == pszXml) 
    {
        return false;
    }
    
	parseXML(pszXml,obj);

	return true;
}

void CBaseXMLHelper::getXMLFileParam(const char *FileName,CBaseEntityClass<char*> &obj)
{
//	PTHREAD_MUTEX_LOCK(&_xmlhelperlock);
	if(NULL == FileName)
		return;
	 XMLDocument  _doc;
	_doc.LoadFile(FileName);
    if(_doc.ErrorID()) 
    {
        	printf("parse_ret=%d path=[%s]\n", _doc.ErrorID(), FileName);
        	return;
    }	
	XMLDocument *pDoc = &_doc;

	std::map<char*,int> paramXMLMap; 
	obj.getAllParamXML(paramXMLMap);
	
	std::map<char*,int>::iterator itr= paramXMLMap.begin();
	XMLElement* pfirstNode=NULL;
	XMLElement* psecondNode=NULL;
	char* szFirstParam ="";
	while(itr != paramXMLMap.end())
	{
		XMLElement* pNode ; //当前节点
		switch(itr->second)
		{
		case ROOT_TYPE:
			 XML_TRY_FIND_ELEMENT(pDoc, pNode, itr->first);
			  pfirstNode = pNode;
			 break;
		case ROOT_INT_TYPE:
		case ROOT_CHAR_TYPE:
		case ROOT_STRING_TYPE:
		case ROOT_PCHAR_TYPE:
		case ROOT_INTP_TYPE:
			 XML_TRY_FIND_ELEMENT(pfirstNode, pNode, itr->first);
			 obj.setParamValue(itr->first,const_cast<char*>(pNode->GetText()));
			 break;
		case SECOND_ROOT_TYPE:
			 XML_TRY_FIND_ELEMENT(pfirstNode, pNode, itr->first);
			 psecondNode = pNode;
             break;
		case SECOND_INT_TYPE:
		case SECOND_CHAR_TYPE:
		case SECOND_STRING_TYPE:
		case SECOND_PCHAR_TYPE:
		case SECOND_INTP_TYPE:
			XML_TRY_FIND_ELEMENT(psecondNode, pNode, itr->first);
			obj.setParamValue(itr->first,const_cast<char*>(pNode->GetText()));
			break;
		case UNKNOW:
		default:
			break;
		}
		itr++;
	}
}
      