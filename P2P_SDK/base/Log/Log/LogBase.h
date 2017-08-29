#pragma once

#include"../../../Global/Global.h"

enum LOG_LEVEL
{
	LEVEL_ERROR = 8,
	LEVEL_DEBUG = 4,
	LEVEL_WARN = 2,
	LEVEL_INFO = 1,
};
class CLogBase
{
	public:
		CLogBase();
		virtual ~CLogBase();
		virtual bool InitErroInfoMethod(void* pLogErroInfoMethod)  = 0;
		virtual bool CreateLogFile(const char* const fileName,int MAXSize, int File_Number) = 0;
		virtual bool WriteLogRecord(int LeveL,const char *Ptr) = 0; 
};