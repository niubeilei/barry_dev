////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Cli.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef AOS_OLD_CLI
#include "KernelInterface/Cli.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelInterface/CliCmd.h"
#include "KernelInterface/CliSysCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include <string.h>

//kevin 07/28/2006 for uname
#include <sys/utsname.h>
#include "KernelInterface_exe/version.h"

// GB, 11/16/2006

extern struct pppoeConfigEntry sgPppoe;

int 
AosCli::pppoeStart(OmnString &rslt)
{
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
		cout << "The PPPoE's config has not been setted well! Sorry, can not to run!" << endl;
		return -1;
	}

	OmnFilePtr pppoeFile1 = OmnNew OmnFile("../rhcConf/pppoe.conf.template",OmnFile::eReadOnly);
	OmnFilePtr pppoeFile2 = OmnNew OmnFile("../rhcConf/pap-secrets.template",OmnFile::eReadOnly);
	OmnFilePtr tmpFile1 = OmnNew OmnFile("../rhcConf/pppoe.conf.tmp",OmnFile::eCreate);
	OmnFilePtr tmpFile2 = OmnNew OmnFile("../rhcConf/pap-secrets.tmp",OmnFile::eCreate);
	
	if (!pppoeFile1)
	{
		rslt = "Failed to open configure file: ";
		rslt << "pppoe.conf.template";
		return eAosAlarm_FailedToOpenFile;
	}

	if (!pppoeFile1->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pppoe.conf.template";
		return eAosAlarm_FailedToOpenFile;
	}
	
    if (!tmpFile1)
	{
		rslt = "Failed to open configure file: ";
		rslt << "pppoe.conf.tmp";
		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpFile1->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pppoe.conf.tmp";
		return eAosAlarm_FailedToOpenFile;
	}
	
	if (!pppoeFile2)
	{
		rslt = "Failed to open configure file: ";
		rslt << "pap-secrets.template";
		return eAosAlarm_FailedToOpenFile;
	}

	if (!pppoeFile2->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pap-secrets.template";
		return eAosAlarm_FailedToOpenFile;
	}
	
    if (!tmpFile2)
	{
		rslt = "Failed to open configure file: ";
		rslt << "pap-secrets.tmp";
		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpFile2->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pap-secrets.tmp";
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
		return false;
				
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

	return 0;
}


// GB, 11/16/2006
int 
AosCli::pppoeStop(OmnString &rslt)
{
	if ( sgPppoe.status == "stop")
	{
		cout << "pppoe has already  been stoped!";
		return -1;
	}
	
	OmnString systemCmd;
	systemCmd << "/usr/sbin/pppoe-stop";
	OmnCliSysCmd::doShell(systemCmd, rslt);
    sgPppoe.status = "stop";	

	return 0;
}


// GB, 11/16/2006
int
AosCli::pppoeStatus(OmnString &rslt)
{
    OmnString systemCmd;

	systemCmd << "/usr/sbin/pppoe-status";
	OmnCliSysCmd::doShell(systemCmd, rslt);
			         
    return 0;
}


// GB, 11/20/2006
int
AosCli::pppoeShow(OmnString &rslt)
{
	cout << "PPPoE information\n" << "----------------------------\n" << "\t  status : " << sgPppoe.status << "\n\t" << "username : " << sgPppoe.username << "\n\tpassword : " << sgPppoe.password << "\n\t    dns1 : " << sgPppoe.dns1 << "\n\t    dns2 : " << sgPppoe.dns2 << endl;
	
	return 0;
}


// GB, 11/20/2006
int
AosCli::pppoeUsernameSet(OmnString &rslt)
{
	
	int curPos = 0;
	curPos = mFileName.getWord(curPos, sgPppoe.username);
	return 0;
}


// GB, 11/17/2006
int
AosCli::pppoePasswordSet(OmnString &rslt)
{  
	int curPos = 0;
	curPos = mFileName.getWord(curPos, sgPppoe.password);
	return 0;
}


// GB, 11/17/2006
int
AosCli::pppoeDnsSet(OmnString &rslt)
{
	int curPos = 0;

	curPos = mFileName.getWord(curPos, sgPppoe.dns1);
	curPos = mFileName.getWord(curPos, sgPppoe.dns2);	

	return 0;
}


