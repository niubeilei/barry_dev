#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/limits.h>
#include <linux/security.h>
#include <linux/posix-timers.h>
#include "aos/aosKernelApi.h"
#include "aosUtil/StringUtil.h"
#include "aos/aosReturnCode.h"

enum
{
	eAosRc_SetCoreFileSizeFailed = eAosRc_RosOSStart+1,
	eAosRc_GetCoreFileSizeFailed,
	eAosRc_GetProcessCoreFileProcNoExist,
	eAosRc_GetProcessCoreFileFailedSet,
};

static int 
get_process_limit(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int process_id, index;
	struct task_struct *process;
	struct rlimit value;
	char *buff, tmp[512];
	index = 0;
	buff = aosKernelApi_getBuff(data);
	memset(tmp, 0, sizeof(tmp));

	process_id = parms->mIntegers[0];
	read_lock(&tasklist_lock);
	process = find_task_by_pid(process_id);
	if (!process) {
		snprintf(errmsg, errlen-1, "Usage: No this process,%d\n", process_id);
		errmsg[errlen-1] = 0;
		*length = 0;
		return -eAosRc_GetProcessCoreFileProcNoExist;
	}

	task_lock(process->group_leader);
	value = process->signal->rlim[4];
	task_unlock(process->group_leader);

	snprintf(tmp, sizeof(tmp), "Process Info:\t%8s\t\t%6s\t\t%6s\n             \t%8s\t\t%6d\t\t%6d\n", \
			"Name", "ID", "Core", process->comm, process_id, value.rlim_max);
	printk(KERN_ERR"%s", tmp);

	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));
	*length = index;

	read_unlock(&tasklist_lock);
	return 0;
}

static int
set_process_limit(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int process_id, retval; 
	struct task_struct *process;
	struct rlimit new_rlim, *old_rlim;
	*length = 0;

	process_id = parms->mIntegers[0];
	new_rlim.rlim_cur = new_rlim.rlim_max = parms->mIntegers[1];

	process = find_task_by_pid(process_id);
	if (!process) {
		snprintf(errmsg, errlen-1, "Usage: no this process which id is %d\n", process_id);
		errmsg[errlen-1] = 0;
		return -eAosRc_GetProcessCoreFileProcNoExist;
	}

	old_rlim = process->signal->rlim + 4;
	if ((new_rlim.rlim_max > old_rlim->rlim_max) &&
	    !capable(CAP_SYS_RESOURCE))
		return -eAosRc_GetProcessCoreFileFailedSet;

	retval = security_task_setrlimit(4, &new_rlim);
	if (retval)
		return -eAosRc_GetProcessCoreFileFailedSet;

	task_lock(process->group_leader);
	*old_rlim = new_rlim;
	task_unlock(process->group_leader);

	return 0;
}

int reg_setrlimit_cli(void)
{
	int ret = 0;
	ret |= OmnKernelApi_addCliCmd("set_process_limit", set_process_limit);
	ret |= OmnKernelApi_addCliCmd("show_process_limit", get_process_limit);

	return ret;
}

static int __init ros_os_init(void)
{
	if (reg_setrlimit_cli() < 0)
		printk(KERN_ERR "Some error occur\n");

	return 0;
}

static void __exit ros_os_cleanup(void)
{
	return;
}

module_init(ros_os_init);
module_exit(ros_os_cleanup);


