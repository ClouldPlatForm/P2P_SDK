#ifndef WriteLog_HHHHHHHHHHHHHHHHHHHHHHHHH
#define WriteLog_HHHHHHHHHHHHHHHHHHHHHHHHH
#include "../../CFile/CFile/FileFactory.h"
#include "../../ExecutePath/ExecutePath/PathFactory.h"
#include "../../TimeLib/TimeLib/TimeOperatorBase.h"
#include "../../CriticalSession/CriticalSession/CriticalSectionFactory.h"
 
#include "LogBase.h"
#include "../../../Global/Global.h"
 
class CMySelfLog : public CLogBase
{
public:
	CMySelfLog();
	~CMySelfLog();
	void Version();
    bool Init(bool bPrintDebugInfo, char *pDoucument);
    virtual bool InitErroInfoMethod(void* pLogErroInfoMethod);
	void WriteLogRecord(int LeveL,const char * ModuleStr,const char *format,...);	
	virtual bool CreateLogFile(const char* const fileName,int MAXSize, int File_Number) ;
	virtual bool WriteLogRecord(int LeveL,const char *Ptr) = 0; 
	void WriteLogRecord(int LeveL,char * pTotalBuf,const char *format, const char * cppFileName,int CodeLine,char *pFunctionName, ...);
	void debugPrint(const char * cppFileName, int CodeLine, const char *pFunctionName, char * pformatBuf, const char *format, ...);
private:
	CFileFactory m_FileFactory;
	CPathFactory m_PathFactory;
	CTimeOperatorBase m_TimeOperator;	
	bool m_bPrintDebug;
	CCriticalSectionFactory m_CSFac;
	char m_szDocument[100];
    CErrorInfo* m_pLogErroInfoMethod;
};
#endif
