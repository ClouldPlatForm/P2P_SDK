#include "Xml.h"
#include<iostream>
#include "Config.h"
CXml::CXml()
{
}


CXml::~CXml()
{
}
bool CXml::CreateFileXMLFromMemXml(string &MemXml, string &FileXml)
{
    XMLError  Ret = XML_NO_ERROR;

    try
    {
        Ret = m_Doc.Parse(MemXml.c_str());
        if (Ret != XML_NO_ERROR)
        {
            m_pXMLErroInfoMethod->Error(PRGNAME,__FILE__,__LINE__,__FUNCTION__,"Parse failed!\n");
            return false;
        }
        if (m_Doc.SaveFile((const char*)(FileXml.c_str())) != XML_NO_ERROR)
        {
			m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "SaveFile failed!\n");
            return false;
        }
    }
    catch (...)
    {
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "CreateFileXMLFromMemXml Exception!\n");
        return false;
    }
    

    return true;

}
bool CXml::GetMemXmlFromFileXML(string &MemXml, string &FileXml)
{
    XMLError  Ret = XML_NO_ERROR;
    try
    {
        if (m_Doc.LoadFile(FileXml.c_str()) != XML_NO_ERROR)
        {
			m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "LoadFile failed!\n");
            return false;
        }

        if (!m_Doc.Accept(&m_Printer))
        {
			m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "Accept failed!\n");
            return false;
        }
        MemXml = m_Printer.CStr();
        cout << MemXml << endl;


        Ret = m_Doc.Parse(MemXml.c_str());
        if (Ret != XML_NO_ERROR)
        {
			m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "Parse failed!\n");
            return false;
        }
    }
    catch (...)
    {
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "GetMemXmlFromFileXML Exception!\n");
        return false;
    }

    return true;
}
bool CXml::XMLRun(string &strMem, void * pData, XML_TYPE XmlType)
{
	if (NULL == pData)
	{
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "Parse failed!\n");
		return false;
	}
    try
    {
        switch (XmlType)
        {
        case GET_MEMSTRUCT_FROM_MEMXML:
        {

            if (XML_NO_ERROR != m_Doc.Parse(strMem.c_str()))
            {
				m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "Parse failed!\n");
                return false;
            }

            if (!RunData(pData))
            {
				m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "RunData failed!\n");
                return false;
            }

        }
        break;
        case CREATE_MEMXML_FROM_MEMSTRUCT:
        {
            if (!RunData(pData))
            {
				m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "RunData failed!\n");
                return false;
            }
            if (!m_Doc.Accept(&m_Printer))
            {
				m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "Accept failed!\n");
                return false;
            }
            strMem = m_Printer.CStr();
        }
        break;
        case CREATE_FILEXML_FROM_MEMSTRUCT:
        {
            m_Doc.LoadFile(strMem.c_str());
            if (!RunData(pData))
            {
				m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "LoadFile failed!\n");
                return false;
            }
            if (m_Doc.SaveFile(strMem.c_str()) != XML_NO_ERROR)
            {
				m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "SaveFile failed!\n");
                return false;
            }

        }
        break;
        case GET_MEMSTRUCT_FROM_FILEXML:
        {
            if (m_Doc.LoadFile(strMem.c_str()) != XML_NO_ERROR)
            {
				m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "LoadFile failed!\n");
                return false;
            }
            if (!RunData(pData))
            {
				m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "RunData failed!\n");
                return false;
            }
            // 										   if (m_Doc.(strMem.c_str()) != XML_NO_ERROR)
            // 										   {
            // 											   return false;
            // 										   }
        }
        break;
        default:
            break;
        }
    }
    catch (...)
    {
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "XMLRun Exception!\n");
        return false;
    }
    return true;
}

