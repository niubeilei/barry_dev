////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: util.c
// Description:
//   
//
// Modification History:
// 2007-03-07 Created
////////////////////////////////////////////////////////////////////////////

#include "util.h"
#include "rhcUtil/aosResMgrToolkit.h"
#include "KernelInterface/CliSysCmd.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "aosUtil/Alarm.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/utsname.h>

// Linux programmer
#include <time.h>
#include <unistd.h>

#define MAXCOUNT 6

void reset_WanCM_App(struct WanCM_App * const ptrApp)
{
	ptrApp->m_dip[0] = 0;
	ptrApp->m_interval = 1;
	
	// something about thread
	ptrApp->m_switch_on = FALSE;
}

// Declare global variables
struct WanCM_App g_theWanCMApp;

// print error and exit the program
void usage(const char *zErrmsg, const char *program_name)
{
	printf ("%s\nTry \"%s -h\" for more information.\n",
			zErrmsg, program_name);
}

BOOL wanCM_start_thread()
{
	strncpy(g_theWanCMApp.m_thread_input.param1, g_theWanCMApp.m_dip, MAX_IP_ADDR);
	sprintf(g_theWanCMApp.m_thread_input.param2, "%d", g_theWanCMApp.m_interval);
	// create the thread 
	aos_uland_thread_create(&g_theWanCMApp.m_thread, wanCM_thread_func, 
							&g_theWanCMApp.m_thread_input, &g_theWanCMApp.m_mutex);
	return TRUE;
}

BOOL wanCM_kill_thread()
{
	// if the old thread is not running, return 
	if(!g_theWanCMApp.m_switch_on)
	{
		return TRUE;
	}

	// kill old thread
	struct timespec ts;
	pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	g_theWanCMApp.m_switch_on = FALSE;
	pthread_mutex_unlock (&g_theWanCMApp.m_mutex);

	{
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += (RESMGR_EXPIRED_TIME_DEFAULT);


		pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	//	pthread_cond_wait( &g_theWanCMApp.m_thread_stopped, &g_theWanCMApp.m_mutex);
		pthread_cond_timedwait( &g_theWanCMApp.m_thread_stopped, &g_theWanCMApp.m_mutex, &ts);
		pthread_mutex_unlock (&g_theWanCMApp.m_mutex);
	}
	return TRUE;
}


