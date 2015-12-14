
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SyatemVlan.cpp
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
#include "Util/IpAddr.h"
//#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"
#include <string.h>
#define RHC_VLAN_CONFIG "/usr/local/rhc/config/vlan.conf"
#define VLAN "eth1"
#include "Global.h"



struct aos_list_head sgVlanConfigList;
struct vlanSwitchEntry sgVlanSwitch;


//initialize variable
int vlanInit()
{
	AOS_INIT_LIST_HEAD(&sgVlanConfigList);
	sgVlanSwitch.status = "off";
	sgVlanSwitch.reset = "no";
	sgVlanSwitch.port = "";
	sgVlanSwitch.vlanName = "";
	return 0;
}

int vlanDeviceAdd(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char rslt[64];
	OmnString tmpDev;
	OmnString cmd;
	OmnString rslt1;
	int vlanId;
	OmnString tmpVlanId;
	char id[6];
	//get the parms
	tmpDev = parms->mStrings[0];
	vlanId = parms->mIntegers[0];
	if(tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	else if(tmpDev == "incard")
	{
		tmpDev = "br0";
	}
	else
	{
		rslt1="Interface only be outcard or incard";
		strncpy(errmsg,rslt1.data(), errlen-1);
		errmsg[errlen-1] = 0;
		return -1;

	}
	if (vlanId < 1 || vlanId > 4095)
	{
		rslt1="vlan id is between 1 to 4095!";
		strncpy(errmsg,rslt1.data(), errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidVlanTag;

	}
	sprintf(id,"%d",vlanId);
	tmpVlanId = id;
	struct vlanConfigEntry * ptr;
	struct vlanConfigEntry * tmp;

	aos_list_for_each_entry(ptr, &sgVlanConfigList, datalist)
	{
		if(ptr->dev == tmpDev)	
		{
			if (ptr->id == tmpVlanId)
			{
				sprintf(rslt,"Error: vlan is exist!");
				strncpy(errmsg,rslt,errlen-1);
				errmsg[errlen-1] = 0;
				return -eAosRc_VlanTagAlreadyUsed;
			}
		}	

	}
	if((tmp = new struct vlanConfigEntry())==NULL) 
	{
		return -eAosRc_MallocError;
	}

	tmp->dev=tmpDev;
	tmp->id=tmpVlanId;
	
	aos_list_add_tail((struct aos_list_head *)&tmp->datalist, &sgVlanConfigList);

	cmd << "vconfig add " << tmpDev << " " << tmpVlanId ;
	OmnCliSysCmd::doShell(cmd, rslt1);
	*optlen = index;
	return 0;
}

extern int vlanDeviceDel(OmnString &dev, OmnString&id, OmnString &rslt) 
{
	OmnString cmd;
	cmd << "vconfig rem " << dev << "." << id ;
	OmnCliSysCmd::doShell(cmd, rslt);
	
	return 0;
}

int vlanDeviceDel(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString tmpDev;
	OmnString rslt;
	OmnString cmd;
	OmnString tmpVlanId;
	int existflag = 0;
	//get the parms
	tmpDev = parms->mStrings[0];
	tmpVlanId = parms->mStrings[1];
	if(tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	else if(tmpDev == "incard")
	{
		tmpDev = "br0";
	}
	else
	{
		rslt="Interface only be outcard or incard";
		strncpy(errmsg,rslt.data(), errlen-1);
		errmsg[errlen-1] = 0;
		return -1;

	}

	struct vlanConfigEntry * ptr;

	aos_list_for_each_entry(ptr, &sgVlanConfigList, datalist)
	{
		if(ptr->dev == tmpDev)	
		{
			if (ptr->id == tmpVlanId)
			{
				aos_list_del(&ptr->datalist);
				aos_free(ptr);
				existflag = 1;
				break;
			}
		}	

	}
	if ( existflag != 1 )
	{
		rslt =  "Error:  vlan does not exist !";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_VlanNoExist;
	}
	cmd << "vconfig rem " << tmpDev << "." << tmpVlanId ;
	OmnCliSysCmd::doShell(cmd, rslt);
	*optlen = index;

	return 0;
}


int vlanSetIp(OmnString &dev, OmnString &id, OmnString &ip, OmnString &rslt)
{
	OmnString cmd;
	OmnString setIpCmd;
	struct vlanConfigEntry* data;
	if((data = new struct vlanConfigEntry())==NULL)
	{
		return -eAosRc_MallocError;
	}

	data->dev = dev;
	data->id = id;
	data->ip = ip;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgVlanConfigList);

	cmd << "vconfig add " << dev << " " << id ;
	OmnCliSysCmd::doShell(cmd, rslt);

	setIpCmd << "ifconfig " << dev<< "." << id  << " " << ip << " up";
	OmnCliSysCmd::doShell(setIpCmd, rslt);
	return 0;
}


int vlanSetIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString tmpDev;
	OmnString cmd;
	OmnString rslt;
	OmnString tmpVlanId;
	int existflag =0;
	int tmpip;
	//OmnString tmpIp;
	char ipStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;

    tmpip = parms->mIntegers[0];
	tmpaddr.s_addr = tmpip;
	strcpy(ipStr, inet_ntoa(tmpaddr));
		
	//get the parms
	tmpDev = parms->mStrings[0];
	tmpVlanId = parms->mStrings[1];
	if(tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	else if(tmpDev == "incard")
	{
		tmpDev = "br0";
	}
	else
	{
		rslt="Interface only be outcard or incard";
		strncpy(errmsg,rslt.data(), errlen-1);
		errmsg[errlen-1] = 0;
		return -1;

	}

	struct vlanConfigEntry * ptr;

	aos_list_for_each_entry(ptr, &sgVlanConfigList, datalist)
	{
		if(ptr->dev == tmpDev)	
		{
			if (ptr->id == tmpVlanId)
			{
				ptr->ip = ipStr;

				existflag = 1;
				break;
			}
		}	

	}
	if ( existflag != 1 )
	{
		rslt =  "Error:  vlan does not exist !";
		strncpy(errmsg, rslt.data(), errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_VlanNoExist;
	}

	cmd << "ifconfig " << tmpDev<< "." << tmpVlanId  << " " << ptr->ip << " up";
	//cout << cmd << endl;
	OmnCliSysCmd::doShell(cmd, rslt);
	*optlen = index;

	return 0;
}


int vlanClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	struct vlanConfigEntry * ptr;
	struct vlanConfigEntry * tmp;
	OmnString rslt;
	OmnString dev;
	OmnString id;
	OmnString ip;
	OmnString cmd;
	OmnString Resetcmd;


	aos_list_for_each_entry(ptr, &sgVlanConfigList, datalist)
	{   
		dev = ptr->dev;
		id = ptr->id; 
		ip = ptr->ip;
		vlanDeviceDel(dev, id, rslt);
	}  

	aos_list_for_each_entry_safe(ptr, tmp, &sgVlanConfigList, datalist)
	{       
		aos_list_del(&ptr->datalist);
		aos_free(ptr);
	}
	AOS_INIT_LIST_HEAD(&sgVlanConfigList);

	//clear vlan switch config
	Resetcmd << "echo " << "0" << ">" << "/proc/switch/eth1/reset";

	cmd << "echo " << "0" << ">" << "/proc/switch/eth1/enable_vlan";

	OmnCliSysCmd::doShell(cmd, rslt);
	OmnCliSysCmd::doShell(Resetcmd, rslt);
	*optlen = index;

	return 0;

}

int vlanSaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	/*
	 * , fn;
	OmnFilePtr mFile;

	fn = RHC_VLAN_CONFIG;

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

	char* buf = CliUtil_getBuff(data);

	OmnString contents;
	int ret;
	struct vlanConfigEntry * ptr;
	contents << "<Cmd>vlanSwitch " << "status "<< sgVlanSwitch.status << "</Cmd>\n";
	contents << "<Cmd>vlanSwitch " << "isReset "<< sgVlanSwitch.reset << "</Cmd>\n";
	contents << "<Cmd>vlanSwitch " << "set "<< sgVlanSwitch.vlanName <<" "
		<< sgVlanSwitch.port << "</Cmd>\n";
	aos_list_for_each_entry(ptr, &sgVlanConfigList, datalist)
	{
		contents << "<Cmd>vlan ";
		contents << ptr->dev << " ";
		contents << ptr->id << " ";
		contents << ptr->ip << "</Cmd>\n";
	}

	char local[1000];
	sprintf(local, "------AosCliBlock: vlan------\n");
	if (!mFile->put(local, false) ||
			!mFile->put("<Commands>\n", false) ||
			!mFile->put(contents, false) ||
			!mFile->put("</Commands>\n", false) ||
			!mFile->put("------EndAosCliBlock------\n\n", false))
	{
		rslt = "Error: Failed to write to config: ";
		rslt << fn;
		ret = aos_alarm(eAosMD_Platform, eAosAlarm_FailedToWriteConfig,
				"%s", rslt.data());
	}

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
*/	
	char* buf = CliUtil_getBuff(data);
	OmnString contents;
	OmnString vlanlist;
	OmnString vlanIp;
		
	struct vlanConfigEntry * ptr;

	aos_list_for_each_entry(ptr, &sgVlanConfigList, datalist)
	{
		if (ptr->dev != "")
		{
			if (ptr->ip == "")
			{	
				vlanlist << "\t\t\t\t" << ptr->dev << " "<< ptr->id;
				contents <<"<Cmd>";                                   
				contents << "vlan device add " << vlanlist; 
				contents <<"</Cmd>\n"; 
			}
			else
			{
				vlanIp << "\t\t\t\t" << ptr->dev << " "<< ptr->id << " " << ptr->ip;
				contents <<"<Cmd>";                                   
				contents << "vlan set ip " << vlanlist; 
				contents <<"</Cmd>\n"; 
			}
		}
	}

    contents <<"<Cmd>";      
	contents << "vlan switch status " << sgVlanSwitch.status; 
	contents <<"</Cmd>\n"; 
	if ((sgVlanSwitch.vlanName != "") && (sgVlanSwitch.port != ""))
	{
	
		contents << "<Cmd>";
		contents << "vlan switch set " << sgVlanSwitch.vlanName 
				 << " " << sgVlanSwitch.port ;  
		contents << "</Cmd>\n"; 
	}

	contents <<"<Cmd>";      
	contents << "vlan switch reset " << sgVlanSwitch.reset; 
	contents <<"</Cmd>\n"; 

	char local[4096];
	strcpy(local, contents.data());
	
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;

	return 0;
}

int vlanLoadConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	// Clear the old config
	struct vlanConfigEntry * ptr;
	struct vlanConfigEntry * tmp;
	OmnFilePtr mFile;
	OmnString rslt;
	OmnString fn;
	OmnString dev;
	OmnString id;
	OmnString ip;
	OmnString status;
	OmnString isReset;
	OmnString vlanName;
	OmnString ports;

	fn = RHC_VLAN_CONFIG;

	aos_list_for_each_entry(ptr, &sgVlanConfigList, datalist)
	{   
		dev = ptr->dev;
		id = ptr->id; 
		ip = ptr->ip;
		vlanDeviceDel(dev, id, rslt);
	}  

	aos_list_for_each_entry_safe(ptr, tmp, &sgVlanConfigList, datalist)
	{       
		aos_list_del(&ptr->datalist);
		aos_free(ptr);
	}
	AOS_INIT_LIST_HEAD(&sgVlanConfigList);


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
	start << "vlan" << "------\n";
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
				if (prefix == "vlan")
				{

					curPos = cmd.getWord(curPos, dev);
					curPos = cmd.getWord(curPos, id);
					curPos = cmd.getWord(curPos, ip);
					vlanSetIp(dev,id,ip,rslt);
					continue;
				}
				else if (prefix == "vlanSwitch")                                                    			{                                                               
					curPos = cmd.getWord(curPos,subPrefix);                     
					if (subPrefix == "status")                                  
					{                                                           
						curPos = cmd.getWord(curPos, sgVlanSwitch.status);      
						if (sgVlanSwitch.status == "on")                        
						{                                                       
							status = "1";                                       
						}                                                       
						else if (sgVlanSwitch.status == "off")                  
						{                                                       
							status = "0";                                       
						}                                                       
						vlanSwitchStatus(status, rslt);                         
					}                                                           
					else if (subPrefix == "isReset")                            
					{                                                           
						curPos = cmd.getWord(curPos, sgVlanSwitch.reset);       
						if (sgVlanSwitch.reset == "yes")                        
						{                                                       
							isReset = "1";                                      
						}                                                       
						else if (sgVlanSwitch.reset == "no")                    
						{                                                       
							isReset = "0";                                      
						}                                                       
						vlanSwitchReset(isReset, rslt);                         
					}                                                           
					else if(subPrefix == "set")                                
					{                                                           
						curPos = cmd.getWord(curPos, sgVlanSwitch.vlanName);    
						vlanName = sgVlanSwitch.vlanName;                       

						curPos = cmd.getWord(curPos, sgVlanSwitch.port);        
						ports = sgVlanSwitch.port;                              

						vlanSwitchSet(vlanName, ports, rslt);                                                    
					}
					else
					{
						rslt << "Error:This is a bad vlan switch set config file!";
						strncpy(errmsg,rslt.data(),errlen-1);
						errmsg[errlen-1] = 0;
						return -1;
						continue;	
					}

				}
				else
				{	
					cout << "Error:This is a bad vlan config file!" << endl;
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
			<< " vlan" << endl;
		//return -eAosAlarm_FailedToLoadConfig;
		return -1;
	}

	mFile->closeFile();
	mFile = 0;

	char* buf = CliUtil_getBuff(data);
	char local[1000];
	strcpy(local, "Vlan's config load ok!");
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;

	return 0;
}

int vlanShowConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	//char dns[20];
	char local[2048];
	OmnString contents;
	OmnString vlanlist;

	struct vlanConfigEntry * ptr;

	contents << "vlan information\n"
		<< "----------------------------\n\t"
		<< "      vlan switch status : "  << sgVlanSwitch.status << "\n\t"
		<< "       vlan switch reset : "  << sgVlanSwitch.reset << "\n\t"
		<< "         vlan switch set : \n"  << "\t\t\tVlanId\tPorts\n"
		<< "\t\t\t--------------------\n" << "\t\t\t" << sgVlanSwitch.vlanName 
		<< "\t" << sgVlanSwitch.port << "\n\t"
		<< "               vlan list : \n";

	aos_list_for_each_entry(ptr, &sgVlanConfigList, datalist)
	{
		vlanlist << "\t\t\t\t" << ptr->dev << "\t"<< ptr->id << "\t" << ptr->ip << "\n";
	}

	contents << "\t\t\tDev\tId\tIp\n"
		<<	"\t\t\t---------------------------------\n" << vlanlist;

	strcpy(local, contents.data());

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;

	return 0;
}

extern int vlanSwitchStatus(OmnString &status, OmnString &rslt)
{
	OmnString cmd;
	cmd << "echo " << status << ">" << "/proc/switch/eth1/enable_vlan";
	//cout << cmd << endl;
	OmnCliSysCmd::doShell(cmd, rslt);
	return 0;

}

