
#ifndef _INCLUDE_ROSOS_H_
#define _INCLUDE_ROSOS_H_
#include "aosReturnCode.h"

enum
{
	eAosRc_SetCoreFileSizeFailed = eAosRc_RosOSStart+1,
	eAosRc_GetCoreFileSizeFailed,
	eAosRc_GetProcessCoreFileProcNoExist,
	eAosRc_GetProcessCoreFileFailedSet,
};
int aos_set_core_size(int sz);
int aos_get_core_size(int *sz);
int aos_set_pid_core_size(int pid, int sz);
int aos_get_pid_core_size(int pid, int *sz);
#endif

