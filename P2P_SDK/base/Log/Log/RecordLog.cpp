#include "RecordLog.h"

// log4cpp Category  
static log4cpp::Category& g_root = log4cpp::Category::getRoot().getInstance("Record");

CRecordLog::CRecordLog() 
{
	//*m_root = g_root;
}

CRecordLog::~CRecordLog()
{

}
bool CRecordLog::InitErroInfoMethod(void* pLogErroInfoMethod) 
{	
	if(NULL == pLogErroInfoMethod)
	{
		ScreenPrintf("Log","m_errorInfo is NULL\r\n");
	}	
	m_errorInfo = (CErrorInfo*)pLogErroInfoMethod;
	return true;
}

bool CRecordLog::WriteLogRecord(int LeveL,const char *Ptr)
{ 
	switch(LeveL)
	{
		case LEVEL_DEBUG:
		{
			g_root.debug(string(Ptr));
			break;
		}
		case LEVEL_WARN:
		{
			g_root.warn(string(Ptr));
			break;
		}
		case LEVEL_INFO:
		{
			g_root.info(string(Ptr));
			break;
		}
		case LEVEL_ERROR:
		{
			g_root.error(string(Ptr));
			break;
		}
	} 
	 
	return true;
	
}


bool CRecordLog::CreateLogFile(const char* const fileName,int MAXSize, int File_Number)
{
	//注： log4cpp new创建的自带内存回收，不需要delete手动释放
	m_layout = new log4cpp::PatternLayout();
	m_layout->setConversionPattern("%d: %p %c %x: %m%n");
	//回滚文件
	m_FileAppender = new log4cpp::RollingFileAppender("rollfileAppender",string(fileName),MAXSize,File_Number);
	m_FileAppender->setLayout(m_layout);
	g_root.addAppender(m_FileAppender);
	g_root.setPriority(log4cpp::Priority::DEBUG);

	 
	return true;
}
