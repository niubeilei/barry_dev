#include "rosos.h"
#include "common.h"
#include <sys/times.h>
#include <sys/resource.h>

int aos_get_core_size(int *sz)
{
	struct rlimit rlimit;
	if ( 0==getrlimit(RLIMIT_CORE, &rlimit) )
		*sz = rlimit.rlim_cur;
	else
		return -eAosRc_SetCoreFileSizeFailed;
	return 0;
}

int aos_set_core_size(int sz)
{
	struct rlimit rlimit;
	int ret;

	rlimit.rlim_cur = rlimit.rlim_max = sz;
	ret = setrlimit(RLIMIT_CORE, &rlimit); 
	if (ret != 0)
		return -eAosRc_GetCoreFileSizeFailed;
	return 0;
}

int aos_set_pid_core_size(int pid, int sz)
{
    OmnString rslt = "";
    char* tmp;
    char cmdline[128];

    memset(cmdline, 0, sizeof(cmdline));
    sprintf(cmdline, "set process limit %d %d", pid, sz);

    OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);

    tmp = rslt.getBuffer();
    tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}


int aos_get_pid_core_size(int pid, int *sz)
{

    OmnString rslt = "";
    char* tmp;
    char cmdline[128];
	char show[512], line[128], *p;

    memset(cmdline, 0, sizeof(cmdline));
	memset(show, 0, sizeof(show));
	memset(line, 0, sizeof(line));
    sprintf(cmdline, "show process limit %d", pid);

    OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);

    tmp = rslt.getBuffer();
    tmp += rslt.find(0, false) + 1;

	strcpy(show, rslt.data());
	p = strtok(show, "\n");
	p = strtok(NULL, "\n");

	strcpy(line, p);
	sscanf(p, "%*s %*d %d", sz);

	return *((int*)tmp);
}


