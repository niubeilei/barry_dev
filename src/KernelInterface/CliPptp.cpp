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

//extern struct pptpUserEntry pptpUser;
extern struct pptpConfigEntry sgPptp;
//extern struct aos_list_head sgPptpUserList;

 // GB, 11/16/2006
int 
AosCli::pptpStart(OmnString &rslt)
{
	bool changedFlag1 = false;
    bool changedFlag2 = false;
    bool changedFlag3 = false;
	bool isFileFinished1 = false;
	bool isFileFinished3 = false;
	
	OmnString stopPptpCmd;
	stopPptpCmd << "/etc/init.d/pptpd stop ";
	OmnCliSysCmd::doShell(stopPptpCmd, rslt);
	
	if ((sgPptp.ipLocal == 0) && (sgPptp.ip1 == 0) && (sgPptp.ip2 == 0) \
	   && (sgPptp.ipLocal == "") && (sgPptp.ip1 == "") && (sgPptp.ip2 == ""))
	{
		cout << "The PPtp's config has not been setted well! Sorry, can not to run!" << endl;
		
		return -1;
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
			if(sgPptp.ip2 ="")
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
				if(ptr->ip == "")
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
		return false;
				
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
	return 0;
}


// GB, 11/16/2006
int 
AosCli::pptpStop(OmnString &rslt)
{
	if ( sgPptp.status == "stop")
	{
		cout << "pptp has already been stoped!";
		return -1;
	}
	OmnString systemCmd;
	
	systemCmd << "/etc/init.d/pptpd stop";
	OmnCliSysCmd::doShell(systemCmd, rslt);
	
	sgPptp.status = "stop";
	return 0;
}


// GB, 11/22/2006
int
AosCli::pptpIpLocal(OmnString &rslt)
{
	int curPos = 0;

	curPos = mFileName.getWord(curPos, sgPptp.ipLocal);

	return 0;
}


// GB, 11/22/2006
int
AosCli::pptpIpRange(OmnString &rslt)
{
	int curPos = 0;

	curPos = mFileName.getWord(curPos, sgPptp.ip1);
	curPos = mFileName.getWord(curPos, sgPptp.ip2);

	return 0;
}


int
AosCli::pptpAddUser(OmnString &rslt)
{
	int curPos1 = 0;
	OmnString tmpUser;
	OmnString tmpPassword;
	OmnString tmpIP;
	
	curPos1 = mFileName.getWord(curPos1, tmpUser);
	curPos1 = mFileName.getWord(curPos1, tmpPassword);
	curPos1 = mFileName.getWord(curPos1, tmpIP);
	if(tmpUser == "" ||
	   tmpPassword == "")
	{
		rslt = "invalid value";
		return false;
	}
		
	struct pptpUserEntry * ptr;

    aos_list_for_each_entry(ptr, &sgPptpUserList, datalist)
    {       
    	if(ptr->username == tmpUser) 
   		{       
        	cout << "Error: The user already exists !" << endl;
			return -1;
      	}            
    }

	struct pptpUserEntry * data;
	if((data = (struct pptpUserEntry *)aos_malloc(sizeof(struct pptpUserEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct pptpUserEntry));
	data->username = tmpUser;
	data->password = tmpPassword;
	data->ip = tmpIP;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgPptpUserList);

//cout << "ADD USER OK!" << endl;

   return 0;
}


int
AosCli::pptpDelUser(OmnString &rslt)
{
	int curPos1 = 0;
	int existflag = 0;
	OmnString tmpUser;
	
	curPos1 = mFileName.getWord(curPos1,tmpUser);
	if(tmpUser == "")
	{
		rslt = "invalid value";
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
        	cout << "Error: The username does not exist !" << endl;
			return -1;
	}

	return 0;
}


// GB, 11/22/2006
int
AosCli::pptpShowConfig(OmnString &rslt)
{
	OmnString contents;
	struct pptpUserEntry * ptr;

	cout << "PPTP information\n" << "----------------------------\n\t   status : " \
			<< sgPptp.status << "\n\t" << " local ip : " << sgPptp.ipLocal\
			<< "\n\t ip range : " << sgPptp.ip1 << " " << sgPptp.ip2 \
			<< "\n\tuser list : \n" ;
	
	aos_list_for_each_entry(ptr, &sgPptpUserList, datalist)
	{
		contents << "\t\t\t" << ptr->username << " " << ptr->password << " " << ptr->ip << "\n";
	}
	cout << "\t\t\tusername password ip\n\t\t\t--------------------\n" << contents << endl;

	return 0;
}


//add by GB 11/22/06
int 
AosCli::saveOnlyPptpConfig(OmnString &rslt)
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

	savePptpConfig(rslt);
	contents = rslt;

	char local[200];
	sprintf(local, "------AosCliBlock: pptp------\n");
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
AosCli::savePptpConfig(OmnString &rslt)
{
	OmnString contents;
	
	contents << "<Cmd>pptp local ip " << sgPptp.ipLocal << "</Cmd>\n";
	contents << "<Cmd>pptp ip range " << sgPptp.ip1 << sgPptp.ip2 <<"</Cmd>\n";
	contents << "<Cmd>pptp " << sgPptp.status << "</Cmd>\n";

	struct pptpUserEntry  * ptr; 
	aos_list_for_each_entry(ptr, &sgPptpUserList, datalist)
	{
		contents << "<Cmd>pptp user set ";
		contents << ptr->username << " ";
		contents << ptr->password << " ";
		contents << ptr->ip << "</Cmd>\n";
	}
	
	rslt = contents;
	
	return 0;
}


int 
AosCli::loadPptpConfig(OmnString &rslt)
{
    // Clear the old config
	struct pptpUserEntry * ptr;
	struct pptpUserEntry * tmp;

   	aos_list_for_each_entry_safe(ptr, tmp, &sgPptpUserList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        aos_free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgPptpUserList);
	

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

	OmnString contents, cmd, pptpPrefix, subPrefix;
	OmnString start = "------AosCliBlock: ";
	start << "pptp" << "------\n";
	OmnString end = "------EndAosCliBlock------\n";
	bool err;

	if (!mFile->readBlock(start, end, contents, err))
	{
		if (err)
		{
			rslt << "********** Failed to read config for: ";
			rslt << "pptp" << "\n";
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
				int curPos = 0;
				curPos = cmd.getWord(curPos,pptpPrefix);
				if (pptpPrefix == "pptp")
				{
					curPos = cmd.getWord(curPos,subPrefix);
					if(subPrefix == "local")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "ip")
						{
							curPos = cmd.getWord(curPos, sgPptp.ipLocal);
							continue;
						}
						else 
						{
							cout << "Error:The Pptp local ip SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else if(subPrefix == "ip")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "range")
						{
							curPos = cmd.getWord(curPos, sgPptp.ip1);
							curPos = cmd.getWord(curPos, sgPptp.ip2);
							continue;
						}
						else 
						{
							cout << "Error:The Pptp ip range SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else if(subPrefix == "user")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "set")
							{	
								struct pptpUserEntry * ptr;
							//	if((ptr = (struct pptpUserEntry *)aos_malloc(sizeof(struct pptpUserEntry)))==NULL)
							 if((ptr = new struct pptpUserEntry())==NULL)	
								{
									//printk("malloc error");
									return -1;
								}
							//	printf("%p\n", ptr);
							//	memset(ptr, 0, sizeof(struct pptpUserEntry));
								
							/*	OmnString tmpUser;
								OmnString tmpPassword;
								OmnString tmpIP;
	                         */  
								curPos = cmd.getWord(curPos, ptr->username);
								curPos = cmd.getWord(curPos, ptr->password);
								curPos = cmd.getWord(curPos, ptr->ip);
							/*	
								ptr->username = tmpUser;
								ptr->password = tmpPassword;
								ptr->ip = tmpIP;
							*/	
								aos_list_add_tail((struct aos_list_head *)&ptr->datalist, &sgPptpUserList);
								continue;
							}

						else 
						{
							cout << "Error:The Pptp set user SubPrefix is wrong!" << endl;
							continue;
						}
				    }
					else if(subPrefix == "start")
					{
						curPos = cmd.getWord(curPos, sgPptp.status);
						pptpStart(rslt);
					}
					else if(subPrefix == "stop")
					{
						curPos = cmd.getWord(curPos, sgPptp.status);
						pptpStop(rslt);
					}
				   	else 
					{
						cout << "Error:The Pptp first SubPrefix is wrong!" << endl;
						continue;
					}
				}
				else
				{	
					cout << "Error:This is a bad pptp command!" << endl;
					continue;
				}
			} catch (const OmnExcept &e)
			{
				rslt = "Failed to read command: ";
				rslt << item->toString();
				return -eAosAlarm_FailedToLoadConfig;
			}
		}
		
	} catch (const OmnExcept &e)
	{
		cout << "Failed to load configure for module: " << "pptp" << endl;
		return -eAosAlarm_FailedToLoadConfig;
	}

	mFile->closeFile();
	mFile = 0;

	return 0;
}

#endif

