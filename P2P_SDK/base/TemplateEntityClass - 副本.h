#pragma once
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <map>
#include <vector>
#include "../common/commonUtil.h"
#define MAX_PARAM_NUM 15;
using namespace std;
template<class T>
class CBaseEntityClass
{
public:
	CBaseEntityClass();
	virtual ~CBaseEntityClass(void);
public:   
	void initList(std::map<char*,paramType> templateList);
	bool setParamValue(string szParamName,T szValue);
	T getParamValue(string szParamName) const;
	vector<T> getAllParamName();
	static T queryByParamName(string szParamName,std::map<char*,T> _TElementList);
	void getAllParamXML(std::map<char*,int> &paramXmlMap)
	{
		std::map<char*,int>::iterator itr = m_TElementDocList.begin();
		while(itr != m_TElementDocList.end())
		{
			paramXmlMap.insert(std::make_pair(itr->first,itr->second));
			++itr;
		}
	};
	std::map<char*,int> getAllParamXML1() const
	{
		return m_TElementDocList;
	}
private:
	std::map<char*,int>    m_T1ElementsList;
	std::map<char*,char>   m_T2ElementsList;
	std::map<char*,string> m_T3ElementsList;
	std::map<char*,char*>  m_T4ElementsList;
	std::map<char*,int>    m_TElementDocList;//xml结构map
	vector<T>				m_vecElementName;//xml结构的节点名称
};


template<class T>
CBaseEntityClass<T>::CBaseEntityClass()
{
}
template<class T>
CBaseEntityClass<T>::~CBaseEntityClass(void)
{
}
template<class T>
void CBaseEntityClass<T>::initList(std::map<char*,paramType> templateList)
{
	if(templateList.size() == 0)
      return;
	m_T1ElementsList.clear();
	m_T2ElementsList.clear();
	m_T3ElementsList.clear();
	m_T4ElementsList.clear();

	vector<PAIR> tVector;    
	sortMapByValue(templateList,tVector); 
	for(int j=0;j<tVector.size();j++)    
	{
			int i =0;
			m_vecElementName.push_back(tVector[j].first);
			{
				switch(tVector[j].second)
				{
					case ROOT_INT_TYPE:
						 i=int(ROOT_INT_TYPE);
						 m_T1ElementsList.insert(std::make_pair(tVector[j].first,0));
						 break;
					case SECOND_INT_TYPE:
						 i = int(SECOND_INT_TYPE);
						 m_T1ElementsList.insert(std::make_pair(tVector[j].first,0));
						 break;
					case ROOT_CHAR_TYPE:
						 i= int(ROOT_CHAR_TYPE);
						 m_T2ElementsList.insert(std::make_pair(tVector[j].first,'0'));
						 break;
					case SECOND_CHAR_TYPE:
						 i= int(SECOND_CHAR_TYPE);
						 m_T2ElementsList.insert(std::make_pair(tVector[j].first,'0'));
						 break;
					case ROOT_STRING_TYPE:
						 i = int(ROOT_STRING_TYPE);
						 m_T3ElementsList.insert(std::make_pair(tVector[j].first,"0"));
						 break;
					case SECOND_STRING_TYPE:
						 i = int(SECOND_STRING_TYPE);
						 m_T3ElementsList.insert(std::make_pair(tVector[j].first,"0"));
						 break;
					case ROOT_PCHAR_TYPE:
						 i = int(ROOT_PCHAR_TYPE);
					     m_T4ElementsList.insert(std::make_pair(tVector[j].first,const_cast<char*>("0")));
						 break;
				    case SECOND_PCHAR_TYPE:
						 i = int(SECOND_PCHAR_TYPE);
						 m_T4ElementsList.insert(std::make_pair(tVector[j].first,const_cast<char*>("0")));
						 break;
					case ROOT_TYPE:
					     i= 0;
						 break;
					case SECOND_ROOT_TYPE:
						 i= -1;
					default://-i表示第几个根节点
						 i = -2;
						 break;			
				}
				m_TElementDocList.insert(std::make_pair(tVector[j].first,i));
			}	
		}
	
}

template<class T>
T CBaseEntityClass<T>::queryByParamName(string szParamName,std::map<char*,T> _TElementList)
{
	std::map<char*,T>::iterator itr = _TElementList.begin();
	while(itr != _TElementList.end())
	{
		if(strcmp(itr->first,szParamName.c_str())==0)
		{
			return itr->second;
		}
		++itr;
	}
   return T(0);
}

template<class T>
bool CBaseEntityClass<T>::setParamValue(string szParamName,T szValue)
{
	const char* pType = typeid(szValue).name();	
	const char* pInt = strstr(pType,"int");
	const char* pChar = strstr(pType,"char");
	const char* pString = strstr(pType,"string");
	const char* pStruct = strstr(pType,"struct");
	const char* pCharP = strstr(pType,"char *");

	if(strcmp(pCharP,pChar) == 0)
	{
		pChar = NULL;
	}
	if(NULL != pInt)
	{
		std::map<char*,int>::iterator itr = m_T1ElementsList.begin();
		while(itr != m_T1ElementsList.end())
		{
			if(strcmp(itr->first,szParamName.c_str()) == 0)
			{
				itr->second = (int)szValue;
				return true;
			}
			itr++;
		}
	}
	else if(NULL != pChar)
	{
		std::map<char*,char>::iterator itr = m_T2ElementsList.begin();
		while(itr != m_T2ElementsList.end())
		{
			if(strcmp(itr->first,szParamName.c_str()) == 0)
			{
				itr->second = (char)szValue;
				return true;
			}
			itr++;
		}
	}
	else if(NULL != pString)
	{
		std::map<char*,string>::iterator itr = m_T3ElementsList.begin();
		while(itr != m_T3ElementsList.end())
		{
			if(strcmp(itr->first,szParamName.c_str()) == 0)
			{
				itr->second = (string)szValue;
				return true;
			}
			itr++;
		}
	}
	else if(NULL != pCharP)
	{
        std::map<char*,char*>::iterator itr = m_T4ElementsList.begin();
		while(itr != m_T4ElementsList.end())
		{
			if(strcmp(itr->first,szParamName.c_str()) == 0)
			{
				itr->second = (char*)szValue;
				return true;
			}
			itr++;
		}
	}
	return false;
}

template<class T>	
T CBaseEntityClass<T>::getParamValue(std::string szParamName) const
{//模板暂不支持string类型
	const char* pszType = typeid(T).name();
    const char* pInt = strstr(pszType,"int");
	const char* pChar = strstr(pszType,"char");
	//const char* pString = strstr(pszType,"string");
	const char* pStruct = strstr(pszType,"struct");
	const char* pCharP = strstr(pszType,"char *");

	if(NULL != pInt)
	{
		return (T)CBaseEntityClass<int>::queryByParamName(szParamName,m_T1ElementsList);
	}
	else if(NULL != pChar)
	{
		return (T)CBaseEntityClass<char>::queryByParamName(szParamName,m_T2ElementsList);
	}
	/*else if(NULL != pString)
	{
		return (T)CBaseEntityClass<string>::queryByParamName(szParamName,m_T3ElementsList);
	}*/
	else if(NULL != pCharP)
	{
		return (T)CBaseEntityClass<char*>::queryByParamName(szParamName,m_T4ElementsList);
	}
	
	return T(0);
}

template<class T>
vector<T> CBaseEntityClass<T>::getAllParamName()
{
//	if(m_vecElementName.size() > 0)
		return m_vecElementName;	
}
