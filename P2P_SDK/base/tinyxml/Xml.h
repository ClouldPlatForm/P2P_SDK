#pragma once
#include "tinyxml2.h"
#include <string>
#include "../../Global/Global.h"

using namespace std;

enum XML_TYPE
{
	GET_MEMSTRUCT_FROM_MEMXML = 0,
	CREATE_MEMXML_FROM_MEMSTRUCT = 1,
	GET_MEMSTRUCT_FROM_FILEXML = 2,
	CREATE_FILEXML_FROM_MEMSTRUCT = 3
	
};
class CXml
{
public:
	CXml();
	virtual ~CXml();
	//从内存XML或文件XML中获取结构体，从结构体中产生内存xml文件，或文件xml
    bool InitErroInfoMethod(void* pXMLErroInfoMethod);
    bool XMLRun(string &strMemOrFile, void * pData, XML_TYPE XmlType);
	bool GetMemXmlFromFileXML(string &MemXml, string &FileXml);
	bool CreateFileXMLFromMemXml(string &MemXml, string &FileXml);
    CErrorInfo* GetXMLErroInfoMethod(){ return m_pXMLErroInfoMethod; }
protected:
	XMLDocument m_Doc;
	XMLPrinter  m_Printer;
	virtual bool RunData(void * pData) = 0;	
	bool GetLeafNode(XMLNode* pElmParent, char* pszNode, char* pszText);
	bool AddLeafNode(XMLNode* pElmParent, const char* pszNode, const char* pszText);
	bool GetLeafElement(XMLNode* pParentNode, XMLElement* pElem, char* pszElemValue);
	bool AddLeafElement(XMLNode*pParentNode, const char* pSzElem,const char *pszText);
	bool GetLeafAttribute(XMLElement* pElem, char *pszAttribute, char *pszAttText);
	bool AddLeafAttribute(XMLElement * pElement, char * pAttributeName, char* pAttributeValue);	
	bool RemoveAttribute(XMLElement *pElement, char *pAttributeName);
    CErrorInfo* m_pXMLErroInfoMethod;
};

