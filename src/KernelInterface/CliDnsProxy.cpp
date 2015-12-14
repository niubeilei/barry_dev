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
#include "aosUtil/List.h"  
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

extern struct dnsProxyConfigEntry sgDnsproxy;

// Gb, 11/14/2006
int 
AosCli::dnsproxyStart(OmnString &rslt)
{
	OmnString stopDnsproxyCmd;
	stopDnsproxyCmd << "/usr/bin/killall -9 pdnsd >/dev/null 2>&1";
	OmnCliSysCmd::doShell(stopDnsproxyCmd, rslt);
	
	bool changedFlag1 = false;
    bool changedFlag2 = false;
    bool changedFlag3 = false;
    bool changedFlag4 = false;
    bool isFileFinished = false;

	if ((sgDnsproxy.ip == 0) && (sgDnsproxy.ip == "") && (sgDnsproxy.lIp == 0) && (sgDnsproxy.lIp == ""))
	{
		cout << "The DNSPROXY's config has not been setted well! Sorry, can not to run!" << endl;
		return -1;
	}
	
	OmnFilePtr pdnsFile = OmnNew OmnFile("../rhcConf/pdnsd.conf.template",OmnFile::eReadOnly);
	OmnFilePtr tmpFile = OmnNew OmnFile("../rhcConf/pdnsd_init.conf",OmnFile::eCreate);
	
	if (!pdnsFile)
	{
		rslt = "Failed to open configure file: ";
		rslt << "pdnsd.conf.template";
		return eAosAlarm_FailedToOpenFile;
	}

	if (!pdnsFile->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pdnsd.conf.template";
		return eAosAlarm_FailedToOpenFile;
	}
	
        if (!tmpFile)
	{
		rslt = "Failed to open configure file: ";
		rslt << "dnsd_init.conf";
		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpFile->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pdnsd_init.conf";
		return eAosAlarm_FailedToOpenFile;
	}
	OmnString line1 = pdnsFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString word1;
		int curPos = 0;
		curPos = line1.getWord(curPos,word1);
		OmnString prefix1(word1.getBuffer(),10);
    	if(changedFlag1 == false && (word1 == "label" || word1 == "label="))
	   	{
			OmnString  newLine= "\tlabel";
			newLine << "= \"" << sgDnsproxy.name << "\";\n";
			tmpFile->append(newLine);
			changedFlag1 = true;
		}
		else if(changedFlag2 == false && (word1 == "ip" || word1 == "ip="))
	   	{
			OmnString  newLine = "\tip";
			newLine << " = " << sgDnsproxy.ip << ";\n";
		 	tmpFile->append(newLine);
			changedFlag2 = true;
		}
        else if(changedFlag3 == false && word1 == "server_ip")
	   	{
			OmnString  newLine = "\tserver_ip";
			newLine << " = " << sgDnsproxy.lIp << ";\n";
		 	tmpFile->append(newLine);
			changedFlag3 = true;
		}
        else if(changedFlag4 == false &&  prefix1 == "interface=")
	   	{
            OmnString  eth;
			if(sgDnsproxy.dev == "outcard")
			{
				eth = "eth0";
			}
			else if(sgDnsproxy.dev == "incard")
			{
				eth = "eth1";
			}
			else
			{
				cout << "You input is wrong, please check!";	
			}
			OmnString  newLine = "\tinterface=";
			newLine << " " << eth << ";\n";
		 	tmpFile->append(newLine);
			changedFlag4 = true;
		}
	    else if (sgDnsproxy.global == "yes" && line1 == "/*# This section is meant for resolving from root servers.")
		{
			OmnString  newLine = "#This section is meant for resolving from root servers.\n";
		 	tmpFile->append(newLine);
		}
        else if (sgDnsproxy.global == "yes" && line1 == "}*/")
		{
			OmnString  newLine = "}\n";
		 	tmpFile->append(newLine);
		}
       /* else if (sgDnsproxy.global == "yes" && line1 == "# fixed connections to the Internet.")
		{
			OmnString  newLine = "\/\*# fixed connections to the Internet.\n";
		 	tmpFile->append(newLine);
		}
        else if (sgDnsproxy.global == "yes" && line1 == "} #fixed")
		{
			OmnString  newLine = "}*\/\n";
		 	tmpFile->append(newLine);
		}*/
        else
		{
			line1 << "\n";
			tmpFile->append(line1);
		}
		line1 = pdnsFile->getLine(isFileFinished);
	}
     
	if(changedFlag1 == false || changedFlag2 == false || changedFlag3 == false )
	{
		// it's an error, send an alarm, then return false;
		//cout << "Not change the config!";
		return false;
	}
	tmpFile->closeFile();
	pdnsFile->closeFile();
	
    OmnString renameCmd;
	renameCmd << "/bin/mv ../rhcConf/pdnsd_init.conf /usr/local/etc/pdnsd.conf";
	OmnCliSysCmd::doShell(renameCmd, rslt);

	OmnString systemCmd;
	//systemCmd << "../rhcConf/pdnsdstart.sh";
	systemCmd << "/usr/local/sbin/pdnsd >/dev/null 2>&1 &";
	OmnCliSysCmd::doShell(systemCmd, rslt);
	sgDnsproxy.status = "start";
	
	return 0;
}

// GB 11/14/2006
int 
AosCli::dnsproxyStop(OmnString &rslt)
{
	if (sgDnsproxy.status == "stop")
	{
		cout << "Dnsproxy already has been stoped!";
		return -1;
	}
	OmnString systemCmd;
	
	systemCmd << "/usr/bin/killall -9 pdnsd >/dev/null 2>&1 ";
	OmnCliSysCmd::doShell(systemCmd, rslt);
	
	sgDnsproxy.status = "stop";
			         
    return 0;
}