extern int vlanSwitchStatus(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString status;
	OmnString cmd;
	OmnString rslt;

	status = parms->mStrings[0];
	sgVlanSwitch.status = status;

	if (status == "on")
	{
		status = "1";
	}
	else if (status == "off")
	{
		status = "0";
	}

	cmd << "echo " << status << ">" << "/proc/switch/eth1/enable_vlan";
	//cout << cmd << endl;
	OmnCliSysCmd::doShell(cmd, rslt);

	*optlen = index;
	return 0;
}

extern int vlanSwitchReset(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString cmd;
	OmnString rslt;

	cmd << "echo " << "1"<< ">" << "/proc/switch/eth1/reset";
	OmnCliSysCmd::doShell(cmd, rslt);
	sgVlanSwitch.reset = "yes";	
	*optlen = index;
	return 0;

}

extern int vlanSwitchReset(OmnString isReset, OmnString &rslt)
{
	OmnString cmd;
	cmd << "echo " << isReset << ">" << "/proc/switch/eth1/reset";
	//cout << cmd << endl;
	OmnCliSysCmd::doShell(cmd, rslt);
	return 0;

}

extern int vlanSwitchSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString vlanName;
	OmnString vlanfile;
	OmnString port;
	int ports[5];
	OmnString cmd;
	OmnString rslt;
	int i, j;
	int len_port;
	int len_name;
	char name[2];
	char portParms[5];
