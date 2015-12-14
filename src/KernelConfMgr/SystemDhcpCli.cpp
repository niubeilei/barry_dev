////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemDhcpCli.cpp
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
//#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include <string.h>

#include "Global.h"

#define RHC_DHCP_SERVER_CONFIG "/usr/local/rhc/config/dhcp.conf"

struct dhcpIpBindEntry dhcpIpBind;
struct aos_list_head sgDhcpIpBindList;
struct dhcpConfigEntry sgDhcp;

int dhcpInit()
{
	sgDhcp.dft_leaseTime = 600;
	sgDhcp.ip1 = "";
	sgDhcp.ip2 = "";
	sgDhcp.dns = "";
	sgDhcp.router = "";
	sgDhcp.status = "stop";

	AOS_INIT_LIST_HEAD(&sgDhcpIpBindList);
    
	return 0;
}

int dhcpServerDnsSet(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;

	sgDhcp.dns = parms->mStrings[0];

	*optlen = index;
	return 0;
}

int dhcpServerLeaseTimeSet(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;

	sgDhcp.dft_leaseTime = parms->mIntegers[0];

	*optlen = index;
	return 0;
}

int dhcpServerRouterSet(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	sgDhcp.router = parms->mStrings[0];

	*optlen = index;
	return 0;
}

int dhcpServerIpBlockSet(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;

	sgDhcp.ip1 = parms->mStrings[0];
	sgDhcp.ip2 = parms->mStrings[1];

	*optlen = index;
	return 0;
}

