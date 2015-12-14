////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemPptpCli.cpp
// Description:
//   
//
// Modification History: 2007/4/10
// 
////////////////////////////////////////////////////////////////////////////
#include "KernelInterface/CliSysCmd.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"
#include "aosUtil/List.h"  

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"

#include "Global.h"


struct pptpConfigEntry sgPptp;
struct pptpUserEntry pptpUser;
struct aos_list_head sgPptpUserList;

int pptpInit()
{
	sgPptp.ipLocal= 0;
	sgPptp.ip1= 0;
	sgPptp.ip2= 0;
	sgPptp.status= "stop";
	AOS_INIT_LIST_HEAD(&sgPptpUserList);
	return 0;
}

int pptpStart(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	unsigned int index = 0;
	bool changedFlag1 = false;
    bool changedFlag2 = false;
    bool changedFlag3 = false;
	bool isFileFinished1 = false;
	bool isFileFinished3 = false;
	
	OmnString stopPptpCmd;
	OmnString rslt;
	stopPptpCmd << "/etc/init.d/pptpd stop ";
	OmnCliSysCmd::doShell(stopPptpCmd, rslt);
	
	if ((sgPptp.ipLocal == 0) && (sgPptp.ip1 == 0) && (sgPptp.ip2 == 0)) 
	{
		//cout << "The PPtp's config has not been setted well! Sorry, can not to run!" << endl;
		
		return -eAosRc_ConfigureError;
	}

	OmnFilePtr pptpFile1 = OmnNew OmnFile("../rhcConf/pptpd.conf.template",OmnFile::eReadOnly);
	OmnFilePtr pptpFile3 = OmnNew OmnFile("../rhcConf/chap-secrets.template",OmnFile::eReadOnly);
	OmnFilePtr tmpFile1 = OmnNew OmnFile("../rhcConf/pptpd.conf.tmp",OmnFile::eCreate);
	OmnFilePtr tmpFile3 = OmnNew OmnFile("../rhcConf/chap-secrets.tmp",OmnFile::eCreate);
	
	if (!pptpFile1)
	{
		rslt = "Failed to open configure file: ";
		rslt << "pptpd.conf.template";
		return eAosAlarm_FailedToOpenFile;
	}

	if (!pptpFile1->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pptpd.conf.template";
		return eAosAlarm_FailedToOpenFile;
	}
	
    if (!tmpFile1)
	{
		rslt = "Failed to open configure file: ";
		rslt << "pptpd.conf.tmp";
		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpFile1->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pptpd.conf.tmp";
		return eAosAlarm_FailedToOpenFile;
	}
	
    if (!pptpFile3)
	{
		rslt = "Failed to open configure file: ";
		rslt << "chap-secrets.template";
		return eAosAlarm_FailedToOpenFile;
	}

	if (!pptpFile3->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "chap-secrets.template";
		return eAosAlarm_FailedToOpenFile;
	}
	
    if (!tmpFile3)
	{
		rslt = "Failed to open configure file: ";
		rslt << "chap-secrets.tmp";
		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpFile3->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "chap-secrets.tmp";
		return eAosAlarm_FailedToOpenFile;
	}

    OmnString line1 = pptpFile1->getLine(isFileFinished1);
	while(!isFileFinished1)
	{
		OmnString word1;
		int curPos1 = 0;
		curPos1 = line1.getWord(curPos1,word1);
    	if(changedFlag1 == false &&  word1 == "localip")
	   	{
			OmnString  newLine = "localip";
			newLine << " " << sgPptp.ipLocal << "\n";
		 	tmpFile1->append(newLine);
			changedFlag1 = true;
		}
        else if(changedFlag2 == false && word1 == "remoteip")
	   	{
			OmnString  newLine = "remoteip ";
			if(sgPptp.ip2 == 0)
			{
				newLine << sgPptp.ip1 << "\n";
		 		tmpFile1->append(newLine);
				changedFlag2 = true;
			}
			else
			{
				newLine << sgPptp.ip1 << "," << sgPptp.ip2 << "\n";
		 		tmpFile1->append(newLine);
				changedFlag2 = true;
			}
		}
		else
		{
			line1 << "\n";
			tmpFile1->append(line1);
		}
		line1 = pptpFile1->getLine(isFileFinished1);
	}

	
    OmnString line3 = pptpFile3->getLine(isFileFinished3);
	while(!isFileFinished3)
	{
    	if(changedFlag3 == false &&  line3 == "# client server secret IP addresses")
	   	{
			line3 << "\n" ;
	        tmpFile3->append(line3);	   
		   	pptpFile3->getLine(isFileFinished3);	
			struct pptpUserEntry * ptr;
			OmnString newLine = 0;
			aos_list_for_each_entry(ptr, &sgPptpUserList, datalist)
			{
				if(ptr->ip == 0)
				{
					newLine << ptr->username << " " ;
					newLine <<"pptpd " <<  ptr->password << " " << "*" << "\n";
				}
				else
				{
					newLine <<  ptr->username << " " ;
					newLine <<"pptpd " << ptr->password << " " << ptr->ip << "\n";
				}
			}
		    changedFlag3= true;
			tmpFile3->append(newLine);
			break;
		}
		else
		{
			line3 << "\n";
			tmpFile3->append(line3);
		}
		line3 = pptpFile3->getLine(isFileFinished3);
	}

    if(changedFlag1 == false || changedFlag2 == false || changedFlag3 == false )
	{
		// it's an error, send an alarm, then return false;
		// cout << "Not change the config!";
		return -eAosRc_ConfigureError;
				
	}

	pptpFile1->closeFile();
	pptpFile3->closeFile();
	tmpFile1->closeFile();
	tmpFile3->closeFile();
	
   OmnString renameCmd;
	renameCmd << "/bin/mv -f ../rhcConf/pptpd.conf.tmp /etc/pptpd.conf && /bin/mv -f ../rhcConf/chap-secrets.tmp  /etc/ppp/chap-secrets";
	OmnCliSysCmd::doShell(renameCmd, rslt);
   
	OmnString systemCmd;
	
	systemCmd << "/etc/init.d/pptpd start";
	OmnCliSysCmd::doShell(systemCmd, rslt);
    sgPptp.status = "start";	
	*optlen = index;

	return 0;
}

