////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystmePppoeCli.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelInterface/CliSysCmd.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
//#include "KernelInterface/CliProc.h"
//#include "KernelInterface/CliCmd.h"
//#include "KernelInterface/CliSysCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
// GB, 11/16/2006

#include "Global.h"

struct pppoeConfigEntry sgPppoe;

int pppoeInit()
{
	sgPppoe.username= "";
	sgPppoe.password= "";
	sgPppoe.dns1= "";
	sgPppoe.dns2= "";
	sgPppoe.status= "stop";
	
	return 0;
}

int pppoeStart(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
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
	
	if ((sgPppoe.username == 0) && (sgPppoe.password == 0) && (sgPppoe.dns1 == 0) \
	   && (sgPppoe.username == "") && (sgPppoe.password == "") && (sgPppoe.dns1 == ""))
	{
		rslt << "The PPPoE's config has not been setted well! Sorry, can not to run!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;

		return -eAosRc_ConfigureError;
	}

	OmnFilePtr pppoeFile1 = OmnNew OmnFile("../rhcConf/pppoe.conf.template",OmnFile::eReadOnly);
	OmnFilePtr pppoeFile2 = OmnNew OmnFile("../rhcConf/pap-secrets.template",OmnFile::eReadOnly);
	OmnFilePtr tmpFile1 = OmnNew OmnFile("../rhcConf/pppoe.conf.tmp",OmnFile::eCreate);
	OmnFilePtr tmpFile2 = OmnNew OmnFile("../rhcConf/pap-secrets.tmp",OmnFile::eCreate);
	
	if (!pppoeFile1)
	{
		rslt = "Failed to open configure file: ";
		rslt << "pppoe.conf.template";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}

	if (!pppoeFile1->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pppoe.conf.template";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}
	
    if (!tmpFile1)
	{
		rslt = "Failed to open configure file: ";
		rslt << "pppoe.conf.tmp";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpFile1->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pppoe.conf.tmp";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}
	
	if (!pppoeFile2)
	{
		rslt = "Failed to open configure file: ";
		rslt << "pap-secrets.template";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}

	if (!pppoeFile2->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pap-secrets.template";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}
	
    if (!tmpFile2)
	{
		rslt = "Failed to open configure file: ";
		rslt << "pap-secrets.tmp";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpFile2->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pap-secrets.tmp";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;

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
			newLine << "\'" << sgPppoe.username << "\'\n";
		 	tmpFile1->append(newLine);
			changedFlag1 = true;
		}
        else if(changedFlag2 == false && prefix1 == "DNS1=")
	   	{
			OmnString  newLine = "DNS1=";
			newLine << sgPppoe.dns1 << "\n";
		 	tmpFile1->append(newLine);
			changedFlag2 = true;
		}
        else if(changedFlag3 == false && prefix1 == "DNS2=")
		{
			OmnString  newLine = "DNS2=";
			newLine << sgPppoe.dns2 << "\n";
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
						newLine << sgPppoe.username << "\"\t" << "*\t\"" << sgPppoe.password << "\"\n";
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
		return -eAosRc_ConfigureError;
				
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

	sgPppoe.status = "start";

	*optlen = index;
	return 0;
}


// GB, 11/16/2006
int pppoeStop(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	
	unsigned int index = 0;
	OmnString systemCmd;
	OmnString rslt;
	if ( sgPppoe.status == "stop")
	{
		rslt << "pppoe has already  been stoped!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ServerAlreadyStopped;
	}
	
	systemCmd << "/usr/sbin/pppoe-stop";
	OmnCliSysCmd::doShell(systemCmd, rslt);
    sgPppoe.status = "stop";	

	*optlen = index;
	return 0;
}


// GB, 11/20/2006
int pppoeShow(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	//char dns[20];
	char local[2048];
	OmnString contents;
	 contents << "PPPoE information\n" 
		      << "----------------------------\n" 
			  << "      status : " << sgPppoe.status << "\n" 
			  << "    username : " << sgPppoe.username << "\n"
			  << "    password : " << sgPppoe.password << "\n"
			  << "        dns1 : " << sgPppoe.dns1 << "\n"
			  << "        dns2 : " << sgPppoe.dns2 << "\n";
	
	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	//*optlen = strlen(local);
	return 0;
}

// GB, 11/20/2006
int	pppoeUsernameSet(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	
	unsigned int index = 0;
	sgPppoe.username = parms->mStrings[0];

	*optlen = index;
	return 0;
}


// GB, 11/17/2006
int	pppoePasswordSet(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{  
	unsigned int index = 0;
	sgPppoe.password = parms->mStrings[0];

	*optlen = index;
	return 0;
}


// GB, 11/17/2006
int	pppoeDnsSet(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	int tmpDns1;
	int tmpDns2;
	char ip1[MAX_IP_ADDR_LEN];
	char ip2[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;

	tmpDns1 = parms->mIntegers[0];
	tmpDns2 = parms->mIntegers[1];
	
	tmpaddr.s_addr = tmpDns1;
	strcpy(ip1, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = tmpDns2;
	strcpy(ip2, inet_ntoa(tmpaddr));
	sgPppoe.dns1 = ip1;	
	sgPppoe.dns2 = ip2;	

	*optlen = index;
	return 0;
}

int pppoeSaveConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	//char dns[20];
	char local[2048];
	OmnString contents;
	
	 if (sgPppoe.status == "stop")
	 {
	 	contents<< "<Cmd>";
	 	contents << "pppoe stop";
	 	contents << "</Cmd>\n";
	 } 
	 else
	 {
	 	contents<< "<Cmd>";
	 	contents << "pppoe start";
	 	contents << "</Cmd>\n";
	 }

	 if (sgPppoe.username != "")
	 {
	 	contents<< "<Cmd>";
	 	contents << "pppoe username set " << sgPppoe.username;
	 	contents << "</Cmd>\n";
	 }

	 if (sgPppoe.password != "")
	 {
	 	contents<< "<Cmd>";
	 	contents << "pppoe password set " << sgPppoe.username;
	 	contents << "</Cmd>\n";
	 }

	 if ((sgPppoe.dns1 != ""))
	 {
	 	contents << "<Cmd>";
	 	contents << "pppoe dns set " << sgPppoe.dns1 << " " << sgPppoe.dns2;
	 	contents << "</Cmd>\n";
	 }
	
	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	//*optlen = strlen(local);
	return 0;
}

int	pppoeClearConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	pppoeInit();

	*optlen = index;
	return 0;
}


int Pppoe_regCliCmd(void)
{	
	int ret;
	
	ret = CliUtil_regCliCmd("pppoe start", pppoeStart);
	ret |= CliUtil_regCliCmd("pppoe stop", pppoeStop);
	ret |= CliUtil_regCliCmd("pppoe username set",pppoeUsernameSet);
	ret |= CliUtil_regCliCmd("pppoe password set",pppoePasswordSet );
	ret |= CliUtil_regCliCmd("pppoe dns set", pppoeDnsSet);
	ret |= CliUtil_regCliCmd("pppoe show config", pppoeShow);
	ret |= CliUtil_regCliCmd("pppoe save config", pppoeSaveConfig);
	ret |= CliUtil_regCliCmd("pppoe clear config", pppoeClearConfig);
	
	return ret;
}