bool CXml::AddLeafNode(XMLNode* pElmParent, const char* pszNode, const char* pszText)
{
	if (NULL == pElmParent || NULL == pszNode || NULL == pszText)
	{
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "NULL Point  failed!\n");
		return false;
	}
    XMLElement *pElmNode;
    XMLText    *pElmText;

    pElmNode = m_Doc.NewElement(pszNode);
    pElmText = m_Doc.NewText(pszText);

    if (NULL == pElmNode->InsertEndChild(pElmText))
    {
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "InsertEndChild failed!\n");
        return false;
    }
    if (NULL == pElmParent->InsertEndChild(pElmNode))
    {
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "InsertEndChild failed!\n");
        return false;
    }
    return true;
}
//get leaf node
bool CXml::GetLeafNode(XMLNode* pElmParent, char* pszNode, char* pszText)
{
	if (NULL == pElmParent || NULL == pszNode || NULL == pszText)
	{
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "NULL Point  failed!\n");
		return false;
	}
    XMLNode *pTemp = NULL;

    pTemp = pElmParent->FirstChildElement(pszNode);
    if (NULL != pElmParent && NULL != pTemp)
    {
        pTemp = pTemp->FirstChild();
        if (NULL != pTemp)
        {
            strcpy(pszText, pTemp->Value());

            return true;
        }
    }
	m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "GetLeafNode failed!\n");
    return false;
}
bool CXml::GetLeafElement(XMLNode* pParentNode, XMLElement* pElem, char* pszElemValue)
{
	if (NULL == pParentNode || NULL == pParentNode || NULL == pElem)
	{
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "NULL Point  failed!\n");
		return false;
	}
    XMLElement * pElm = NULL;
    pElm = pParentNode->FirstChildElement(pszElemValue);
    if (NULL != pElm)
    {
        pElem = pElm;
        return true;
    }
	m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "GetLeafElement failed!\n");
    return false;
}
bool CXml::AddLeafElement(XMLNode*pParentNode, const char* pSzElem, const char *pszText)
{
    if (NULL == pParentNode || NULL == pSzElem)
    {
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "AddLeafElement failed!\n");
        return false;
    }

    XMLElement *pElem = m_Doc.NewElement(pSzElem);
    pElem->SetText(pszText);
    pParentNode->LinkEndChild(pElem);
    return true;
}
bool CXml::GetLeafAttribute(XMLElement* pElem, char *pszAttribute, char *pszAttText)
{
    if (NULL == pElem || NULL == pszAttribute || NULL == pszAttText)
    {
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "GetLeafAttribute failed!\n");
        return false;
    }
    char * ptr = NULL;
    ptr = (char*)pElem->Attribute(pszAttribute);
    if (NULL == ptr)
    {
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "GetLeafAttribute failed!\n");
        return false;
    }
    strcpy(pszAttText, ptr);
    return true;
}
bool CXml::AddLeafAttribute(XMLElement * pElement, char * pAttributeName, char* pAttributeValue)
{
    if (NULL == pElement || NULL == pAttributeName || NULL == pAttributeValue)
    {
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "AddLeafAttribute failed!\n");
        return false;
    }
    pElement->SetAttribute(pAttributeName, pAttributeValue);
    return true;
}
bool CXml::RemoveAttribute(XMLElement *pElement, char *pAttributeName)
{
    if (NULL == pElement || NULL == pAttributeName)
    {
		m_pXMLErroInfoMethod->Error(PRGNAME, __FILE__, __LINE__, __FUNCTION__, "RemoveAttribute failed!\n");
        return false;
    }
    pElement->DeleteAttribute(pAttributeName);
    return true;
}

bool CXml::InitErroInfoMethod(void* pXMLErroInfoMethod)
{
    if (NULL == pXMLErroInfoMethod)
    {
		ScreenPrintf(PRGNAME, "pXMLErrorInfoMethod == NULL Failed \n");
        return false;
    }

    m_pXMLErroInfoMethod = (CErrorInfo*)pXMLErroInfoMethod;
    return true;
}