#define ERRMSG(errmsg, errlen, rslt) \
	strncpy(errmsg,rslt.data(),errlen-1); \
	errmsg[errlen-1] = 0; \
	return -1; \

	//get the parms
	sgVlanSwitch.vlanName = parms->mStrings[0];
	sgVlanSwitch.port = parms->mStrings[1];
	vlanName = sgVlanSwitch.vlanName;

	len_port = strlen(sgVlanSwitch.port.data());
	len_name = strlen(sgVlanSwitch.vlanName.data());

	//judge the parms vlanname' length is two. ports's length is five.
	if ((len_port > 6)||(len_name > 2))
	{
		rslt = "You input a wrong parameters!";
		ERRMSG(errmsg, errlen, rslt)
	}

	//judge the vlanname valid
	strncpy(name, vlanName.data(), len_name);

	int z = int (name[0]);
	int k = int (name[1]);
	if ((z != '0')&&(z !='1'))
	{
		rslt = "You input a wrong vlan id!";
		ERRMSG(errmsg, errlen, rslt)

	}
	else if ((k >= '6')&& (k < '0'))
	{
		rslt = "You input a wrong vlan id!";
		ERRMSG(errmsg, errlen, rslt)
	}

	strncpy(portParms,sgVlanSwitch.port.data(), len_port);

	for (i = 0; i < len_port; i++) 
	{
		ports[i] = portParms[i];
		if ( ports[i] < '0' || ports[i] > '6')
		{
			rslt = "You input a wrong parameters!";
			ERRMSG(errmsg, errlen, rslt)
		}
	}
	for (i = 0; i < len_port; i++) 
	{
		for (j = i + 1; j < len_port; j++) 
		{
			if (ports[i] == ports[j]) 
			{
				rslt = "You input a wrong parameters!";
				ERRMSG(errmsg, errlen, rslt)
			}
		} 
	}
	for (i = 0; i < len_port; i++) 
	{
		port<<portParms[i]<<"\t";
	}

	vlanfile << "/proc/switch/eth1/vlan/" << VLAN << "." << vlanName << "/ports";
	cmd << "echo " << port << ">" << vlanfile;

	//cout << cmd << endl;
	OmnCliSysCmd::doShell(cmd, rslt);
	*optlen = index;
	return 0;
}

