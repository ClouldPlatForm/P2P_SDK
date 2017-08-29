#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式
class CLogErroInfoInterfaceMethod
{
public:
    CLogErroInfoInterfaceMethod(){};
    virtual ~CLogErroInfoInterfaceMethod(){};
    virtual bool Init() = 0;
    virtual bool Error(char* pStr) = 0;
    virtual bool PrintfInfo(char*pStr) = 0;
};