int dhcpServerAddBind(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	
	unsigned int index = 0;
	OmnString rslt;
	char local[256];
		
	struct dhcpIpBindEntry * ptr;

    aos_list_for_each_entry(ptr, &sgDhcpIpBindList, datalist)
    {       
    	if(ptr->alias == parms->mStrings[0]) 
   		{       
			strcpy(local,rslt);
	    	rslt = "Error: The alias already exists !";
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -1;
      	}            
    }

	struct dhcpIpBindEntry * bind;
	if((bind = (struct dhcpIpBindEntry *)aos_malloc(sizeof(struct dhcpIpBindEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(bind, 0, sizeof(struct dhcpIpBindEntry));
	bind->alias = parms->mStrings[0];
	bind->mac = parms->mStrings[1];
	bind->ip = parms->mStrings[2];
	aos_list_add_tail((struct aos_list_head *)&bind->datalist, &sgDhcpIpBindList);
	
	*optlen = index;
	return 0;
}

int dhcpServerDelBind(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	int existflag = 0;
	OmnString rslt;
	
	struct dhcpIpBindEntry * ptr;
	struct dhcpIpBindEntry * tmp;

   	aos_list_for_each_entry_safe(ptr, tmp, &sgDhcpIpBindList, datalist)
    {       
    	if( ptr->alias == parms->mStrings[0] ) 
   		{       
        	aos_list_del(&ptr->datalist);
    	    aos_free(ptr);
	        existflag = 1;
			break;
      	}            
    }
  
  	if ( existflag != 1 )
	{
        rslt =  "Error: The alias does not exist !";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}

	*optlen = index;

	return 0;
}

int dhcpServerShowConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	//char dns[20];
	char local[2048];
	OmnString contents;
	OmnString maclist;

	struct dhcpIpBindEntry * ptr;

	contents << "DHCP Server information\n"
		     << "----------------------------\n"
		     << "      status : " << sgDhcp.status << "\n" 
			 << "  lease time : " << sgDhcp.dft_leaseTime << "\n"
		     << "      router : " << sgDhcp.router << "\n"
			 << "         dns : " << sgDhcp.dns << "\n"
		     << "    ip block : " << sgDhcp.ip1 << " " << sgDhcp.ip2 << "\n"
			 << "   bind list : \n" ;

	aos_list_for_each_entry(ptr, &sgDhcpIpBindList, datalist)
	{
		maclist << "\t\t\t" << ptr->alias << " " << ptr->mac << " " << ptr->ip << "\n";
	}

	contents << "\t\t\talias MAC IP\n\t\t\t--------------\n" << maclist;

	strcpy(local, contents.data());

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;

	return 0;
}

int dhcpServerStart(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	
	if ((sgDhcp.ip1 == "") || (sgDhcp.ip2 == "")  || (sgDhcp.dns == "")
		|| (sgDhcp.router == ""))
	{
		rslt = "The DHCP server config has not been set well! Sorry, can not to run!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	
	OmnFilePtr dhcpFile = OmnNew OmnFile("../rhcConf/dhcpd.conf.template",OmnFile::eReadOnly);
	OmnFilePtr tmpFile = OmnNew OmnFile("../rhcConf/dhcpd_tmp.conf",OmnFile::eCreate);
	
	if(!dhcpFile)
	{
		rslt = "Failed to find configure file:";
		rslt << "../rhcConf/dhcpd.conf.template";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return eAosAlarm_FailedToOpenFile;
	}

	if(!dhcpFile->isGood())
	{
		rslt = "Failed to find configure file:";
		rslt << "../rhcConf/dhcpd.conf.template";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return eAosAlarm_FailedToOpenFile;
	}
	
	if(!tmpFile)
	{
		rslt = "Failed to find configure file:";
		rslt << "../rhcConf/dhcpd_tmp.conf";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return eAosAlarm_FailedToOpenFile;
	}

	if(!dhcpFile->isGood())
	{
		rslt = "Failed to find configure file:";
		rslt << "../rhcConf/dhcpd_tmp.conf";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
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
	renameCmd << "/bin/mv -f ../rhcConf/dhcpd_tmp.conf /etc/dhcp/dhcpd.conf";
	OmnCliSysCmd::doShell(renameCmd, rslt);
	
	OmnString systemCmd;
//	systemCmd << "/etc/init.d/dhcpd restart";       //for Gentoo pc
	systemCmd << "/etc/init.d/dhcp restart";        //for box
	OmnCliSysCmd::doShell(systemCmd, rslt);

	sgDhcp.status = "start";
	
	*optlen = index;
	return 0;
}

// dxr, 11/08/2006
int dhcpServerStop(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString systemCmd;
	OmnString rslt;
//	systemCmd << "/etc/init.d/dhcpd stop";           //for Gentoo pc
	systemCmd << "/etc/init.d/dhcp stop";            //for box
	OmnCliSysCmd::doShell(systemCmd, rslt);
	
	sgDhcp.status = "stop";
	
	*optlen = index;
	return 0;
}

// dxr, 11/23/2006
int dhcpClientGetIp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;
	OmnString systemCmd;
	char local[2048];
//	systemCmd << "/sbin/dhcpcd ";        //for Gentoo PC start dhcp client
	systemCmd << "/sbin/dhclient ";        //for ROS box start dhcp client

	OmnString devName;
	devName = parms->mStrings[0];
	
	//if(!strcmp(parms->mStrings[0], "incard"))
	if(devName == "incard")
	{
		systemCmd << "br0";
	}
	//if(!strcmp(parms->mStrings[0], "outcard"))
	else
	{
		systemCmd << "eth0";
	}
	
	OmnCliSysCmd::doShell(systemCmd, rslt);
	strcpy(local, rslt.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	
	*optlen = index;
	return 0;
}

// dxr, 11/23/2006
int dhcpClientShowIp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;
	char local[4096];
	OmnString systemCmd;
	systemCmd << "/sbin/ifconfig ";

	OmnString devName;
	devName = parms->mStrings[0];
	if(devName == "incard")
	{
		systemCmd << "br0";
	}
	if(devName == "outcard")
	{
		systemCmd << "eth0";
	}
	
	OmnCliSysCmd::doShell(systemCmd, rslt);
	strcpy(local, rslt.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	
	*optlen = index;
	return 0;
}


int saveDhcpServerConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{	
	/*unsigned int index = 0;
	OmnString rslt, fn;
	OmnFilePtr mFile;
	
	fn = RHC_DHCP_SERVER_CONFIG;
	
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

	//sgDhcp.ip1 = parms->mStrings[0];
	//sgDhcp.ip2 = parms->mStrings[1];

	//strcpy(local, sgDhcp.dns.getBuffer());
	//CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	//optlen = index;
	//return 0;

	OmnString contents;
	int ret;
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
	
	char local[1000];
	sprintf(local, "------AosCliBlock: dhcp server------\n");
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
		OmnTrace << "To run command: " << cmd << endl;
		system(cmd);
	}

	mFile->closeFile();
	mFile = 0;

	cmd = "mv -f ";

	cmd << fn << ".active " << fn;
	system(cmd);
	OmnTrace << "To run command: " << cmd << endl;
	
	strcpy(local, "DHCP Server's config save ok!");
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
*/
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString contents;
	char local[4096];
	struct dhcpIpBindEntry * ptr;
	
	contents = "<Cmd>";
	contents << "dhcp server lease time set "<< sgDhcp.dft_leaseTime;
	contents << "</Cmd>\n";
	contents << "<Cmd>";
	contents << "dhcp server "<< sgDhcp.status;
	contents << "</Cmd>\n";
	if ( (sgDhcp.ip1 != "") && (sgDhcp.ip2 != ""))
	{
		contents << "<Cmd>";
		contents << "dhcp server ip block set "<< sgDhcp.ip1 \
			 << " " << "sgDhcp.ip2";
		contents << "</Cmd>\n";
	}
	if (sgDhcp.dns != "")
	{
		contents << "<Cmd>";
		contents << "dhcp server dns set "<< sgDhcp.dns;
		contents << "</Cmd>\n";	
	}
	
	if (sgDhcp.router != "")
	{
		contents << "<Cmd>";
		contents << "dhcp server router set "<< sgDhcp.router;
		contents << "</Cmd>\n";	
	}
	
	aos_list_for_each_entry(ptr, &sgDhcpIpBindList, datalist)
	{
		if (ptr->alias != "")
		contents << "<Cmd>";
		contents << "dhcp server add bind "<< ptr->alias << " " << ptr->mac
			 << " " << ptr->ip;
		contents << "</Cmd>\n";
		
	}

	strcpy(local, contents.data());
	
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	return 0;
}


int loadDhcpServerConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	// Clear the old config
	struct dhcpIpBindEntry * ptr;
	struct dhcpIpBindEntry * tmp;
	OmnFilePtr mFile;
	OmnString rslt;
	OmnString fn;

	fn = RHC_DHCP_SERVER_CONFIG;

   	aos_list_for_each_entry_safe(ptr, tmp, &sgDhcpIpBindList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        aos_free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgDhcpIpBindList);
	

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
									OmnString tmp;
									sprintf(tmp.getBuffer(), "%d", sgDhcp.dft_leaseTime); 
									curPos = cmd.getWord(curPos,tmp);
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
				//return -eAosAlarm_FailedToLoadConfig;
				return -1;
			}
		}
	}

	catch (const OmnExcept &e)
	{
		cout << "Failed to load configure for module: " 
			<< "dhcp server" << endl;
		//return -eAosAlarm_FailedToLoadConfig;
		return -1;
	}

	mFile->closeFile();
	mFile = 0;
	
	char* buf = CliUtil_getBuff(data);
	char local[1000];
	strcpy(local, "DHCP Server's config load ok!");
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;
	return 0;
}

extern int DhcpServerClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	struct dhcpIpBindEntry * ptr;
	struct dhcpIpBindEntry * tmp;

   	aos_list_for_each_entry_safe(ptr, tmp, &sgDhcpIpBindList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        aos_free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgDhcpIpBindList);

	dhcpInit();

	*optlen = index;
	return 0;
}