//add by GB 11/20/06
int 
AosCli::saveOnlyPppoeConfig(OmnString &rslt)
{
	if (!mFile)
	{
		OmnString fn = mFileName;
		fn << ".active";
		mFile = OmnNew OmnFile(fn, OmnFile::eCreate);
		if (!mFile || !mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << fn;
			return aos_alarm(eAosMD_Platform, eAosAlarm_FailedToOpenFile, 
				"%s", rslt.data());
		}
	}

	OmnString contents;
	int ret;

	savePppoeConfig(rslt);
	contents = rslt;

	char local[200];
	sprintf(local, "------AosCliBlock: pppoe------\n");
	if (!mFile->put(local, false) ||
		!mFile->put("<Commands>\n", false) ||
	    !mFile->put(contents, false) ||
		!mFile->put("</Commands>\n", false) ||
		!mFile->put("------EndAosCliBlock------\n\n", false))
	{
		rslt = "Failed to write to config: ";
		rslt << mFileName;
		ret = aos_alarm(eAosMD_Platform, eAosAlarm_FailedToWriteConfig,
			"%s", rslt.data());
	}


	// 
	// Back up the current config
	//
	OmnFile file(mFileName, OmnFile::eReadOnly);
	OmnString cmd;
	if (file.isGood())
	{
		OmnString newFn = mFileName;
		newFn << ".bak";
		cmd << "cp " << mFileName << " " << newFn;
OmnTrace << "To run command: " << cmd << endl;
		system(cmd);
	}

	mFile->closeFile();
	mFile = 0;

	cmd = "mv -f ";

	cmd << mFileName << ".active " << mFileName;
	system(cmd);
	OmnTrace << "To run command: " << cmd << endl;

	return 0;
}


int 
AosCli::savePppoeConfig(OmnString &rslt)
{
	OmnString contents;
	
	contents << "<Cmd>pppoe username set " << sgPppoe.username << "</Cmd>\n";
	contents << "<Cmd>pppoe password set " << sgPppoe.password << "</Cmd>\n";
	contents << "<Cmd>pppoe dns set " << sgPppoe.dns1 << " " << sgPppoe.dns2 << "</Cmd>\n";
	contents << "<Cmd>pppoe " << sgPppoe.status << "</Cmd>\n";

	rslt = contents;
	
	return 0;
}


int 
AosCli::loadPppoeConfig(OmnString &rslt)
{
	if (!mFile)
	{
		mFile = OmnNew OmnFile(mFileName, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			rslt << mFileName;
			return eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << mFileName;
			return eAosAlarm_FailedToOpenFile;
		}
	}

	OmnString contents, cmd, pppoePrefix, subPrefix;
	OmnString start = "------AosCliBlock: ";
	start << "pppoe" << "------\n";
	OmnString end = "------EndAosCliBlock------\n";
	bool err;

	if (!mFile->readBlock(start, end, contents, err))
	{
		if (err)
		{
			rslt << "********** Failed to read config for: ";
			rslt << "pppoe" << "\n";
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
				curPos = cmd.getWord(curPos,pppoePrefix);
				if (pppoePrefix == "pppoe")
				{
					curPos = cmd.getWord(curPos,subPrefix);
					if(subPrefix == "username")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "set")
						{
							curPos = cmd.getWord(curPos, sgPppoe.username);
							continue;
						}
						else 
						{
							cout << "Error:The Pppoe SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else if(subPrefix == "password")
					{	
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "set")
						{
							curPos = cmd.getWord(curPos, sgPppoe.password);
							continue;
						}
						else 
						{
							cout << "Error:The Pppoe SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else if(subPrefix == "dns")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "set")
						{
							curPos = cmd.getWord(curPos, sgPppoe.dns1);
							curPos = cmd.getWord(curPos, sgPppoe.dns2);
							continue;
						}
						else 
						{
							cout << "Error:The Pppoe SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else if(subPrefix == "start")
					{
						curPos = cmd.getWord(curPos, sgPppoe.status);
						pppoeStart(rslt);
					}
					else if(subPrefix == "stop")
					{
						curPos = cmd.getWord(curPos, sgPppoe.status);
						pppoeStop(rslt);
					}
					else 
					{
						cout << "Error:The First SubPrefix is wrong!" << endl;
						continue;
					}
				}
				else
				{	
					cout << "Error:This is a bad pppoe command!" << endl;
					continue;
				}
			}

			catch (const OmnExcept &e)
			{
				rslt = "Failed to read command: ";
				rslt << item->toString();
				return -eAosAlarm_FailedToLoadConfig;
			}
		}
	}

	catch (const OmnExcept &e)
	{
		cout << "Failed to load configure for module: " 
			<< "pppoe" << endl;
		return -eAosAlarm_FailedToLoadConfig;
	}

	mFile->closeFile();
	mFile = 0;

	return 0;
}

#endif