//int main(int argc, char **argv)
void wanCM_thread_func(void * ptr)
{
	int ret;
	int interval;
	//int i;
	char dip[MAX_IP_ADDR];
	char *zErrmsg = NULL;

	// 1 validate the input
	if(0 != getuid())
	{
		zErrmsg = "Error: Authority not enough.";
		usage(zErrmsg, "WanCM");
		return ;
	}

	strncpy(dip, ((struct wanCM_thread_struct*) ptr)->param1, MAX_IP_ADDR);
	interval = atoi(((struct wanCM_thread_struct*) ptr)->param2);
	if(interval <= 0)
	{
		zErrmsg = "Error: Illegal input.";
		usage(zErrmsg, "WanCM");
		return ;
	}

	if(interval > MAX_TIME_INTERVAL)
	{
		zErrmsg = (char *) malloc(sizeof(char)*100);  // malloc(40);
		usage(zErrmsg, "WanCM");
		if(zErrmsg)
		{
			free(zErrmsg);
			zErrmsg = NULL;
		}
		return ;
	}

	// 2 if the old thread is running 
	if(g_theWanCMApp.m_switch_on)
	{
		zErrmsg = (char *) malloc(sizeof(char)*256);
		snprintf(zErrmsg, 256, "Error: system failure. The old wanCM thread is running.");
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		if(zErrmsg)
		{
			free(zErrmsg);
			zErrmsg = NULL;
		}
		return ;
	}
	pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	g_theWanCMApp.m_switch_on = TRUE;
	pthread_mutex_unlock (&g_theWanCMApp.m_mutex);

	// 3 Get in the real loop
	while (1)
	{
		ret = traceloop(dip);
		if(ret<0)
		{
			printf("error: %d\n", __LINE__);
			printf("network is bad");
			for (int i = 0; i < MAXCOUNT; i++) {
				if (g_theWanCMApp.m_switch_on == FALSE)
				{
					printf("\n Disconnect network success!");
					return;
				
				} 
				else if(g_theWanCMApp.m_switch_on == TRUE) 
				{ // do some thing to bring up the WAN
					ret = traceloop(dip);
					if (ret >= 0)
						break;
						
					if (g_theWanCMApp.m_sgWan.method == "static")
					{
						wanStatic(g_theWanCMApp.m_sgWan.ip, g_theWanCMApp.m_sgWan.netmask);		
					}		
					else if (g_theWanCMApp.m_sgWan.method == "dhcp")
					{
						wanDhcp();	
					}
					else if (g_theWanCMApp.m_sgWan.method == "PPPoE")
					{
						wanPppoe(g_theWanCMApp.m_sgWan.ad_user, g_theWanCMApp.m_sgWan.ad_passwd, g_theWanCMApp.m_sgWan.dns1, g_theWanCMApp.m_sgWan.dns2);	
					}
				}
			}
		}
		else
		{
			printf("\nNetwork is good\n");
			//network work
			
			if (g_theWanCMApp.m_switch_on == FALSE)
			{
				for (int j = 0; j < MAXCOUNT; j++) 
				{
					ret = traceloop(dip);
					if (ret < 0)
						return;
					else
						wanStatusOff();
				}
				return;
			}
		}
		sleep(interval * 60);
    }
	// 6 Leave the current thread.
	{
		pthread_mutex_lock (&g_theWanCMApp.m_mutex);
		pthread_cond_signal(&g_theWanCMApp.m_thread_stopped);
		pthread_mutex_unlock (&g_theWanCMApp.m_mutex);
	}

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Tool functions for WAN operation
/////////////////////////////////////////////////////////////////////////////////////////

int wanInit()
{
	pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	g_theWanCMApp.m_sgWan.ad_user = "";
	g_theWanCMApp.m_sgWan.ad_passwd = "";
	g_theWanCMApp.m_sgWan.dns1 = "";
	g_theWanCMApp.m_sgWan.dns2 = "";
	g_theWanCMApp.m_sgWan.ip = "";
	g_theWanCMApp.m_dip[0] = 0;
	g_theWanCMApp.m_sgWan.netmask = "";
	g_theWanCMApp.m_sgWan.method = "static";
	g_theWanCMApp.m_sgWan.hc_time = 1;
	g_theWanCMApp.m_interval = 1;
	g_theWanCMApp.m_sgWan.hc_ip = "";
	g_theWanCMApp.m_sgWan.status = "off";
	g_theWanCMApp.m_switch_on = FALSE;
	pthread_mutex_unlock (&g_theWanCMApp.m_mutex);

	return 0;
}

int wanPppoe(OmnString &username, OmnString &password, OmnString &dns1, OmnString &dns2)
{
	OmnString rslt;

	bool changedFlag1 = false;
    bool changedFlag2 = false;
    bool changedFlag3 = false;
    bool changedFlag4 = false;
	bool isFileFinished1 = false;
	bool isFileFinished2 = false;

	OmnString stopPppoeCmd;
	stopPppoeCmd << "/usr/sbin/pppoe-stop >/dev/null 2>&1";
	OmnCliSysCmd::doShell(stopPppoeCmd, rslt);

	OmnFilePtr pppoeFile1 = OmnNew OmnFile("../rhcConf/pppoe.conf.template",OmnFile::eReadOnly);
	OmnFilePtr pppoeFile2 = OmnNew OmnFile("../rhcConf/pap-secrets.template",OmnFile::eReadOnly);
	OmnFilePtr tmpFile1 = OmnNew OmnFile("../rhcConf/pppoe.conf.tmp",OmnFile::eCreate);
	OmnFilePtr tmpFile2 = OmnNew OmnFile("../rhcConf/pap-secrets.tmp",OmnFile::eCreate);

	if (!pppoeFile1)
	{
	/*	rslt = "Failed to open configure file: ";
		rslt << "pppoe.conf.template";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
	*/
		return eAosAlarm_FailedToOpenFile;
	}

	if (!pppoeFile1->isGood())
	{
	/*	rslt = "Failed to open configure file: ";
		rslt << "pppoe.conf.template";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
     */
		return eAosAlarm_FailedToOpenFile;
	}
	
    if (!tmpFile1)
	{
	/*	rslt = "Failed to open configure file: ";
		rslt << "pppoe.conf.tmp";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
	*/
		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpFile1->isGood())
	{
	/*	rslt = "Failed to open configure file: ";
		rslt << "pppoe.conf.tmp";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		*/
		return eAosAlarm_FailedToOpenFile;
	}
	
	if (!pppoeFile2)
	{
		/*rslt = "Failed to open configure file: ";
		rslt << "pap-secrets.template";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		*/

		return eAosAlarm_FailedToOpenFile;
	}

	if (!pppoeFile2->isGood())
	{
		/*rslt = "Failed to open configure file: ";
		rslt << "pap-secrets.template";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		*/
		return eAosAlarm_FailedToOpenFile;
	}
	
    if (!tmpFile2)
	{
		/*rslt = "Failed to open configure file: ";
		rslt << "pap-secrets.tmp";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		*/
		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpFile2->isGood())
	{
		/*rslt = "Failed to open configure file: ";
		rslt << "pap-secrets.tmp";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		*/
		return eAosAlarm_FailedToOpenFile;
	}
    
    OmnString line1 = pppoeFile1->getLine(isFileFinished1);
	while(!isFileFinished1)
	{
		OmnString word1;
		int curPos1 = 0;
		curPos1 = line1.getWord(curPos1,word1);
		OmnString prefix1(word1.getBuffer(),5);
    	if(changedFlag1 == false && prefix1 == "USER=")
	   	{
			OmnString  newLine = "USER=";
			newLine << "\'" << g_theWanCMApp.m_sgWan.ad_user << "\'\n";
		 	tmpFile1->append(newLine);
			changedFlag1 = true;
		}
        else if(changedFlag2 == false && prefix1 == "DNS1=")
	   	{
			OmnString  newLine = "DNS1=";
			newLine << g_theWanCMApp.m_sgWan.dns1 << "\n";
		 	tmpFile1->append(newLine);
			changedFlag2 = true;
		}
        else if(changedFlag3 == false && prefix1 == "DNS2=")
		{
			OmnString  newLine = "DNS2=";
			newLine << g_theWanCMApp.m_sgWan.dns2 << "\n";
		 	tmpFile1->append(newLine);
			changedFlag3 = true;
		}
        else 
		{
			line1 << "\n";
			tmpFile1->append(line1);
		}
		line1 = pppoeFile1->getLine(isFileFinished1);
	}
     
    OmnString line2 = pppoeFile2->getLine(isFileFinished2);
	while(!isFileFinished2)
	{
		
		OmnString word1;
		OmnString word2;
		OmnString word3;
		int curPos1 = 0;
		curPos1 = line2.getWord(curPos1,word1);
    	if(changedFlag4 == false &&  word1 == "#")
	   	{
			curPos1 = line2.getWord(curPos1,word2);
			curPos1 = line2.getWord(curPos1,word3);
			if(word2 == "*" && word3 == "password")
			{
				line2 << "\n";
		 		tmpFile2->append(line2);

				line2 = "";
				OmnString word4;
				line2 = pppoeFile2->getLine(isFileFinished2);
				while(!isFileFinished2)
				{
					int curPos2 = 0;
					curPos2=line2.getWord(curPos2,word4);
					if(!(word4 == ""))
					{
						OmnString  newLine = "\"";
						newLine << g_theWanCMApp.m_sgWan.ad_user << "\"\t" << "*\t\"" << g_theWanCMApp.m_sgWan.ad_passwd << "\"\n";
//	cout << newLine << endl;
						tmpFile2->append(newLine);
						changedFlag4 = true;
						break;
					}
					line2 << "\n";
		 			tmpFile2->append(line2);
					line2 = pppoeFile2->getLine(isFileFinished2);
				}
			}
			else
			{
				line2 << "\n";
		 		tmpFile2->append(line2);
			}
		}
        else
		{
			line2 << "\n";
			tmpFile2->append(line2);
		}
		line2 = pppoeFile2->getLine(isFileFinished2);
	}
     
	if(changedFlag1 == false || changedFlag2 == false || changedFlag4 == false )
	{
		// it's an error, send an alarm, then return false;
		// cout << "Not change the config!";
		return 0;
				
	}

	pppoeFile1->closeFile();
	pppoeFile2->closeFile();
	tmpFile1->closeFile();
	tmpFile2->closeFile();
// cp ip-up to /etc/ppp/for modify the default route 	
    OmnString renameCmd;
	renameCmd << "/bin/mv ../rhcConf/pppoe.conf.tmp /etc/ppp/pppoe.conf && /bin/mv ../rhcConf/pap-secrets.tmp  /etc/ppp/pap-secrets && /bin/cp -f ../rhcConf/ip-up.template /etc/ppp/ip-up";
	OmnCliSysCmd::doShell(renameCmd, rslt);
    OmnString systemCmd;
	systemCmd << "/usr/sbin/pppoe-start >/dev/null 2>&1";
	OmnCliSysCmd::doShell(systemCmd, rslt);


	return 0;
}

int wanStatic(OmnString &ip, OmnString &netmask)
{
	OmnString cmd;
	OmnString rslt;
	cmd << "/sbin/ifconfig eth0 " << ip << " netmask " << netmask << " up";

	OmnCliSysCmd::doShell(cmd, rslt);
	return 0;
}

int wanDhcp()
{
	OmnString cmd;
	OmnString rslt;
	cmd << "/sbin/dhclient " << "eth0";

	OmnCliSysCmd::doShell(cmd,rslt);

	return 0;
}

int wanStatusOff()
{
	pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	g_theWanCMApp.m_switch_on = FALSE;
	pthread_mutex_unlock (&g_theWanCMApp.m_mutex);
	OmnString offCmd;
	OmnString rslt;
	offCmd << "/sbin/ifconfig eth0 0";
	OmnCliSysCmd::doShell(offCmd, rslt);
	OmnString cmd;
	cmd << "/usr/sbin/pppoe-stop >/dev/null 2>&1";
	OmnCliSysCmd::doShell(cmd, rslt);

		return 0;
}
