///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemWan.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Global.h"
#include "wanCM/util.h"

#include <sys/file.h>
#include "KernelInterface/CliSysCmd.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"
#include "Util/File.h"
#include "XmlParser/XmlItem.h"
#include "Tracer/Tracer.h"
#include "aosUtil/Alarm.h"
#include "Util/OmnNew.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"

#define RHC_WAN_CONFIG "/usr/local/rhc/config/wan.conf"
#define ERRMSG(errmsg, rslt, errlen) \
	strncpy(errmsg, rslt.data(), errlen-1); \
errmsg[errlen-1] = 0

int wanIpAddressSet(u32 ip, u32 mask, OmnString rslt)
{
	char ipStr[MAX_IP_ADDR_LEN];
	char maskStr[MAX_IP_ADDR_LEN];
	OmnString systemCmd;
	struct in_addr tmpaddr;

	tmpaddr.s_addr = ip;
	strcpy(ipStr, inet_ntoa(tmpaddr));
	tmpaddr.s_addr = mask;
	strcpy(maskStr, inet_ntoa(tmpaddr));
		
	pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	g_theWanCMApp.m_sgWan.ip = ipStr;
	g_theWanCMApp.m_sgWan.netmask = maskStr;
	pthread_mutex_unlock (&g_theWanCMApp.m_mutex);

	systemCmd << "/sbin/ifconfig eth0 " << ipStr << " " << maskStr;            
	cout << systemCmd << endl;
	OmnCliSysCmd::doShell(systemCmd, rslt);

	return 0;

}


int wanIpAddressSet(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	char local[1024];
	int ret;

	u32 ip, mask;

	char* buf = CliUtil_getBuff(data);

	ip = parms->mIntegers[0];
	mask = parms->mIntegers[1];

	ret = wanIpAddressSet(ip, mask, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}

	return ret;
}


int wanIpAddressShow(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString systemCmd;
	OmnString rslt;
	char local[2048];

	systemCmd << "/sbin/ifconfig eth0";       
	OmnCliSysCmd::doShell(systemCmd, rslt);

	strcpy(local, rslt.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;
	return 0;
}

int wanConnectMethodSet(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	//char* buf = CliUtil_getBuff(data);

	pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	g_theWanCMApp.m_sgWan.method =  parms->mStrings[0];
	pthread_mutex_unlock (&g_theWanCMApp.m_mutex);

	//static,dhcp, adsl
	*optlen = index;
	return 0;
}

int wanGetIp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString systemCmd;
	OmnString rslt;

	systemCmd << "/sbin/ifconfig";       
	OmnCliSysCmd::doShell(systemCmd, rslt);

	CliUtil_checkAndCopy(buf, &index, *optlen, rslt.data(), strlen(rslt.data()));

	*optlen = index;
	return 0;
}					

int wanHealthCheckSetTime(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	//char* buf = CliUtil_getBuff(data);
	int i;
	OmnString rslt;
	i = parms->mIntegers[0];
	if ( (i <= 0) || (i >59))
	{
		rslt = "Interval is between 1 to 59 ";
		ERRMSG(errmsg, rslt, errlen);
		return -eAosRc_InvalidFreq;
	}
	pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	
	g_theWanCMApp.m_sgWan.hc_time = i; 
	g_theWanCMApp.m_interval =  parms->mIntegers[0];
	pthread_mutex_unlock (&g_theWanCMApp.m_mutex);

	*optlen = index;
	return 0;
}

int wanAdslUserSet(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	//char* buf = CliUtil_getBuff(data);

	pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	g_theWanCMApp.m_sgWan.ad_user =  parms->mStrings[0];
	pthread_mutex_unlock (&g_theWanCMApp.m_mutex);

	*optlen = index;
	return 0;
}

int wanAdslPasswdSet(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	//char* buf = CliUtil_getBuff(data);

	pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	g_theWanCMApp.m_sgWan.ad_passwd =  parms->mStrings[0];
	pthread_mutex_unlock (&g_theWanCMApp.m_mutex);

	*optlen = index;
	return 0;
}

