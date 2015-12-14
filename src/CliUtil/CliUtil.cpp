#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"

#include <stdarg.h>

int CliUtil_regCliCmd(char *id, aosCliFunc func)
{
	if (!OmnCliUtilProc::getSelf()->OmnUserLand_addCliCmd(id, func))
	{
		return -1;
	}
	return 0;
}

int CliUtil_initCli(char** names, int num, short port)
{
	if (!OmnCliUtilProc::getSelf()->getRelatedCmds(names, num))
		return -1;
	
	if (port == 0)
	{
		if (!OmnCliUtilProc::getSelf()->startListen(names[0]))
			return -1;
	}
	else {	
		if (!OmnCliUtilProc::getSelf()->startListen(port))
			return -1;
	}
	return 0;
}

int CliUtil_checkAndCopy(char *result, 
				   unsigned int *index, 
				   const unsigned int length, 
				   const char *buf, 
				   const unsigned int len)
{
	if (!result || !index || !buf || 
		len >= length - *index - 1 ||
		*index < 0 || *index >= length)
	{
		return -1;
	}

	strncpy(&result[*index], buf, len);
	*index += len;
	result[*index] = 0;
	return 0;
}

int CliUtil_sprintf(char *result, 
				   unsigned int *index, 
				   const unsigned int length, 
				   char* format, ...)
{
	if (!result || !index || *index < 0 || *index >= length)
		return -1;

	va_list ap;
	int len;
	
	va_start(ap, format);
	len = vsnprintf(&result[*index], length-*index-1, format, ap);
	va_end(ap);
	
	if (len < 0)
		return -1;

	*index += len;
	result[*index] = 0;
	return 0;
}

char *CliUtil_getBuff(char *data)
{
	return 	&data[4];
}