extern int pptpStop(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	unsigned int index = 0;
	if ( sgPptp.status == "stop")
	{
		//cout << "pptp has already been stoped!";
		return -eAosRc_ServerAlreadyStopped;
	}
	OmnString systemCmd;
	OmnString rslt;
	
	systemCmd << "/etc/init.d/pptpd stop";
	OmnCliSysCmd::doShell(systemCmd, rslt);
	sgPptp.status = "stop";
	*optlen = index;

	return 0;
}


// GB, 11/22/2006
extern int pptpIpLocal(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	unsigned int index = 0;
	sgPptp.ipLocal = parms->mIntegers[0];
	*optlen = index;

	return 0;
}


// GB, 11/22/2006
extern int pptpIpRange(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	unsigned int index = 0;
	sgPptp.ip1 = parms->mIntegers[0];
	sgPptp.ip2 = parms->mIntegers[1];

	*optlen = index;
	return 0;
}


extern int pptpAddUser(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	OmnString tmpUser;
	OmnString tmpPassword;
	
	tmpUser = parms->mStrings[0];
	tmpPassword = parms->mStrings[1];

	if(tmpUser == "" ||
	   tmpPassword == "")
	{
		//rslt = "invalid value";
		return -eAosRc_InvalidParm;
	}
		
	struct pptpUserEntry * ptr;

    aos_list_for_each_entry(ptr, &sgPptpUserList, datalist)
    {       
    	if(ptr->username == tmpUser) 
   		{       
        	//cout << "Error: The user already exists !" << endl;
			return -eAosRc_PptpUserExist;
      	}            
    }

	struct pptpUserEntry * dataptr;
	if((dataptr = (struct pptpUserEntry *)aos_malloc(sizeof(struct pptpUserEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(dataptr, 0, sizeof(struct pptpUserEntry));
	dataptr->username = tmpUser;
	dataptr->password = tmpPassword;
	dataptr->ip = parms->mIntegers[0];
	aos_list_add_tail((struct aos_list_head *)&dataptr->datalist, &sgPptpUserList);

//cout << "ADD USER OK!" << endl;

   *optlen = index;
   return 0;
}


extern int pptpDelUser(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	int existflag = 0;
	OmnString tmpUser;
	
	tmpUser = parms->mStrings[0];
	if(tmpUser == "")
	{
		//rslt = "invalid value";
		return false;
	}
	
	struct pptpUserEntry * ptr;
	struct pptpUserEntry * tmp;

   	aos_list_for_each_entry_safe(ptr, tmp, &sgPptpUserList, datalist)
    {       
    	if(ptr->username == tmpUser) 
   		{       
        	aos_list_del(&ptr->datalist);
    	    aos_free(ptr);
	        existflag = 1;
			break;
      	}            
    }
  
  	if (existflag != 1)
	{
        	//cout << "Error: The username does not exist !" << endl;
			return -eAosRc_PptpUserNotExist;
	}

	return 0;
}


// GB, 11/22/2006
extern int pptpShowConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[2048];
	char ipLocal[MAX_IP_ADDR_LEN];
	char startIp[MAX_IP_ADDR_LEN];
	char endIp[MAX_IP_ADDR_LEN];
	char userIp[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	
	if (sgPptp.ipLocal != 0)
	{
		tmpaddr.s_addr = sgPptp.ipLocal;
		strcpy(ipLocal, inet_ntoa(tmpaddr));
	}
	else
	{
		strcpy(ipLocal, "");
	}

	if (sgPptp.ip1 != 0)
	{
		tmpaddr.s_addr = sgPptp.ip1;
		strcpy(startIp, inet_ntoa(tmpaddr));
	}
	else
	{
		strcpy(startIp, "");
	}

	if (sgPptp.ip2 != 0)
	{
		tmpaddr.s_addr = sgPptp.ip2;
		strcpy(endIp, inet_ntoa(tmpaddr));
	}
	else
	{
		strcpy(endIp, "");
	}
	
	OmnString contents;
	OmnString userlist;
	struct pptpUserEntry * ptr;

	contents << "PPTP information\n" << "----------------------------\n\t   status : " \
			<< sgPptp.status << "\n\t" << " local ip : " << ipLocal \
			<< "\n\t ip range : " << endIp << " " << endIp \
			<< "\n\tuser list : \n" ;
	
	aos_list_for_each_entry(ptr, &sgPptpUserList, datalist)
	{
		if (ptr->ip != 0)
		{
			tmpaddr.s_addr = ptr->ip;
			strcpy(userIp, inet_ntoa(tmpaddr));
		}
		else
		{
			strcpy(userIp, "");
		}
		userlist << "\t\t\t" << ptr->username << " " << ptr->password 
				 << " " << userIp << "\n";
	}
	contents << "\t\t\tusername password ip\n\t\t\t--------------------\n" << userlist;

	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;
	return 0;
}

int savePptpConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	OmnString contents;
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[2048];
	char ipLocal[MAX_IP_ADDR_LEN];
	char startIp[MAX_IP_ADDR_LEN];
	char endIp[MAX_IP_ADDR_LEN];
	char userIp[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	
	if (sgPptp.ipLocal != 0)
	{
		tmpaddr.s_addr = sgPptp.ipLocal;
		strcpy(ipLocal, inet_ntoa(tmpaddr));
		contents << "<Cmd>pptp local ip " << ipLocal << "</Cmd>\n";
	}

	if (sgPptp.ip1 != 0 && sgPptp.ip2)
	{
		tmpaddr.s_addr = sgPptp.ip1;
		strcpy(startIp, inet_ntoa(tmpaddr));
		tmpaddr.s_addr = sgPptp.ip2;
		strcpy(endIp, inet_ntoa(tmpaddr));
		contents << "<Cmd>pptp ip range " << startIp << " " << endIp <<"</Cmd>\n";
	}

	if (sgPptp.status == "stop")
	{
		contents << "<Cmd>pptp stop</Cmd>\n";
	}
	else
	{
		contents << "<Cmd>pptp start</Cmd>\n";
	}
	struct pptpUserEntry  * ptr; 
	aos_list_for_each_entry(ptr, &sgPptpUserList, datalist)
	{
		if (ptr->username != "" && ptr->password != "")
		{
			if (ptr->ip != 0)
			{
				tmpaddr.s_addr = ptr->ip;
				strcpy(userIp, inet_ntoa(tmpaddr));
			}
			else
			{
				strcpy(userIp, "");
			}
			contents << "<Cmd>pptp add user ";
			contents << ptr->username << " ";
			contents << ptr->password << " ";
			contents << userIp << "</Cmd>\n";
		}
	}	
	
	//cout << contents << endl;
	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	return 0;
}


int pptpClearConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	pptpInit();

	struct pptpUserEntry * ptr;
	struct pptpUserEntry * tmp;
	aos_list_for_each_entry_safe(ptr, tmp,&sgPptpUserList, datalist)
	{
		aos_list_del(&ptr->datalist);
		aos_free(ptr);
	}

	*optlen = 0;
	return 0;
}

int Pptp_regCliCmd(void)
{	
	int ret;

	ret = CliUtil_regCliCmd("pptp start", pptpStart);
	ret |= CliUtil_regCliCmd("pptp stop", pptpStop);
	ret |= CliUtil_regCliCmd("pptp local ip", pptpIpLocal);
	ret |= CliUtil_regCliCmd("pptp ip range", pptpIpRange);
	ret |= CliUtil_regCliCmd("pptp add user", pptpAddUser);
	ret |= CliUtil_regCliCmd("pptp del user", pptpDelUser);
	ret |= CliUtil_regCliCmd("pptp show config", pptpShowConfig);
	ret |= CliUtil_regCliCmd("pptp save config", savePptpConfig);
	ret |= CliUtil_regCliCmd("pptp clear config", pptpClearConfig);
	return ret;	
}