extern int vlanSwitchSet(OmnString &vlanName, OmnString &ports, OmnString &rslt)
{
	OmnString vlanfile;
	OmnString port;
	int portsName[5];
	OmnString cmd;
	OmnString rslt1;
	int i, j;
	int len_port;
	int len_name;
	char name[2];
	char portParms[5];
	//get the parms

	len_port = strlen(ports.data());
	len_name = strlen(vlanName.data());

	//judge the parms vlanname' length is two. ports's length is five.
	if ((len_port > 6)||(len_name > 2))
	{
		rslt = "You input a wrong parameters!";
		return -eAosRc_InvalidParm;
	}

	//judge the vlanname valid
	strncpy(name, vlanName.data(), len_name);

	int z = int (name[0]);
	int k = int (name[1]);
	if ((z != '0')&&(z !='1'))
	{
		rslt1 = "You input a wrong vlan id!";
		return -eAosRc_InvalidVlanTag;

	}
	else if ((k >= '6')&& (k < '0'))
	{
		rslt1 = "You input a wrong vlan id!";
		return -eAosRc_InvalidVlanTag;
	}

	strncpy(portParms,ports.data(), len_port);

	for (i = 0; i < len_port; i++) 
	{
		portsName[i] = portParms[i];
		if ( portsName[i] < '0' || portsName[i] > '6')
		{
			rslt1 = "You input a wrong parameters!";
			return -eAosRc_InvalidParm;
		}
	}
	for (i = 0; i < len_port; i++) 
	{
		for (j = i + 1; j < len_port; j++) 
		{
			if (portsName[i] == portsName[j]) 
			{
				rslt1 = "You input a wrong parameters!";
				return -eAosRc_InvalidParm;
			}
		} 
	}
	for (i = 0; i < len_port; i++) 
	{
		port<<portParms[i]<<"\t";
	}

	vlanfile << "/proc/switch/eth1/vlan/" << VLAN << "." << vlanName << "/ports";
	cmd << "echo " << port << ">" << vlanfile;

	//cout << cmd << endl;
	OmnCliSysCmd::doShell(cmd, rslt1);
	return 0;	
}

