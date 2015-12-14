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

extern struct dhcpConfigEntry sgDhcp;

// dxr, 11/08/2006
int 
AosCli::dhcpStart(OmnString &rslt)
{
	if ((sgDhcp.ip1 == "") || (sgDhcp.ip2 == "") || (sgDhcp.dft_leaseTime == "") || (sgDhcp.dns == "")
		|| (sgDhcp.router == ""))
	{
		cout << "The DHCP server config has not been set well! Sorry, can not to run!" << endl;
		return -1;
	}
	
	OmnFilePtr dhcpFile = OmnNew OmnFile("../rhcConf/dhcpd.conf.template",OmnFile::eReadOnly);
	OmnFilePtr tmpFile = OmnNew OmnFile("../rhcConf/dhcpd_tmp.conf",OmnFile::eCreate);
	
	if(!dhcpFile)
	{
		rslt = "Failed to find configure file:";
		rslt << "../rhcConf/dhcpd.conf.template";
		return eAosAlarm_FailedToOpenFile;
	}

	if(!dhcpFile->isGood())
	{
		rslt = "Failed to find configure file:";
		rslt << "../rhcConf/dhcpd.conf.template";
		return eAosAlarm_FailedToOpenFile;
	}
	
	if(!tmpFile)
	{
		rslt = "Failed to find configure file:";
		rslt << "../rhcConf/dhcpd_tmp.conf";
		return eAosAlarm_FailedToOpenFile;
	}

	if(!dhcpFile->isGood())
	{
		rslt = "Failed to find configure file:";
		rslt << "../rhcConf/dhcpd_tmp.conf";
		return eAosAlarm_FailedToOpenFile;
	}

	bool isFileFinished = false;
	OmnString line1 = dhcpFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString word1;
		int curPos = 0;
		curPos = line1.getWord(curPos,word1);

		if(word1 == "range")
		{
			OmnString newLine = "range ";
			newLine << sgDhcp.ip1 << " " << sgDhcp.ip2 << ";\n";
			tmpFile->append(newLine);
		}	
		
		else if(word1 == "option")
		{
			curPos = line1.getWord(curPos,word1);
			if(word1 ==	"domain-name-servers")
			{
				OmnString newLine = "option domain-name-servers ";
				newLine << sgDhcp.dns << ";\n";
				tmpFile->append(newLine);
			}
			if(word1 == "routers")
			{
				OmnString newLine = "option routers ";
				newLine << sgDhcp.router << ";\n";
				tmpFile->append(newLine);
			}
			if(word1 == "subnet-mask")
			{
				line1 << "\n";
				tmpFile->append(line1);
			}
		}	

		else if(word1 == "default-lease-time")
		{
			OmnString newLine = "default-lease-time ";
			newLine << sgDhcp.dft_leaseTime << ";\n";
			tmpFile->append(newLine);
		
		}	
		
		else
		{
			line1 << "\n";
			tmpFile->append(line1);
		}
		line1 = dhcpFile->getLine(isFileFinished);
		
		if(word1 =="}")
		{
			// write the host part
			struct dhcpIpBindEntry * ptr;
			OmnString newLine2 = 0;
			aos_list_for_each_entry(ptr, &sgDhcpIpBindList, datalist)
			{
				newLine2 << "host " << ptr->alias << " {\n";
				newLine2 << "\thardware ethernet " << ptr->mac << ";\n\t" << "fixed-address " << ptr->ip << ";\n}\n";
			}
			tmpFile->append(newLine2);
			break;
		}
	}	
	
	dhcpFile->closeFile();
	tmpFile->closeFile();
	
	OmnString renameCmd;
	renameCmd << "/bin/mv ../rhcConf/dhcpd_tmp.conf /etc/dhcp/dhcpd.conf";
	OmnCliSysCmd::doShell(renameCmd,rslt);
	
	OmnString systemCmd;
	systemCmd << "/etc/init.d/dhcpd restart";
	OmnCliSysCmd::doShell(systemCmd, rslt);

	sgDhcp.status = "start";
	
	return 0;
}

// dxr, 11/08/2006
int 
AosCli::dhcpStop(OmnString &rslt)
{
	OmnString systemCmd;
	
	systemCmd << "/etc/init.d/dhcpd stop";
	OmnCliSysCmd::doShell(systemCmd, rslt);
	
	sgDhcp.status = "stop";
	
	return 0;
}


// dxr, 11/15/2006
int
AosCli::dhcpServerShowConfig(OmnString &rslt)
{
	OmnString contents;
	struct dhcpIpBindEntry * ptr;

	cout << "DHCP Server information\n" << "----------------------------\n\t    status : " \
			<< sgDhcp.status << "\n\t" << "lease time : " << sgDhcp.dft_leaseTime \
			<< "\n\t    router : " << sgDhcp.router << "\n\t       dns : " << sgDhcp.dns \
			<< "\n\t  ip block : " << sgDhcp.ip1 << " " << sgDhcp.ip2 \
			<< "\n\t bind list : \n";
	
	aos_list_for_each_entry(ptr, &sgDhcpIpBindList, datalist)
	{
		contents << "\t\t\t" << ptr->alias << " " << ptr->mac << " " << ptr->ip << "\n";
	}
	cout << "\t\t\talias MAC IP\n\t\t\t--------------\n" << contents << endl;

	return 0;
}

