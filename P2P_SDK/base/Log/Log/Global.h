#pragma once
#pragma execution_character_set("utf-8")
// ���ļ�Ϊutf-8 �����ʽ
class CLogErroInfoInterfaceMethod
{
public:
    CLogErroInfoInterfaceMethod(){};
    virtual ~CLogErroInfoInterfaceMethod(){};
    virtual bool Init() = 0;
    virtual bool Error(char* pStr) = 0;
    virtual bool PrintfInfo(char*pStr) = 0;
};
