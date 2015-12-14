////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemFwBlacklist.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"
#include "KernelInterface/CliSysCmd.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/List.h"  
#include "aosUtil/Memory.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
//#include "Debug/Debug.h"
//#include "KernelInterface/CliProc.h"
//#include "KernelInterface/CliCmd.h"
#include "Tracer/Tracer.h"
//#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
//#include "XmlParser/XmlItem.h"
#include <sys/stat.h>
#include <string.h>
#include "Global.h"

using namespace std;

int fwBlacklistLoadFile(const OmnString fileName, OmnString &rslt)
{
    OmnFilePtr      mFile;
	struct stat s;	

	if ((stat(fileName, &s) < 0) || (!S_ISREG(s.st_mode)))
	{
		return -eAosRc_InvalidFileFormat; 
	}

	//cout << "filenameis " << fileName << endl;
	if (!mFile)
	{		
		mFile = OmnNew OmnFile(fileName, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			//rslt << &parms;
			return eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
		//	rslt << &parms;
			return eAosAlarm_FailedToOpenFile;
		}
	}
	
	bool isFileFinished = false;
	OmnString line1 = mFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString domainName;
		OmnString ipAddress;
		OmnString netMask;
		//cout << line1 << endl;
		int curPos = 0;
		curPos = line1.getWord(curPos,domainName);
		curPos = line1.getWord(curPos,ipAddress);
		
		//judge ip is invalid
	    if (inet_addr(ipAddress) == INADDR_NONE)
		{		
			return -eAosRc_InvalidIpAddress;
		}

		curPos = line1.getWord(curPos,netMask);
		//fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", ipAddress, netMask, "deny", "no log", rslt);
		OmnString forward("forward");
		OmnString tmpIp1("0.0.0.0");
		OmnString tmpMask("0.0.0.0");
		OmnString tmpMethod("deny");
		OmnString tmpLogFlag("nolog");
		/*mFileName  << tmpIp1 << tmpMask << ipAddress << " " << \
			netMask << " " << tmpMethod << " " <<tmpLogFlag;
	     cout<< mFileName << endl;*/
		
		fwIpfilAll(forward, inet_addr(tmpIp1), inet_addr(tmpMask), inet_addr(ipAddress), inet_addr(netMask), tmpMethod, tmpLogFlag, rslt);
		line1 = mFile->getLine(isFileFinished);
//		fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", "", "", "deny", "no log", rslt);
	}
	return 0;
}


int fwBlacklistLoadUrl(const OmnString fileName, OmnString &rslt)
{
	OmnFilePtr      mFile;
	OmnString systemCmd;
	systemCmd << "wget -q " << fileName;
	OmnCliSysCmd::doShell(systemCmd, rslt);

	OmnString shortName;
	unsigned char *startPos;
	unsigned char *curPos;
	unsigned char *endPos;
	struct stat s;	

	startPos = (unsigned char*)fileName.getBuffer();
	endPos = startPos + fileName.length();
	curPos = endPos;
	while(curPos >= startPos)
	{
		if(*curPos == '\\' || *curPos == '/')
		{
			shortName.assign((char*)(curPos + 1),endPos - curPos); 
			break;
		}
		curPos --;
	}
	if(shortName == "")
	{
		// pop an alarm
		return false;
	}
	
	//judge file format is valid
	if ((stat(shortName, &s) < 0) || (!S_ISREG(s.st_mode)))
	{
		return -eAosRc_InvalidFileFormat; 
	}
	
	if (!mFile)
	{
		mFile = OmnNew OmnFile(shortName, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			rslt << shortName;
			return eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << shortName;
			return eAosAlarm_FailedToOpenFile;
		}
	}
	
	bool isFileFinished = false;
	OmnString line1 = mFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString domainName;
		OmnString ipAddress;
		OmnString netMask;
		//cout << line1 << endl;
		int curPos = 0;
		curPos = line1.getWord(curPos,domainName);
		curPos = line1.getWord(curPos,ipAddress);

		if (inet_addr(ipAddress) == INADDR_NONE)
			return -eAosRc_InvalidIpAddress;

		curPos = line1.getWord(curPos,netMask);
		//fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", ipAddress, netMask, "deny", "no log", rslt);
		OmnString forward("forward");
		OmnString tmpIp1("0.0.0.0");
		OmnString tmpMask("0.0.0.0");
		OmnString tmpMethod("deny");
		OmnString tmpLogFlag("nolog");
		
		fwIpfilAll(forward, inet_addr(tmpIp1), inet_addr(tmpMask), inet_addr(ipAddress), inet_addr(netMask), tmpMethod, tmpLogFlag, rslt);
		line1 = mFile->getLine(isFileFinished);
//		fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", "", "", "deny", "no log", rslt);
	}
	
	OmnString rmCmd;
	rmCmd << "rm -f " << shortName;
	OmnCliSysCmd::doShell(rmCmd, rslt);
	
    return 0;
}