int wanDnsSet(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	int dns1, dns2;
	//char* buf = CliUtil_getBuff(data);
	char dns1Str[MAX_IP_ADDR_LEN];
	char dns2Str[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;

	dns1 = parms->mIntegers[0];
	dns2 = parms->mIntegers[1];
	tmpaddr.s_addr = dns1;
	strcpy(dns1Str, inet_ntoa(tmpaddr));
	tmpaddr.s_addr = dns2;
	strcpy(dns2Str, inet_ntoa(tmpaddr));
		
	pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	g_theWanCMApp.m_sgWan.dns1 = dns1Str;
	if( dns2Str == "")
		g_theWanCMApp.m_sgWan.dns2 = "";
	else
		g_theWanCMApp.m_sgWan.dns2 =  dns2Str;
	pthread_mutex_unlock (&g_theWanCMApp.m_mutex);

	*optlen = index;
	return 0;
}

int wanHealthCheckSetIp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	//char* buf = CliUtil_getBuff(data);
	int ip;
	char ipStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;

	ip = parms->mIntegers[0];
	tmpaddr.s_addr = ip;
	strcpy(ipStr, inet_ntoa(tmpaddr));
		
	pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	g_theWanCMApp.m_sgWan.hc_ip = ipStr;
	strncpy(g_theWanCMApp.m_dip, ipStr, MAX_IP_ADDR);
	pthread_mutex_unlock (&g_theWanCMApp.m_mutex);

	*optlen = index;
	return 0;
}

int wanShowConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	char local[2048];
	OmnString contents;

	contents  << "WAN Connection Manager Information\n" 
		<< "----------------------------------\n" 
		<< "              Status : " << g_theWanCMApp.m_sgWan.status    << "\n" 
		<< "              Method : " << g_theWanCMApp.m_sgWan.method    << "\n"
		<< "           ADSL_User : " << g_theWanCMApp.m_sgWan.ad_user   << "\n"
		<< "         ADSL_Passwd : " << g_theWanCMApp.m_sgWan.ad_passwd << "\n"
		<< "                DNS1 : " << g_theWanCMApp.m_sgWan.dns1      << "\n"
		<< "                DNS2 : " << g_theWanCMApp.m_sgWan.dns2      << "\n"
		<< "             IP_Addr : " << g_theWanCMApp.m_sgWan.ip        << "\n"
		<< "             IP_Mask : " << g_theWanCMApp.m_sgWan.netmask   << "\n"
		<< "Health Check Time(m) : " << g_theWanCMApp.m_sgWan.hc_time   << "\n"
										<< "     Health Check IP : " << g_theWanCMApp.m_sgWan.hc_ip     << "\n";

	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;
	return 0;
}

int wanClearConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	//char* buf = CliUtil_getBuff(data);

	wanInit();

	*optlen = index;
	return 0;
}

int wanSaveConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	/*OmnString rslt, fn;
	OmnFilePtr mFile;

	fn = RHC_WAN_CONFIG;

	if (!mFile)
	{
		OmnString tfn = fn;
		tfn << ".active";
		mFile = OmnNew OmnFile(tfn, OmnFile::eCreate);
		if (!mFile || !mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << tfn;
			return aos_alarm(eAosMD_Platform, eAosAlarm_FailedToOpenFile, 
					"%s", rslt.data());
		}
	}


	//unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	//char dns[20];
	//char local[256];

	OmnString contents;
	int ret;

	contents << "<Cmd>wan adsl username set " << g_theWanCMApp.m_sgWan.ad_user << "</Cmd>\n";
	contents << "<Cmd>wan adsl password set " << g_theWanCMApp.m_sgWan.ad_passwd << "</Cmd>\n";
	contents << "<Cmd>wan dns set " << g_theWanCMApp.m_sgWan.dns1 << " " << g_theWanCMApp.m_sgWan.dns2 << "</Cmd>\n";
	contents << "<Cmd>wan ip address set " << g_theWanCMApp.m_sgWan.ip << " " << g_theWanCMApp.m_sgWan.netmask << "</Cmd>\n";
	contents << "<Cmd>wan connect method set " << g_theWanCMApp.m_sgWan.method << "</Cmd>\n";
	contents << "<Cmd>wan health check set time " << g_theWanCMApp.m_sgWan.hc_time << "</Cmd>\n";
	contents << "<Cmd>wan health check set ip " << g_theWanCMApp.m_sgWan.hc_ip << "</Cmd>\n";
	contents << "<Cmd>wan status set " << g_theWanCMApp.m_sgWan.status << "</Cmd>\n";

	char local[1000];
	sprintf(local, "------AosCliBlock: wan------\n");
	if (!mFile->put(local, false) ||
			!mFile->put("<Commands>\n", false) ||
			!mFile->put(contents, false) ||
			!mFile->put("</Commands>\n", false) ||
			!mFile->put("------EndAosCliBlock------\n\n", false))
	{
		rslt = "Failed to write to config: ";
		rslt << fn;
		ret = aos_alarm(eAosMD_Platform, eAosAlarm_FailedToWriteConfig,
				"%s", rslt.data());
	}

	// 
	// Back up the current config
	//
	OmnFile file(fn, OmnFile::eReadOnly);
	OmnString cmd;
	if (file.isGood())
	{
		OmnString newFn = fn;
		newFn << ".bak";
		cmd << "cp " << fn << " " << newFn;
		//OmnTrace << "To run command: " << cmd << endl;
		system(cmd);
	}

	mFile->closeFile();
	mFile = 0;

	cmd = "mv -f ";

	cmd << fn << ".active " << fn;
	system(cmd);
	//OmnTrace << "To run command: " << cmd << endl;

	strcpy(local, "Wan's config save ok!");
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
*/
	char* buf = CliUtil_getBuff(data);
	//char dns[20];
	char local[2048];
	OmnString contents;
	
	 if (g_theWanCMApp.m_sgWan.ip != "")
	 {
	 	contents<< "<Cmd>";
	 	contents << "wan ip address set " << g_theWanCMApp.m_sgWan.ip << " " << g_theWanCMApp.m_sgWan.netmask;
	 	contents << "</Cmd>\n";
	 }  
	 if (g_theWanCMApp.m_sgWan.ad_user != "")
	 {
	 	contents<< "<Cmd>";
	 	contents << "wan adsl username set" << g_theWanCMApp.m_sgWan.ad_user;
	 	contents << "</Cmd>\n";
	 }
	 if (g_theWanCMApp.m_sgWan.ad_passwd != "")
	 {
	 	contents<< "<Cmd>";
	 	contents << "wan adsl password set " << g_theWanCMApp.m_sgWan.ad_passwd;
	 	contents << "</Cmd>\n";
	 }
	 if ((g_theWanCMApp.m_sgWan.dns1 != "") || (g_theWanCMApp.m_sgWan.dns2 != ""))
	 {
	 	contents << "<Cmd>";
	 	contents << "wan dns set " << g_theWanCMApp.m_sgWan.dns1 << " " << g_theWanCMApp.m_sgWan.dns2;
	 	contents << "</Cmd>\n";
	 }
	 
	if (g_theWanCMApp.m_sgWan.hc_ip != "")
	 {
	 	contents << "<Cmd>";
	 	contents << "wan health check set ip " << g_theWanCMApp.m_sgWan.hc_ip;
	 	contents << "</Cmd>\n";
	 }
	 contents << "<Cmd>";
	 contents << "wan health check set time " << g_theWanCMApp.m_sgWan.hc_time;
	 contents << "</Cmd>\n";
	 contents << "<Cmd>";
	 contents << "wan connect method set " << g_theWanCMApp.m_sgWan.method;
	 contents << "</Cmd>\n";
	 contents << "<Cmd>";
	 contents << "wan set status " << g_theWanCMApp.m_sgWan.status;
	 contents << "</Cmd>\n";
		
	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;

	return 0;
}