int
AosCli::dhcpServerRouterSet(OmnString &rslt)
{
	int curPos = 0;
	curPos = mFileName.getWord(curPos, sgDhcp.router);
	if(mFileName == "" || sgDhcp.router == "")
	{
		rslt = "invalid value";
		return false;
	}
	return 0;
}
	
int
AosCli::dhcpServerDnsSet(OmnString &rslt)
{
	int curPos = 0;
	curPos = mFileName.getWord(curPos, sgDhcp.dns);
	if(mFileName =="" || sgDhcp.dns == "")
	{
		rslt = "invalid value";
		return false;
	}

	return 0;

}

int
AosCli::dhcpServerLeaseTimeSet(OmnString &rslt)
{
	int curPos = 0;
	curPos = mFileName.getWord(curPos, sgDhcp.dft_leaseTime);
	if(sgDhcp.dft_leaseTime == "")
	{
		rslt = "invalid value";
		return false;
	}
	return 0;
}

int
AosCli::dhcpServerIpBlock(OmnString &rslt)
{
	int curPos = 0;
	curPos = mFileName.getWord(curPos, sgDhcp.ip1);
	curPos = mFileName.getWord(curPos, sgDhcp.ip2);

	if(mFileName == "" || sgDhcp.ip1 == "" || sgDhcp.ip2== "" )
	{
		rslt = "invalid value";
		return false;
	}
	return 0;

}