int fwBlacklistLoadFile(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString mFileName;
	OmnString fwCmd;
	struct stat s;	
	OmnFilePtr	mFile;
	OmnString rslt;

	mFileName = parms->mStrings[0];
	
	//check the file format
	if ((stat(mFileName, &s) < 0) || (!S_ISREG(s.st_mode)))
	{
		cout <<  "format is wrong" << endl;
		return -eAosRc_InvalidFileFormat; 
	}

	if (!mFile)
	{		
		mFile = OmnNew OmnFile(mFileName, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			rslt << mFileName;
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << mFileName;
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return eAosAlarm_FailedToOpenFile;
		}
	}
	
	bool isFileFinished = false;
	OmnString line1 = mFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString domainName = "";
		OmnString ipAddress = "";
		OmnString netMask = "";
		//cout << line1 << endl;
		int curPos = 0;
		curPos = line1.getWord(curPos,domainName);
		curPos = line1.getWord(curPos,ipAddress);
		cout<<ipAddress<<endl;	
		//judge ip valid
		if (inet_addr(ipAddress) == INADDR_NONE)
		{
			cout << "ip is wrong" << endl;
			return -eAosRc_InvalidIpAddress;
		}
		curPos = line1.getWord(curPos,netMask);
		//fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", ipAddress, netMask, "deny", "no log", rslt);

		fwCmd << "/sbin/iptables -A FORWARD -s 0.0.0.0/0.0.0.0 -d "
			  << ipAddress << "/" << netMask << " -j DROP";
				
		OmnCliSysCmd::doShell(fwCmd, rslt);
		//initialize the fwCmd again.
		fwCmd = "";
		//fwIpfilAll(forward,tmpIp1,tmpMask,ipAddress, netMask, tmpMethod, tmpLogFlag, rslt);
		line1 = mFile->getLine(isFileFinished);
//		fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", "", "", "deny", "no log", rslt);
	}
		
	struct fwFilterListEntry * data1;
	if((data1 = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data1, 0, sizeof(struct fwFilterListEntry));
	data1->type = "fwBlacklistFile";
	data1->proto = mFileName;
	aos_list_add_tail((struct aos_list_head *)&data1->datalist, &sgFwFilterList);

	*optlen = index;
	return 0;
}

