
#include "MyselfLog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <string>
#include <fcntl.h>
#ifndef WIN
#include <unistd.h>
#endif

using namespace std;
#define MAX_FILE_SIZE (1024*1024 *10)
#include "Config.h"
#define PRGNAME "LogLib"
void CMySelfLog::Version()
{
	printf("%s:Version:%d.%d.%d\n", PRGNAME, MAR_VERSION, MIN_VERSION, RELEASE_VERSION);
}
CMySelfLog::CMySelfLog()
{
	m_bPrintDebug = false;
	memset(m_szDocument, 0, 100);
	
}
CMySelfLog::~CMySelfLog()
{

}
bool CMySelfLog::Init(bool bPrintDebugInfo, char *pDoucument)
{
	
	m_bPrintDebug = bPrintDebugInfo;
	if (!m_CSFac.CreateProduct())
	{
        m_pLogErroInfoMethod->Error("create Log CS failed\n");
		return false;
	}
    else
    {
        m_CSFac.m_pCS->InitErroInfoMethod((void*)m_pLogErroInfoMethod);
    }

    if (!m_FileFactory.CreateVersionFile())
	{
        m_pLogErroInfoMethod->Error("create file Factory Failed\n");
		return false;
	}
    else
    {
        m_FileFactory.m_pFileBase->InitErroInfoMethod((void*)m_pLogErroInfoMethod);
    }
	
	if (!m_PathFactory.CreateProduct())
	{
        m_pLogErroInfoMethod->Error("create Path Factory Failed\n");
		return false;
	}

	char szPath[255] = { 0 };
	char szFileName[100] = { 0 };
	m_PathFactory.m_pExePath->GetExecutePath(szPath);
	sprintf(szPath,"%s//%s//", szPath,pDoucument);
	sprintf(szFileName, "%d_%s_%d.log", getpid(),(char*)m_TimeOperator.GetCurTime().c_str(), m_TimeOperator.getCurTime());
	if (!m_FileFactory.m_pFileBase->SetFileName(szPath, szFileName))
	{
        m_pLogErroInfoMethod->Error("Create logFile failed\n");
		return false;
	}
	if (!m_FileFactory.m_pFileBase->OpenFile(O_RDWR|O_APPEND|O_CREAT))
	{
        m_pLogErroInfoMethod->Error("open logFile failed\n");
		return false;
	}
	return true;
}
void CMySelfLog::WriteLogRecord(int LeveL,const char * ModuleStr,const char *format,...)
{

	if (NULL == format || NULL == ModuleStr)
	{
        m_pLogErroInfoMethod->Error("Logstr or ModuleStr is NULL,in WriteLog fun\n");
		return;
	}
	string str;
	char src[1000] = { 0 };
	va_list argp;
	va_start(argp, format);
	vsnprintf(src,1000 -1, format, argp);	
	va_end(argp);	
	
	string strModule = "";
	switch (LeveL)
	{
	case LEVEL_ERROR:
	{
        strModule = string("LEVEL_ERROR:");
        break;
	}
	case LEVEL_DEBUG:
	{
        if (m_bPrintDebug)
        {
            strModule = string("LEVEL_Debug:");
        }
        else
        {
            return;
        }
        break;
	}
	case LEVEL_INFO:
	{
        strModule = string("LEVEL_INFO:");
        break;
	}
	case LEVEL_WARN:
	{
		strModule = string("LEVEL_WARN:");
		break;
	}
	default:
	{
		strModule = string("LEVEL_DEFAULT:");
	}
		break;
	}
	
	string strData = strModule + string(ModuleStr) + string(":") + string(src)+string("\r\n");
	m_CSFac.m_pCS->Lock();
	if (!m_FileFactory.m_pFileBase->WriteIntoFile((char*)(strData.c_str()), strData.size()))
	{
		m_CSFac.m_pCS->UnLock();
        m_pLogErroInfoMethod->Error("write file Failed\n");
		return;
	}
	m_CSFac.m_pCS->UnLock();
	unsigned int FileSize = 0;
	unsigned int & FileSizeName = FileSize;
	m_CSFac.m_pCS->Lock();
	m_FileFactory.m_pFileBase->GetFileSize(FileSizeName);
	m_CSFac.m_pCS->UnLock();
	if (FileSize >= MAX_FILE_SIZE)
	{ 
		m_CSFac.m_pCS->Lock();
		m_FileFactory.m_pFileBase->CloseFile();
		m_CSFac.m_pCS->UnLock();
		char szPath[255] = { 0 };
		char szFileName[50] = { 0 };
		m_PathFactory.m_pExePath->GetExecutePath(szPath);
		sprintf(szPath, "%s//%s//",m_szDocument, szPath);
		sprintf(szFileName, "%s_%d.log", (char*)m_TimeOperator.GetCurTime().c_str(), m_TimeOperator.getCurTime());
		m_CSFac.m_pCS->Lock();
		if (!m_FileFactory.m_pFileBase->SetFileName(szPath, szFileName))
		{
			m_CSFac.m_pCS->UnLock();
            m_pLogErroInfoMethod->Error("Create logFile failed\n");
			return ;
		}
		m_CSFac.m_pCS->UnLock();
		m_CSFac.m_pCS->Lock();
		if (!m_FileFactory.m_pFileBase->OpenFile(O_RDWR|O_CREAT|O_APPEND))
		{
			m_CSFac.m_pCS->UnLock();
            m_pLogErroInfoMethod->Error("open logFile failed\n");
			return ;
		}
		m_CSFac.m_pCS->UnLock();
	}
}