int vlanDeviceDelAllTag(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	struct vlanConfigEntry * ptr;
	struct vlanConfigEntry * tmp;
	OmnString rslt;
	OmnString dev;
	OmnString id;
	OmnString ip;
	OmnString cmd;
	
	dev = parms->mStrings[0];
	if(dev == "outcard")
	{
		dev = "eth0";
	}
	else if(dev == "incard")
	{
		dev = "br0";
	}
	else
	{
		rslt ="Interface only be outcard or incard";
		strncpy(errmsg,rslt.data(), errlen-1);
		errmsg[errlen-1] = 0;
		return -1;

	}

	aos_list_for_each_entry_safe(ptr, tmp, &sgVlanConfigList, datalist)
	{ 
		if (ptr->dev == dev)
		{
			id = ptr->id; 
			ip = ptr->ip;
			vlanDeviceDel(dev, id, rslt);
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
		}
	}

	*optlen = index;
	return 0;
}

int vlanDeviceDelAllIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	struct vlanConfigEntry * ptr;
	struct vlanConfigEntry * tmp;
	OmnString rslt;
	OmnString dev;
	OmnString id;
	OmnString ip;
	OmnString cmd;
	ip = "0";	
	dev = parms->mStrings[0];
	if(dev == "outcard")
	{
		dev = "eth0";
	}
	else if(dev == "incard")
	{
		dev = "br0";
	}
	else
	{
		rslt = "Interface only be outcard or incard";
		strncpy(errmsg,rslt.data(), errlen-1);
		errmsg[errlen-1] = 0;
		return -1;

	}

	aos_list_for_each_entry_safe(ptr, tmp, &sgVlanConfigList, datalist)
	{ 
		if (ptr->dev == dev)
		{
			id = ptr->id; 
			ptr->ip = "";
			cmd << "ifconfig " << dev<< "." << id  << " " << ip << " up";
			OmnCliSysCmd::doShell(cmd, rslt);

		}
	}

	*optlen = index;
	return 0;
}


extern int vlanDelIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString tmpDev;
	OmnString tmpIp;
	OmnString cmd;
	OmnString rslt;
	OmnString tmpVlanId;
	int existflag =0;
	tmpIp = "0";
	//get the parms
	tmpDev = parms->mStrings[0];
	tmpVlanId = parms->mStrings[1];
	if(tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	else if(tmpDev == "incard")
	{
		tmpDev = "br0";
	}
	else
	{
		rslt="Interface only be outcard or incard";
		strncpy(errmsg,rslt.data(), errlen-1);
		errmsg[errlen-1] = 0;
		return -1;

	}

	struct vlanConfigEntry * ptr;

	aos_list_for_each_entry(ptr, &sgVlanConfigList, datalist)
	{
		if(ptr->dev == tmpDev)	
		{
			if (ptr->id == tmpVlanId)
			{
				ptr->ip = "";

				existflag = 1;
				break;
			}
		}	

	}
	if ( existflag != 1 )
	{
		rslt =  "Error:  vlan does not exist !";
		strncpy(errmsg, rslt.data(), errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_VlanNoExist;
	}

	cmd << "ifconfig " << tmpDev<< "." << tmpVlanId  << " " << tmpIp << " up";
	//cout << cmd << endl;
	OmnCliSysCmd::doShell(cmd, rslt);
	*optlen = index;

	return 0;
}



int Vlan_regCliCmd(void)
{
	int ret;
	ret = CliUtil_regCliCmd("vlan device add", vlanDeviceAdd);
	ret |= CliUtil_regCliCmd("vlan device del", vlanDeviceDel);
	ret |= CliUtil_regCliCmd("vlan device del all_tag", vlanDeviceDelAllTag);
	ret |= CliUtil_regCliCmd("vlan set ip", vlanSetIp);
	ret |= CliUtil_regCliCmd("vlan device alltag del all_ip",
							vlanDeviceDelAllIp);
	ret |= CliUtil_regCliCmd("vlan del ip", vlanDelIp);
	ret |= CliUtil_regCliCmd("vlan load config", vlanLoadConfig);
	ret |= CliUtil_regCliCmd("vlan save config", vlanSaveConfig);
	ret |= CliUtil_regCliCmd("vlan show config", vlanShowConfig);
	ret |= CliUtil_regCliCmd("vlan clear config", vlanClearConfig);
	ret |= CliUtil_regCliCmd("vlan switch status", vlanSwitchStatus);
	ret |= CliUtil_regCliCmd("vlan switch reset", vlanSwitchReset);
	ret |= CliUtil_regCliCmd("vlan switch set", vlanSwitchSet);
	return ret;	
}