int
AosCli::dhcpServerAddBind(OmnString &rslt)
{

	int curPos1 = 0;
	OmnString tmpAlias;
	OmnString tmpMAC;
	OmnString tmpIP;
	
	curPos1 = mFileName.getWord(curPos1, tmpAlias);
	curPos1 = mFileName.getWord(curPos1, tmpMAC);
	curPos1 = mFileName.getWord(curPos1, tmpIP);
	if(tmpAlias == "" ||
	   tmpMAC == "" ||
	   tmpIP == "")
	{
		rslt = "invalid value";
		return false;
	}
		
	struct dhcpIpBindEntry * ptr;

    aos_list_for_each_entry(ptr, &sgDhcpIpBindList, datalist)
    {       
    	if(ptr->alias == tmpAlias) 
   		{       
        	cout << "Error: The alias already exists !" << endl;
			return -1;
      	}            
    }

	struct dhcpIpBindEntry * data;
	if((data = (struct dhcpIpBindEntry *)aos_malloc(sizeof(struct dhcpIpBindEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct dhcpIpBindEntry));
	data->alias = tmpAlias;
	data->mac = tmpMAC;
	data->ip = tmpIP;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgDhcpIpBindList);
	return 0;
}

int
AosCli::dhcpServerDelBind(OmnString &rslt)
{
	int curPos1 = 0;
	int existflag = 0;
	OmnString tmpAlias;
	
	curPos1 = mFileName.getWord(curPos1,tmpAlias);
	if(tmpAlias == "")
	{
		rslt = "invalid value";
		return false;
	}
	
	struct dhcpIpBindEntry * ptr;
	struct dhcpIpBindEntry * tmp;

   	aos_list_for_each_entry_safe(ptr, tmp, &sgDhcpIpBindList, datalist)
    {       
    	if(ptr->alias == tmpAlias) 
   		{       
        	aos_list_del(&ptr->datalist);
    	    aos_free(ptr);
	        existflag = 1;
			break;
      	}            
    }
  
  	if (existflag != 1)
	{
        	cout << "Error: The alias does not exist !" << endl;
			return -1;
	}

	return 0;
}

//add by dxr 11/20/06
int 
AosCli::saveOnlyDhcpServerConfig(OmnString &rslt)
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

	saveDhcpServerConfig(rslt);
	contents = rslt;

	char local[1000];
	sprintf(local, "------AosCliBlock: dhcp server------\n");
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
AosCli::saveDhcpServerConfig(OmnString &rslt)
{
	OmnString contents;
	struct dhcpIpBindEntry * ptr;
	
	contents << "<Cmd>dhcp server lease time set " << sgDhcp.dft_leaseTime << "</Cmd>\n";
	contents << "<Cmd>dhcp server router set " << sgDhcp.router << "</Cmd>\n";
	contents << "<Cmd>dhcp server dns set " << sgDhcp.dns << "</Cmd>\n";
	contents << "<Cmd>dhcp server ip block " << sgDhcp.ip1 << " " << sgDhcp.ip2 << "</Cmd>\n";
	aos_list_for_each_entry(ptr, &sgDhcpIpBindList, datalist)
	{
		contents << "<Cmd>dhcp server add bind ";
		contents << ptr->alias << " ";
		contents << ptr->mac << " ";
		contents << ptr->ip << "</Cmd>\n";
	}
	
	contents << "<Cmd>dhcp server " << sgDhcp.status << "</Cmd>\n";
	
	rslt = contents;
	
	return 0;
}


int 
AosCli::loadDhcpServerConfig(OmnString &rslt)
{

	// Clear the old config
	struct dhcpIpBindEntry * ptr;
	struct dhcpIpBindEntry * tmp;

   	aos_list_for_each_entry_safe(ptr, tmp, &sgDhcpIpBindList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        aos_free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgDhcpIpBindList);
	

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

	OmnString contents, cmd, prefix, subPrefix;
	OmnString start = "------AosCliBlock: ";
	start << "dhcp server" << "------\n";
	OmnString end = "------EndAosCliBlock------\n";
	bool err;

	if (!mFile->readBlock(start, end, contents, err))
	{
		if (err)
		{
			rslt << "********** Failed to read config for: ";
			rslt << "dhcp server" << "\n";
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
				if (prefix == "dhcp")
				{
					curPos = cmd.getWord(curPos,prefix);
					
					if (prefix == "server")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "lease")
						{
							curPos = cmd.getWord(curPos,subPrefix);
							if(subPrefix == "time")
							{
								curPos = cmd.getWord(curPos,subPrefix);
								if(subPrefix == "set")
								{
									curPos = cmd.getWord(curPos,sgDhcp.dft_leaseTime);
									continue;
								}
								else 
								{
									cout << "Error:The Dhcp server lease SubPrefix is wrong!" << endl;
									continue;
								}
							}
						}
						else if(subPrefix == "router")
						{	
							curPos = cmd.getWord(curPos,subPrefix);
							if(subPrefix == "set")
							{
								curPos = cmd.getWord(curPos, sgDhcp.router);
								continue;
							}
							else 
							{
								cout << "Error:The Dhcp router SubPrefix is wrong!" << endl;
								continue;
							}
						}
						else if(subPrefix == "dns")
						{
							curPos = cmd.getWord(curPos,subPrefix);
							if(subPrefix == "set")
							{
								curPos = cmd.getWord(curPos, sgDhcp.dns);
								continue;
							}
							else 
							{
								cout << "Error:The Dhcp dns SubPrefix is wrong!" << endl;
								continue;
							}
						}
						else if(subPrefix == "ip")
						{
							curPos = cmd.getWord(curPos,subPrefix);
							if(subPrefix == "block")
							{
								curPos = cmd.getWord(curPos, sgDhcp.ip1);
								curPos = cmd.getWord(curPos, sgDhcp.ip2);
								continue;
							}
							else 
							{
								cout << "Error:The Dhcp server ip SubPrefix is wrong!" << endl;
								continue;
							}
						}
						else if(subPrefix == "add")
						{
							curPos = cmd.getWord(curPos,subPrefix);
							if(subPrefix == "bind")
							{	
								struct dhcpIpBindEntry * data;
								if((data = new struct dhcpIpBindEntry())==NULL)
								{
									return -1;
								}
								
								curPos = cmd.getWord(curPos, data->alias);
								curPos = cmd.getWord(curPos, data->mac);
								curPos = cmd.getWord(curPos, data->ip);
								aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgDhcpIpBindList);
								continue;
							}
							else 
							{
								cout << "Error:The Dhcp bind SubPrefix is wrong!" << endl;
								continue;
							}
						}
					}
					else 
					{
						cout << "Error:The First SubPrefix is wrong!" << endl;
						continue;
					}
				}
				else
				{	
					cout << "Error:This is a bad dhcp server command!" << endl;
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
			<< "dhcp server" << endl;
		return -eAosAlarm_FailedToLoadConfig;
	}

	mFile->closeFile();
	mFile = 0;

	return 0;
}

// dxr, 11/23/2006
int 
AosCli::dhcpClientGetIp(OmnString &rslt)
{
	OmnString systemCmd;
	systemCmd << "/sbin/dhcpcd ";

	OmnString devName;
	int curPos = 0;
	curPos = mFileName.getWord(curPos,devName);

	if(devName == "incard")
	{
		systemCmd << "eth1";
	}
	else if(devName == "outcard")
	{
		systemCmd << "eth0";
	}
	
	OmnCliSysCmd::doShell(systemCmd, rslt);
	return 0;
}

// dxr, 11/23/2006
int 
AosCli::dhcpClientShowIp(OmnString &rslt)
{
	OmnString systemCmd;
	systemCmd << "/sbin/ifconfig ";

	OmnString devName;
	int curPos = 0;
	curPos = mFileName.getWord(curPos,devName);

	if(devName == "incard")
	{
		systemCmd << "eth1";
	}
	else if(devName == "outcard")
	{
		systemCmd << "eth0";
	}
	
	OmnCliSysCmd::doShell(systemCmd, rslt);
	return 0;
}

#endif