void CMySelfLog::WriteLogRecord(int LeveL, char * pTotalBuf, const char *format, \
	const char * cppFileName, 	int CodeLine, char *pFunctionName, ...)
{
	if (NULL == format)
	{
        m_pLogErroInfoMethod->Error("Logstr or ModuleStr is NULL,in WriteLog fun\n");
		return;
	}
	string str;
	
	va_list argp;
	va_start(argp, format);
	vsprintf(pTotalBuf, format, argp);
	va_end(argp);

	string strModule = "";
	switch (LeveL)
	{
	case LEVEL_ERROR:
	{
		strModule = string("LEVEL_ERROR:");
		break;
	}
	case LEVEL_DEBUG:
	{
		if (m_bPrintDebug)
		{
			strModule = string("LEVEL_Debug:");
		}
		else
		{
			return;
		}
		break;
	}
	case LEVEL_INFO:
	{
        strModule = string("LEVEL_INFO:");
        break;
	}
	case LEVEL_WARN:
	{
		strModule = string("LEVEL_WARN:");
		break;
	}
	default:
	{
		strModule = string("LEVEL_DEFAULT:");
	}
		break;
	}
	char szLine[20] = { 0 };
	sprintf(szLine, "%d", CodeLine);
	string strData = strModule + string(cppFileName) + string(":");
	strData = strData + string(szLine) + string(":");
	strData = strData + string(pFunctionName) + string(":");
	strData = strData + string(pTotalBuf) +string("\r\n");
	m_CSFac.m_pCS->Lock();
	if (!m_FileFactory.m_pFileBase->WriteIntoFile((char*)(strData.c_str()), strData.size()))
	{
		m_CSFac.m_pCS->UnLock();
        m_pLogErroInfoMethod->Error("write file Failed\n");
		return;
	}
	m_CSFac.m_pCS->UnLock();
	unsigned int FileSize = 0;
	unsigned int & FileSizeName = FileSize;
	m_CSFac.m_pCS->Lock();
	m_FileFactory.m_pFileBase->GetFileSize(FileSizeName);
	m_CSFac.m_pCS->UnLock();
	if (FileSize >= MAX_FILE_SIZE)
	{
		m_CSFac.m_pCS->Lock();
		m_FileFactory.m_pFileBase->CloseFile();
		m_CSFac.m_pCS->UnLock();
		char szPath[255] = { 0 };
		char szFileName[50] = { 0 };
		m_PathFactory.m_pExePath->GetExecutePath(szPath);
		sprintf(szPath, "%s//%s//", m_szDocument, szPath);
		sprintf(szFileName, "%s_%d.log", (char*)m_TimeOperator.GetCurTime().c_str(), m_TimeOperator.getCurTime());
		m_CSFac.m_pCS->Lock();
		if (!m_FileFactory.m_pFileBase->SetFileName(szPath, szFileName))
		{
			m_CSFac.m_pCS->UnLock();
            m_pLogErroInfoMethod->Error("Create logFile failed\n");
			return;
		}
		m_CSFac.m_pCS->UnLock();
		m_CSFac.m_pCS->Lock();
		if (!m_FileFactory.m_pFileBase->OpenFile(O_RDWR | O_CREAT | O_APPEND))
		{
			m_CSFac.m_pCS->UnLock();
            m_pLogErroInfoMethod->Error("open logFile failed\n");
			return;
		}
		m_CSFac.m_pCS->UnLock();
	}
}
void CMySelfLog::debugPrint(const char * cppFileName, int CodeLine, const char *pFunctionName, char * pformatBuf, const char *format, ...)
{
	if (m_bPrintDebug)
	{
		va_list argp;
		va_start(argp, format);
		vsprintf(pformatBuf, format, argp);
		va_end(argp);
		printf("%s:%d:%s:%s\r\n", cppFileName, CodeLine, pFunctionName, pformatBuf);
	}
}

bool CMySelfLog::InitErroInfoMethod(void* pLogErroInfoMethod)
{
    if (NULL == pLogErroInfoMethod)
    {
        return false;
    }
    m_pLogErroInfoMethod = (CErrorInfo*)pLogErroInfoMethod;
    return true;
}
bool CMySelfLog::WriteLogRecord(int LeveL,const char *Ptr)
{

}
