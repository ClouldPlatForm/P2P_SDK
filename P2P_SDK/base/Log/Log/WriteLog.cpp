
#include "WriteLog.h"
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
class log4cpp::Category;
#define MAX_FILE_SIZE (1024*1024 *10)
#include "Config.h"
#define PRGNAME "LogLib"
void CWriteLog::Version()
{
	printf("%s:Version:%d.%d.%d\n", PRGNAME, MAR_VERSION, MIN_VERSION, RELEASE_VERSION);
}
CWriteLog::CWriteLog()
{
	 
}
CWriteLog::~CWriteLog()
{

}
bool CWriteLog::Init(bool bPrintDebugInfo, char *pDoucument)
{
	return true;
}
void CWriteLog::WriteLogRecord(int LeveL,const char * ModuleStr,const char *format,...)
{
#ifdef LOG4CPP 
		char src[500] = { 0 };
		va_list argp;
		va_start(argp, format);
		vsnprintf(src,500-1, format, argp);	
		va_end(argp);	
		char buffer[500] = { 0 };
		snprintf(buffer,sizeof(buffer), "%s %s", ModuleStr,src);
	    m_LogBase->WriteLogRecord(LeveL,buffer);
#endif
 
}

void CWriteLog::WriteLogRecord(int LeveL, char * pTotalBuf, const char *format, \
	const char * cppFileName, 	int CodeLine, char *pFunctionName, ...)
{
#ifdef LOG4CPP 
		va_list argp;
		va_start(argp, format);
		vsprintf(pTotalBuf, format, argp);
		va_end(argp);
		char buffer[500] = { 0 };
		snprintf(buffer,sizeof(buffer), "%s %d %s %s", cppFileName, CodeLine, pFunctionName, pTotalBuf);
	    m_LogBase->WriteLogRecord(LeveL,buffer);
#endif
}
void CWriteLog::debugPrint(const char * cppFileName, int CodeLine, const char *pFunctionName, char * pformatBuf, const char *format, ...)
{
#ifdef LOG4CPP 
		va_list argp;
		va_start(argp, format);
		vsprintf(pformatBuf, format, argp);
		va_end(argp);
		char buffer[500] = { 0 };
		snprintf(buffer,sizeof(buffer), "%s %d %s %s", cppFileName, CodeLine, pFunctionName, pformatBuf);
	    m_LogBase->WriteLogRecord(LEVEL_INFO,buffer);
#endif
}

bool CWriteLog::InitErroInfoMethod(void* pLogErroInfoMethod)
{
#ifdef LOG4CPP 
	m_LogBase = new CRecordLog();
#endif

    if (NULL == pLogErroInfoMethod)
    {
        return false;
    }
    m_pLogErroInfoMethod = (CErrorInfo*)pLogErroInfoMethod;
    m_LogBase->InitErroInfoMethod(pLogErroInfoMethod);

    return true;
}
bool CWriteLog::CreateLogFile(const char* const fileName,int MAXSize, int File_Number)
{
	m_LogBase->CreateLogFile(fileName,MAXSize,File_Number);
}

bool CWriteLog::RecordLogData(int LeveL, const char* const pInfo)
{
	m_LogBase->WriteLogRecord(LeveL,pInfo);
}

/*
int main()
{
	CWriteLog log;
	//log.InitLog();
	log.InitErroInfoMethod(NULL);

	log.CreateLogFile("test.log",100,10);
	log.RecordLogData(LEVEL_DEBUG,"logFile, debug ......................dsdsfsdf sdfsdfdsfgdfgdfgdfgdgdfgvrdfgvdfgvdrgfdgbfdgbfd");
	log.RecordLogData(LEVEL_DEBUG,"logFile, debug ......................dsdsfsdf sdfsdfdsfgdfgdfgdfgdgdfgvrdfgvdfgvdrgfdgbfdgbfd");
	log.RecordLogData(LEVEL_DEBUG,"logFile, debug ......................dsdsfsdf sdfsdfdsfgdfgdfgdfgdgdfgvrdfgvdfgvdrgfdgbfdgbfd");
	log.RecordLogData(LEVEL_DEBUG,"logFile, debug ......................dsdsfsdf sdfsdfdsfgdfgdfgdfgdgdfgvrdfgvdfgvdrgfdgbfdgbfd");
	log.RecordLogData(LEVEL_DEBUG,"logFile, debug ......................dsdsfsdf sdfsdfdsfgdfgdfgdfgdgdfgvrdfgvdfgvdrgfdgbfdgbfd");
	log.RecordLogData(LEVEL_DEBUG,"logFile, debug ......................dsdsfsdf sdfsdfdsfgdfgdfgdfgdgdfgvrdfgvdfgvdrgfdgbfdgbfd");
	log.RecordLogData(LEVEL_ERROR,"logFile, debug ......................dsdsfsdf sdfsdfdsfgdfgdfgdfgdgdfgvrdfgvdfgvdrgfdgbfdgbfd");
	log.RecordLogData(LEVEL_ERROR,"logFile, debug ......................dsdsfsdf sdfsdfdsfgdfgdfgdfgdgdfgvrdfgvdfgvdrgfdgbfdgbfd");
	log.RecordLogData(LEVEL_ERROR,"logFile, debug ......................dsdsfsdf sdfsdfdsfgdfgdfgdfgdgdfgvrdfgvdfgvdrgfdgbfdgbfd");
	log.RecordLogData(LEVEL_ERROR,"logFile, debug ......................dsdsfsdf sdfsdfdsfgdfgdfgdfgdgdfgvrdfgvdfgvdrgfdgbfdgbfd");
	log.RecordLogData(LEVEL_ERROR,"logFile, debug ......................dsdsfsdf sdfsdfdsfgdfgdfgdfgdgdfgvrdfgvdfgvdrgfdgbfdgbfd");
	log.RecordLogData(LEVEL_ERROR,"logFile, debug ......................dsdsfsdf sdfsdfdsfgdfgdfgdfgdgdfgvrdfgvdfgvdrgfdgbfdgbfd");

	return 0;

} */