int Dhcp_regCliCmd(void)
{	
	int ret;
	
	ret = CliUtil_regCliCmd("dhcp server dns set", dhcpServerDnsSet);
	ret |= CliUtil_regCliCmd("dhcp server router set", dhcpServerRouterSet);
	ret |= CliUtil_regCliCmd("dhcp server lease time set", dhcpServerLeaseTimeSet);
	ret |= CliUtil_regCliCmd("dhcp server ip block", dhcpServerIpBlockSet);
	ret |= CliUtil_regCliCmd("dhcp server add bind", dhcpServerAddBind);
	ret |= CliUtil_regCliCmd("dhcp server del bind", dhcpServerDelBind);
	ret |= CliUtil_regCliCmd("dhcp server stop", dhcpServerStop);
	ret |= CliUtil_regCliCmd("dhcp server start", dhcpServerStart);
	ret |= CliUtil_regCliCmd("dhcp server show config", dhcpServerShowConfig);
	ret |= CliUtil_regCliCmd("dhcp server save config", saveDhcpServerConfig);
	ret |= CliUtil_regCliCmd("dhcp server clear config", DhcpServerClearConfig);
	ret |= CliUtil_regCliCmd("dhcp server load config", loadDhcpServerConfig);	
	ret |= CliUtil_regCliCmd("dhcp client show", dhcpClientShowIp);
	ret |= CliUtil_regCliCmd("dhcp client get", dhcpClientGetIp);

	return ret;
}