// create by Bin Gong
int fwBlacklistLoadUrl(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString mFileName;
	OmnString rslt;
	OmnString fwCmd;
	OmnFilePtr	mFile;
	OmnString systemCmd;
	OmnString fileName;
	unsigned char *startPos;
	unsigned char *curPos;
	unsigned char *endPos;
	struct stat s;

	mFileName = parms->mStrings[0];
	systemCmd << "wget -q " << mFileName;
	OmnCliSysCmd::doShell(systemCmd, rslt);

	startPos = (unsigned char*)mFileName.getBuffer();
	endPos = startPos + mFileName.length();
	curPos = endPos;
	while(curPos >= startPos)
	{
		if(*curPos == '\\' || *curPos == '/')
		{
			fileName.assign((char*)(curPos + 1),endPos - curPos); 
			break;
		}
		curPos --;
	}
	if(fileName == "")
	{
		rslt = "Url is incorrect!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}

	//check the file format
	if ((stat(fileName, &s) < 0) || (!S_ISREG(s.st_mode)))
	{
		return -eAosRc_InvalidFileFormat; 
	}

	if (!mFile)
	{
		mFile = OmnNew OmnFile(fileName, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			rslt << fileName;
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;

			return eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << fileName;
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;

			return eAosAlarm_FailedToOpenFile;
		}
	}
	
	bool isFileFinished = false;
	OmnString line1 = mFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString domainName = "";
		OmnString ipAddress = "";
		OmnString netMask = "";
		//cout << line1 << endl;
		int curPos = 0;
		curPos = line1.getWord(curPos,domainName);
		curPos = line1.getWord(curPos,ipAddress);

		//judge ip valid
		if (inet_addr(ipAddress) == INADDR_NONE)
			return -eAosRc_InvalidIpAddress;
		curPos = line1.getWord(curPos,netMask);
		//fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", ipAddress, netMask, "deny", "no log", rslt);
		
		fwCmd << "/sbin/iptables -A FORWARD -s 0.0.0.0/0.0.0.0 -d "
		    	<< ipAddress << "/" << netMask << " -j DROP";
		
		OmnCliSysCmd::doShell(fwCmd, rslt);
		fwCmd = "";
		line1 = mFile->getLine(isFileFinished);
	}
	
	OmnString rmCmd;
	rmCmd << "rm -f " << fileName;
	OmnCliSysCmd::doShell(rmCmd, rslt);
		
	struct fwFilterListEntry * data1;
	if((data1 = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data1, 0, sizeof(struct fwFilterListEntry));
	data1->type = "fwBlacklistUrl";
	data1->proto = mFileName;
	aos_list_add_tail((struct aos_list_head *)&data1->datalist, &sgFwFilterList);

	*optlen = index;
    return 0;
}


//create by Bin Gong
int fwBlacklistRemoveFile(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString mFileName;
	OmnString rslt;
	OmnString fwCmd;
	OmnFilePtr	mFile;
	struct stat s;

	mFileName = parms->mStrings[0];

	//check the file format
	if ((stat(mFileName, &s) < 0) || (!S_ISREG(s.st_mode)))
	{
		return -eAosRc_InvalidFileFormat; 
	}

	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;

    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwBlacklistFile") && (ptr->proto == mFileName)) 
   		{       
        	aos_list_del(&ptr->datalist);
    	    aos_free(ptr);
	        existflag = 1;
			break;
      	}            
    }
    if (existflag == 0) 
    {   
        rslt = "Error: The rule not exist !";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
    }
    else
    {
   		if (!mFile)
		{
			mFile = OmnNew OmnFile(mFileName, OmnFile::eReadOnly);
			if (!mFile)
			{
				rslt = "Failed to open configure file: ";
				rslt <<mFileName;
				strncpy(errmsg, rslt.data(),errlen-1);
				errmsg[errlen-1] = 0;

				return eAosAlarm_FailedToOpenFile;
			}

			if (!mFile->isGood())
			{
				rslt = "Failed to open configure file: ";
				rslt << mFileName;
				strncpy(errmsg, rslt.data(),errlen-1);
				errmsg[errlen-1] = 0;

				return eAosAlarm_FailedToOpenFile;
			}
		}
		bool isFileFinished = false;

		OmnString line1 = mFile->getLine(isFileFinished);
		while(!isFileFinished)
		{
			OmnString domainName = "";
			OmnString ipAddress = "";
			OmnString netMask = "";
			//cout << line1 << endl;
			int curPos = 0;
			curPos = line1.getWord(curPos,domainName);
			curPos = line1.getWord(curPos,ipAddress);

			//judge ip valid
			if (inet_addr(ipAddress) == INADDR_NONE)
			return -eAosRc_InvalidIpAddress;

			curPos = line1.getWord(curPos,netMask);

			fwCmd << "/sbin/iptables -D FORWARD -s 0.0.0.0/0.0.0.0 -d " 
				  << ipAddress << "/" << netMask << " -j DROP";
			//cout << fwCmd << "\n";
			OmnCliSysCmd::doShell(fwCmd, rslt);
			fwCmd = "";
		    line1 = mFile->getLine(isFileFinished);
		}
    }	
	*optlen = index;
    return 0;
}


