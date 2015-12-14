////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 12/02/2007	Moved from aosUtil/Alarm.cpp by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "alarm_c/alarm.h"

#include "alarm_c/alarm_util.h"
#include "KernelSimu/string.h"
#include "KernelSimu/aosKernelDebug.h"
#include "KernelSimu/kernel.h"
#include "util_c/modules.h"
#include "Porting/Sleep.h"
#include <stdarg.h>

#include "aosUtil/AosPlatform.h"

#ifdef __KERNEL__
#include <linux/module.h>
#endif

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <pthread.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/syscall.h>
static int sgFlag = -1;
void send_alarm_to_parent();

void AosAlarmBreakPoint(int pause_on_alarm)
{
	while (pause_on_alarm) 
	{ 
		printf("%s\n", "alarm sleep!");
		OmnSleep(5);                     
	} 

	if (sgFlag == 1)
	{
		send_alarm_to_parent();
	}
}

struct reqmsgbuf
{
	long mtype;     
	long mpid;
	long mttid;
};

struct respmsgbuf
{
	long mtype;  
	long mpid;
};

static int sgMsgid = -1;
static pthread_t sgThread;
static pthread_mutex_t sgLock;


void send_alarm_to_parent()
{
	pthread_mutex_lock(&sgLock);
	// send req
	struct reqmsgbuf msg = {0};
	msg.mtype = 1;
	msg.mpid = getpid();
	msg.mttid = syscall(SYS_gettid);

	size_t len = sizeof(long) + sizeof(long); 
	//msgsnd(sgMsgid, &msg, len, IPC_NOWAIT); 
	msgsnd(sgMsgid, &msg, len, 0); 

	//get resp
	len = sizeof(long);
	long msgtyp = getpid();
	while(1)
	{
		struct respmsgbuf respmsg = {0};
		//int size = msgrcv(sgMsgid, &respmsg, len, msgtyp, IPC_NOWAIT);
		int size = msgrcv(sgMsgid, &respmsg, len, msgtyp, 0);
		if (size > 0)
		{
			if (respmsg.mtype != msgtyp || respmsg.mpid != getpid())
			{
				perror("failed pid!!");
			}
			break;
		}
	}
	pthread_mutex_unlock(&sgLock);
}


static const char *sgAosAlarmErrHeader[eAosAlarmLevel_End+1] = 
{
	"",
	"********** Warn *********",
	"********** Alarm *********",
	"********** Fatal *********",
	""
};


int child_alarm_handler()
{
	size_t len = sizeof(long) + sizeof(long); 
	long msgtyp = 1;
	while(1)
	{
		// read req
		struct reqmsgbuf msg = {0};
		//int size = msgrcv(sgMsgid, &msg, len, msgtyp, IPC_NOWAIT);
		int size = msgrcv(sgMsgid, &msg, len, msgtyp, 0);
		if(size > 0)
		{
			if (msg.mtype != msgtyp)
			{
				perror("failed mtype!!");
			}

			long child_pid = msg.mpid;
			long child_threadid = msg.mttid;
			printf("========== Alarm =========  child_pid = %ld , thread_id = %ld \n", child_pid, child_threadid);

			AosAlarmBreakPoint(0);

			// send resp
			struct respmsgbuf respmsg = {0};
			respmsg.mtype = child_pid;
			respmsg.mpid = child_pid;

			size_t resp_len = sizeof(long); 
			//msgsnd(sgMsgid, &respmsg, resp_len, IPC_NOWAIT); 
			msgsnd(sgMsgid, &respmsg, resp_len, 0); 
		}
	}
}


int aos_alarm_init(int flag)
{
	//http://blog.csdn.net/pbymw8iwm/article/details/7971440
	//flag = 2;
	sgFlag = flag;
	if (flag == 0)
	{
		key_t key = getpid();
		sgMsgid = msgget(key, IPC_CREAT|0666);
		if (sgMsgid ==  -1)
		{
			perror("failed to init alarm fifo: ");
			exit(0);
		}
		
		int rslt = pthread_create(&sgThread, 
				NULL, (void* (*)(void *))child_alarm_handler,NULL);
		if (rslt != 0)
		{
			perror("failed to init alarm fifo: ");
			exit(0);
		}
	}
	else if(flag == 1)
	{
		key_t ppid = getppid();
		sgMsgid = msgget(ppid, IPC_CREAT|0666);
		if (sgMsgid ==  -1)
		{
			perror("failed to init alarm fifo: ");
			exit(0);
		}
	}
	return 0;
};


#ifdef __KERNEL__
EXPORT_SYMBOL(aos_alarm_get_errmsg);
#endif

// 
// The caller should never modify the contents of the buffer
// returned by this function.
//
char *aos_alarm_get_errmsg(const char *fmt, ...)
{
	static int  slBufIndex = 0;
	static char slBuffer[eAosAlarmNumLocalBuf][eAosAlarmLocalBufSize];
	unsigned int index = (slBufIndex++) & 0x07;

	va_list args;

	va_start(args, fmt);
	vsprintf(slBuffer[index], fmt, args);
	va_end(args);

	return slBuffer[index];
}


#ifdef __KERNEL__
EXPORT_SYMBOL(aos_raise_alarm);
#endif

int aos_raise_alarm(const char *file, 
					const int line, 
					int level, 
					int module, 
					int id,
					const char *msg)
{
	//raiseAlarmFromAssert(file, line, level, module, id, msg);
	
	char c = '\0';
	int changed = 0;
	char local[eAosAlarmLocalBufSize+101];

	if (level <= eAosAlarmLevel_Start || level >= eAosAlarmLevel_End)
	{
		level = eAosAlarmLevel_Alarm;
	}

	if (module <= eAosModule_Start || module >= eAosModule_End)
	{
		module = eAosModule_Start;
	}

	if (id <= eAosAlarmID_Start || id >= eAosAlarmID_End)
	{
		id = eAosAlarm_InvalidAlarmId;
	}

	if (strlen(msg) >= eAosAlarmLocalBufSize)
	{
		char *tmp = (char *)msg;
		changed = 1;
		c = tmp[eAosAlarmLocalBufSize];
		tmp[eAosAlarmLocalBufSize] = 0;
	}

	// sprintf(local, 
	// 	"%s"
	// 	"\nLocation: <%s:%d>"
	// 	"\nLevel: %d"
	// 	"\nModule: %s"
	// 	"\nAlarm ID: %d"
	// 	"\nError Message: %s"
	// 	"\n**********************\n", 
	// 	sgAosAlarmErrHeader[level], file, line, level, 
	//		aos_get_module_name(module), id, msg);
	sprintf(local, 
		"%s"
		"\nLocation: <%s:%d>"
		"\nLevel: %d"
		"\nModule: %s"
		"\nAlarm ID: %d"
		"\nError Message: %s"
		"\n**********************\n", 
		sgAosAlarmErrHeader[level], file, line, level, 
			"Module", id, msg);

	if (changed)
	{
		char *tmp = (char *)msg;
		tmp[eAosAlarmLocalBufSize] = c;
	}

	aos_printk(local);
	return -id;
}


