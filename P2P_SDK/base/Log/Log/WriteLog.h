#ifndef WriteLog_HHHHHHHHHHHHHHHHHHHHHHHHH
#define WriteLog_HHHHHHHHHHHHHHHHHHHHHHHHH

//#include "Global.h"
#include "../../../Global/Global.h"
#include "LogBase.h"
#include "RecordLog.h"


class CWriteLog
{
public:
	CWriteLog();
	~CWriteLog();
	void Version();
    bool Init(bool bPrintDebugInfo, char *pDoucument);
    bool InitErroInfoMethod(void* pLogErroInfoMethod);
	void WriteLogRecord(int LeveL,const char * ModuleStr,const char *format,...);	
	void WriteLogRecord(int LeveL,char * pTotalBuf,const char *format, const char * cppFileName,int CodeLine,char *pFunctionName, ...);
	void debugPrint(const char * cppFileName, int CodeLine, const char *pFunctionName, char * pformatBuf, const char *format, ...);
	bool CreateLogFile(const char* const fileName,int MAXSize, int File_Number);
	bool RecordLogData(int LeveL, const char* const pInfo);

private:
	CErrorInfo *m_pLogErroInfoMethod;
	CLogBase *m_LogBase;
};
#endif