//create by Bin Gong 11-14 
int fwBlacklistRemoveUrl(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
   	unsigned int index = 0;
	OmnString mFileName;
	OmnString rslt;
	OmnString fwCmd;
	OmnFilePtr	mFile;
	OmnString fileName;
    OmnString systemCmd;
	unsigned char *startPos;
	unsigned char *curPos;
	unsigned char *endPos;
	struct stat s;

	mFileName = parms->mStrings[0];

	systemCmd << "wget -q " << mFileName;
	OmnCliSysCmd::doShell(systemCmd, rslt);

	startPos = (unsigned char*)mFileName.getBuffer();
	endPos = startPos + mFileName.length();
	curPos = endPos;

	while(curPos >= startPos)
	{
		if(*curPos == '\\' || *curPos == '/')
		{
			fileName.assign((char*)(curPos + 1),endPos - curPos); 
			break;
		}
		curPos --;
	}
	if(fileName == "")
	{
		rslt = "url is incorrect!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	
   //check the file format
	if ((stat(fileName, &s) < 0) || (!S_ISREG(s.st_mode)))
	{
		return -eAosRc_InvalidFileFormat; 
	}
	
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;

    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwBlacklistUrl") && (ptr->proto == mFileName)) 
   		{       
        	aos_list_del(&ptr->datalist);
    	    aos_free(ptr);
	        existflag = 1;
			break;
      	}            
    }
    if (existflag == 0) 
    {   
        rslt = "Error: The rule not exist !";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
    }
	else
    {
		if (!mFile)
		{
			mFile = OmnNew OmnFile(fileName, OmnFile::eReadOnly);
			if (!mFile)
			{
				rslt = "Failed to open configure file: ";
				rslt << fileName;
				strncpy(errmsg, rslt.data(),errlen-1);
				errmsg[errlen-1] = 0;

				return eAosAlarm_FailedToOpenFile;
			}

			if (!mFile->isGood())
			{
				rslt = "Failed to open configure file: ";
				rslt << fileName;
				strncpy(errmsg, rslt.data(),errlen-1);
				errmsg[errlen-1] = 0;

				return eAosAlarm_FailedToOpenFile;
			}
		}
		bool isFileFinished = false;
		OmnString line1 = mFile->getLine(isFileFinished);
		while(!isFileFinished)
		{
			OmnString domainName = "";
			OmnString ipAddress = "";
			OmnString netMask = "";
			//cout << line1 << endl;
			int curPos = 0;
			curPos = line1.getWord(curPos,domainName);
			curPos = line1.getWord(curPos,ipAddress);

			if (inet_addr(ipAddress) == INADDR_NONE)
				return -eAosRc_InvalidIpAddress;
			curPos = line1.getWord(curPos,netMask);
	
			fwCmd << "/sbin/iptables -D FORWARD -s 0.0.0.0/0.0.0.0 -d "
				  << ipAddress << "/" << netMask << " -j DROP";
			OmnCliSysCmd::doShell(fwCmd, rslt);
       		fwCmd = "";	
		    line1 = mFile->getLine(isFileFinished);
		}
	
	}
    

	OmnString rmCmd;
	rmCmd << "rm -f " << fileName;
	OmnCliSysCmd::doShell(rmCmd, rslt);
	
	*optlen = index;
    return 0;
}

int FwBlacklist_regCliCmd(void)
{	
	int ret;
	
	ret = CliUtil_regCliCmd("firewall blacklist load file", fwBlacklistLoadFile);
	ret |= CliUtil_regCliCmd("firewall blacklist remove file", fwBlacklistRemoveFile);
	ret |= CliUtil_regCliCmd("firewall blacklist load url", fwBlacklistLoadUrl);
	ret |= CliUtil_regCliCmd("firewall blacklist remove url", fwBlacklistRemoveUrl);
	
	return ret;
}