// "wan set status [on|off]"
int wanSetStatus(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	pthread_mutex_lock (&g_theWanCMApp.m_mutex);
	g_theWanCMApp.m_sgWan.status = parms->mStrings[0];
	pthread_mutex_unlock (&g_theWanCMApp.m_mutex);

	if (g_theWanCMApp.m_sgWan.status == "off")
	{
		if (g_theWanCMApp.m_switch_on == FALSE)
		{
			rslt = "Wan is not connected!";

			strncpy(errmsg, rslt.data(), errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_WanNotConnect;
		}
	
		wanStatusOff();
		//wanCM_kill_thread();
	}

	if (g_theWanCMApp.m_sgWan.status == "on")
	{
		if (g_theWanCMApp.m_switch_on == TRUE)
		{
			rslt = "Wan has been connected!";
			strncpy(errmsg, rslt.data(), errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_WanHasConnected;

		}
		if  ( (g_theWanCMApp.m_sgWan.hc_ip == "") || ( g_theWanCMApp.m_sgWan.hc_time == 0))
		{
			rslt = "Health check condition not set well!";
			g_theWanCMApp.m_sgWan.status = "off";
			strncpy(errmsg, rslt.data(), errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_HCConditionNotSet;
		}
		
		if (g_theWanCMApp.m_sgWan.method == "static")
		{
			if ( (g_theWanCMApp.m_sgWan.ip == "")||(g_theWanCMApp.m_sgWan.netmask == ""))
			{
				rslt = "You don't set the ip address!";
				g_theWanCMApp.m_sgWan.status = "off";
				strncpy(errmsg, rslt.data(), errlen-1);
				errmsg[errlen-1] = 0;
				return -eAosRc_StaticIpNotSet;
			}
			else
			{
				wanStatic(g_theWanCMApp.m_sgWan.ip, g_theWanCMApp.m_sgWan.netmask);		
			}
		}
		else if (g_theWanCMApp.m_sgWan.method == "dhcp")
		{
			wanDhcp();	
		}
		else if (g_theWanCMApp.m_sgWan.method == "PPPoE")
		{
			if((g_theWanCMApp.m_sgWan.ad_user == "")||(g_theWanCMApp.m_sgWan.ad_passwd == "")||(g_theWanCMApp.m_sgWan.dns1 == ""))
			{
				rslt = "PPPoE is not set well,Please the check you setting!";
				g_theWanCMApp.m_sgWan.status = "off";
				strncpy(errmsg, rslt.data(), errlen-1);
				errmsg[errlen-1] = 0;
				return -eAosRc_PPPoEConfigNotSet;
			}
			else
			{
				wanPppoe(g_theWanCMApp.m_sgWan.ad_user, g_theWanCMApp.m_sgWan.ad_passwd, g_theWanCMApp.m_sgWan.dns1, g_theWanCMApp.m_sgWan.dns2);	
			}
		}
		wanCM_start_thread();
	}
	*optlen = index;
	return 0;
}

int wanLoadConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
/*
	unsigned int index = 0;
	OmnFilePtr mFile;
	OmnString rslt;
	OmnString username;
	OmnString fn;
	OmnString passwd;
	OmnString method;
	OmnString dns1;
	OmnString dns2;
	OmnString ip;
	OmnString netmask;
	OmnString status;
	OmnString hc_time;
	OmnString hc_ip;

	fn = RHC_WAN_CONFIG;

	if (!mFile)
	{
		mFile = OmnNew OmnFile(fn, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			rslt << fn;
			return eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << fn;
			return eAosAlarm_FailedToOpenFile;
		}
	}

	OmnString contents, cmd, prefix, subPrefix;
	OmnString start = "------AosCliBlock: ";
	start << "wan" << "------\n";
	OmnString end = "------EndAosCliBlock------\n";
	bool err;

	if (!mFile->readBlock(start, end, contents, err))
	{
		if (err)
		{
			rslt << "********** Failed to read config from: ";
			rslt << fn << "\n";
			return -eAosAlarm_FailedToReadFile;
		}

		return 0;
	}

	try
	{
		OmnXmlItem config(contents);
		config.reset();
		while (config.hasMore())
		{
			OmnXmlItemPtr item = config.next();
			try
			{
				cmd = item->getStr();
				//cout << "Found command: " << cmd << endl;
				//			if (!OmnCliProc::getSelf()->runCli(cmd, rslt))
				int curPos = 0;
				curPos = cmd.getWord(curPos,prefix);
				if (prefix == "wan")
				{
					if (subPrefix == "adsl")  
					{
						curPos = cmd.getWord(curPos, subPrefix);
						if (subPrefix == "username")
						{

							curPos = cmd.getWord(curPos, subPrefix);
							if (subPrefix == "set")
							{
								curPos = cmd.getWord(curPos, g_theWanCMApp.m_sgWan.ad_user);		
								username = g_theWanCMApp.m_sgWan.ad_user;
							}
							else
							{

							}
						}
						else if (subPrefix == "password")
						{
							curPos = cmd.getWord(curPos, subPrefix);
							if (subPrefix == "set")
							{
								curPos = cmd.getWord(curPos, g_theWanCMApp.m_sgWan.ad_passwd);		
								passwd = g_theWanCMApp.m_sgWan.ad_passwd;
							}

						}
						else
						{
						}
					}
					else if (subPrefix == "dns")
					{

						curPos = cmd.getWord(curPos, subPrefix);
						if (subPrefix == "set")
						{
							curPos = cmd.getWord(curPos, g_theWanCMApp.m_sgWan.dns1);		
							curPos = cmd.getWord(curPos, g_theWanCMApp.m_sgWan.dns2);		
							dns1 = g_theWanCMApp.m_sgWan.dns1;
							dns2 = g_theWanCMApp.m_sgWan.dns2;
						}
						else
						{

						}
					}
					else if(subPrefix == "ip")
					{
						curPos = cmd.getWord(curPos, subPrefix);
						if (subPrefix == "address")
						{
							if (subPrefix == "set")
							{
								curPos = cmd.getWord(curPos, g_theWanCMApp.m_sgWan.ip);		
								curPos = cmd.getWord(curPos, g_theWanCMApp.m_sgWan.netmask);		
								ip = g_theWanCMApp.m_sgWan.ip;
								netmask = g_theWanCMApp.m_sgWan.netmask;
							}
							else
							{

							}
						}
					}
					else if (subPrefix == "connect")
					{
						curPos = cmd.getWord(curPos, subPrefix);
						if (subPrefix == "method")
						{
							if (subPrefix == "set")
							{
								curPos = cmd.getWord(curPos, g_theWanCMApp.m_sgWan.method);		
								method = g_theWanCMApp.m_sgWan.method;
								if (method == "static")
								{
									wanStatic(ip, netmask);
								}
								else if (method == "dhcp")
								{
									wanDhcp();
								}
								else if (method == "adsl")
								{
									wanPppoe(username, passwd, dns1, dns2);
								}
							}
							else
							{

							}
						} 
					}
					else if (subPrefix == "health")
					{
						curPos = cmd.getWord(curPos, subPrefix);
						if (subPrefix == "check")
						{
							if (subPrefix == "set")
							{
								curPos = cmd.getWord(curPos, subPrefix);
								if (subPrefix == "time")
								{
									curPos = cmd.getWord(curPos, g_theWanCMApp.m_sgWan.hc_time);		
									hc_time = g_theWanCMApp.m_sgWan.hc_time;
								}
								else if (subPrefix == "ip")
								{
									curPos = cmd.getWord(curPos, g_theWanCMApp.m_sgWan.hc_ip);		
									hc_ip = g_theWanCMApp.m_sgWan.hc_ip;
								}
								else
								{

								}
							}
							else
							{

							}
						}
					}
					else if (subPrefix == "status")
					{
						curPos = cmd.getWord(curPos, subPrefix);
						if (subPrefix == "set")
						{
							curPos =cmd.getWord(curPos, g_theWanCMApp.m_sgWan.status);
							status = g_theWanCMApp.m_sgWan.status;
						}
					}
					else
					{

					}
				}
				else
				{	
					rslt = "Error:This is a bad wan config file!";
					ERRMSG(errmsg, rslt, errlen);
				}
			}

			catch (const OmnExcept &e)
			{
				rslt = "Failed to read command: ";
				rslt << item->toString();
				//return -eAosAlarm_FailedToLoadConfig;
				return -1;
			}
		}
	}

	catch (const OmnExcept &e)
	{
		cout << "Failed to load configure for module: " 
			<< " Wan" << endl;
		//return -eAosAlarm_FailedToLoadConfig;
		return -1;
	}

	mFile->closeFile();
	mFile = 0;

	char* buf = CliUtil_getBuff(data);
	char local[1000];
	strcpy(local, "Wan's config load ok!");
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;*/
	return 0;
}


int WanConnectionMgr_regCliCmd(void)
{	
	int ret;

	wanInit();

	ret = CliUtil_regCliCmd("wan ip address set",wanIpAddressSet);
	ret |= CliUtil_regCliCmd("wan ip address show",wanIpAddressShow);
	ret |= CliUtil_regCliCmd("wan adsl username set",wanAdslUserSet);
	ret |= CliUtil_regCliCmd("wan adsl password set",wanAdslPasswdSet);
	ret |= CliUtil_regCliCmd("wan dns set",wanDnsSet);
	ret |= CliUtil_regCliCmd("wan connect method set",wanConnectMethodSet);
	ret |= CliUtil_regCliCmd("wan get ip",wanGetIp);
	ret |= CliUtil_regCliCmd("wan health check set time",wanHealthCheckSetTime);
	ret |= CliUtil_regCliCmd("wan health check set ip",wanHealthCheckSetIp);
	ret |= CliUtil_regCliCmd("wan show config",wanShowConfig);
	ret |= CliUtil_regCliCmd("wan clear config",wanClearConfig);
	ret |= CliUtil_regCliCmd("wan save config",wanSaveConfig);
	ret |= CliUtil_regCliCmd("wan load config",wanLoadConfig);
	ret |= CliUtil_regCliCmd("wan set status",wanSetStatus);

	return ret;
}