// GB, 11/14/2006
int
AosCli::dnsproxyShow(OmnString &rslt)
{
	cout << "DNS PROXY information\n" << "--------------------------------\n" <<"\t   status : " << sgDnsproxy.status << "\n\t   global : " << sgDnsproxy.global << "\n\t    label : " << sgDnsproxy.name << "\n\t      dns : " << sgDnsproxy.ip << "\n\t local ip : " << sgDnsproxy.lIp << "\n\t      dev : " << sgDnsproxy.dev << endl; 
	return 0;
}


int
AosCli::dnsproxySetName(OmnString &rslt)
{
	OmnString ispName;
	int curPos = 0;
	curPos = mFileName.getWord(curPos,ispName);
	sgDnsproxy.name = ispName;
	return 0;
}


int
AosCli::dnsproxySetIp(OmnString &rslt)
{
	OmnString ipAddress;
	int curPos = 0;
	curPos = mFileName.getWord(curPos,ipAddress);
	sgDnsproxy.ip = ipAddress;
	return 0;
}


int
AosCli::dnsproxyAddGlobal(OmnString &rslt)
{
	sgDnsproxy.global = "yes";
	return 0;
}


int
AosCli::dnsproxyDelGlobal(OmnString &rslt)
{
	if(sgDnsproxy.global == "no")
	{
		cout << "You not use global option!";
	}
	else
	{
		sgDnsproxy.global = "no";
	}
	return 0;
}


// GB   11/24/2006 
int
AosCli::dnsproxySetLIp(OmnString &rslt)
{
	OmnString ipAddress;
	OmnString dev;
	int curPos = 0;
	curPos = mFileName.getWord(curPos,ipAddress);
	curPos = mFileName.getWord(curPos,dev);
	if((dev == 0) || (dev == "") )
	{
	    sgDnsproxy.lIp = ipAddress;
	    sgDnsproxy.dev = "incard";
        }
	else if((dev == "outcard") ||(dev == "incard"))
    {	
	    sgDnsproxy.dev = dev;
	    sgDnsproxy.lIp = ipAddress;
	}
	else
	{
		cout << "You input is wrong, the parameters must be outcard or incard";
	}
	return 0;
}


//add by GB 11/20/06
int 
AosCli::saveOnlyDnsproxyConfig(OmnString &rslt)
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

	saveDnsproxyConfig(rslt);
	contents = rslt;

	char local[200];
	sprintf(local, "------AosCliBlock: dns proxy------\n");
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
AosCli::saveDnsproxyConfig(OmnString &rslt)
{
	OmnString contents;
	
	contents << "<Cmd>dns proxy set name " << sgDnsproxy.name << "</Cmd>\n";
	contents << "<Cmd>dns proxy set ip " << sgDnsproxy.ip << "</Cmd>\n";
	contents << "<Cmd>dns proxy set lip " << sgDnsproxy.lIp << "</Cmd>\n";
	contents << "<Cmd>dns proxy set dev " << sgDnsproxy.dev << "</Cmd>\n";
	contents << "<Cmd>dns proxy global " << sgDnsproxy.global << "</Cmd>\n";
	contents << "<Cmd>dns proxy " << sgDnsproxy.status << "</Cmd>\n";

	rslt = contents;
	
	return 0;
}


int 
AosCli::loadDnsproxyConfig(OmnString &rslt)
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

	OmnString contents, cmd, dnsproxyPrefix, subPrefix;
	OmnString start = "------AosCliBlock: ";
	start << "dns proxy" << "------\n";
	OmnString end = "------EndAosCliBlock------\n";
	bool err;

	if (!mFile->readBlock(start, end, contents, err))
	{
		if (err)
		{
			rslt << "********** Failed to read config for: ";
			rslt << "dnsproxy" << "\n";
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
				curPos = cmd.getWord(curPos,dnsproxyPrefix);
				if (dnsproxyPrefix == "dns")
				{
					curPos = cmd.getWord(curPos,subPrefix);
					if(subPrefix == "proxy")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "set")
						{
							curPos = cmd.getWord(curPos, subPrefix);
							if(subPrefix == "name")
							{
								curPos = cmd.getWord(curPos, sgDnsproxy.name);
								continue;
							}
							else if(subPrefix == "ip")
							{
								curPos = cmd.getWord(curPos, sgDnsproxy.ip);
								continue;
							}
							else if(subPrefix == "lip")
							{
								curPos = cmd.getWord(curPos, sgDnsproxy.lIp);
								continue;
							}
							else if(subPrefix == "dev")
							{
								curPos = cmd.getWord(curPos, sgDnsproxy.dev);
								continue;
							}
							else
							{
								cout << "Error:The Dnsproxy SubPrefix is wrong!" << endl;
								continue;
							}
						}
						else if(subPrefix == "start")
						{	
							curPos = cmd.getWord(curPos, sgDnsproxy.status);
							dnsproxyStart(rslt);
						}
                        else if(subPrefix == "global")
						{	
							curPos = cmd.getWord(curPos, sgDnsproxy.global);
						}
						else if(subPrefix == "stop")
						{	
							curPos = cmd.getWord(curPos, sgDnsproxy.status);
							dnsproxyStop(rslt);
						}
						else 
						{
							cout << "Error:The Dnsproxy SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else 
					{
						cout << "Error:The Dnsproxy SubPrefix is wrong!" << endl;
						continue;
					}
				}
				else
				{	
					cout << "Error:This is a bad dnsproxy command!" << endl;
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
			<< "dnsproxy" << endl;
		return -eAosAlarm_FailedToLoadConfig;
	}

	mFile->closeFile();
	mFile = 0;

	return 0;
}
#endif



