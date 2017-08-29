#pragma once

#ifndef WIN32
#define _GNU_SOURCE 
#include <unistd.h>
#endif
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "tinyxml/tinyxml2.h"
#include "TemplateEntityClass.h"
//#include "PthreadMutexLock.h"
 

// XML 头组成关键字
#define MSG_HEAD_LEN                        "len:"
#define MSG_HEAD_TYPE						"type:"

// xml头格式定义
#define  XML_HEAD                       "len:%d\ntype:%s\n"
#define  XML_HEAD2						"len:%s\ntype:%s\n"
#define  XML_HEAD3						"%s\ntype:%s\n"
#define  XML_DATA_HEAD                  "<?xml"

#define XML_VERSION                         "version"
#define XML_VERSION_VALUE                   "1.0"


// xml 节点查找
#define XML_FIND_ELEMENT(root, child, text)    {\
        child= root->FirstChildElement( text );\
        if(!child) {\
            return false;\
        }\
    }

#define XML_TRY_FIND_ELEMENT(root, child, text)    {\
        child= root->FirstChildElement( text );\
        if(!child) {\
        }\
    }

// 消息头相关
typedef struct _msg_head_
{
    char len[12];
    char type[32];
}Msg_head;

class CBaseXMLHelper
{
public:
	CBaseXMLHelper(void);
	virtual ~CBaseXMLHelper(void);
	
public:
	//XMLDocument* getInstance();
	bool processRequest(char* buffer,CBaseEntityClass<char*> &obj);
    bool parseXML(const char* buffer,CBaseEntityClass<char*> &obj );
	void assembleObjToXML(const CBaseEntityClass<char*>& obj, char* buffer);
	void getXMLFileParam(const char *FileName,CBaseEntityClass<char*> &obj);
private:
	void assemble_XmlHead(XMLDocument *doc, const char *headstr, char *buf);
	bool parse_MsgHead(char *buf, Msg_head *msg_head);
private:
	//XMLDocument          _doc;
//	CThreadLock 		 _xmlhelperlock;
};

