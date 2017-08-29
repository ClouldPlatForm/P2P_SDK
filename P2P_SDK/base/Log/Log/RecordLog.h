 #pragma once

#include "LogBase.h"
#include <iostream>
#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/RollingFileAppender.hh>
using namespace std;

class CRecordLog : public CLogBase
{

public:
	CRecordLog();
	~CRecordLog();
	virtual bool InitErroInfoMethod(void* pLogErroInfoMethod) ;
	virtual bool WriteLogRecord(int LeveL,const char *Ptr);	
	virtual bool CreateLogFile(const char* const fileName,int MAXSize, int File_Number);
 
	

	
private:
	CErrorInfo *m_errorInfo;
	log4cpp::PatternLayout* m_layout ;
	log4cpp::RollingFileAppender* m_FileAppender;
	log4cpp::Appender* m_RemoteSyslogAppender ;
	
};