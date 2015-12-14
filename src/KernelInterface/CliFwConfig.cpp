////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliFwConfig.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef AOS_OLD_CLI

#include "CliFwMisc.h"

extern struct aos_list_head sgFwFilterList;
extern struct aos_list_head sgFwNatList;

int 
AosCli::saveOnlyFwConfig(OmnString &rslt)
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

	saveFwConfig(rslt);
	contents = rslt;

	char local[200];
	sprintf(local, "------AosCliBlock: firewall------\n");
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
AosCli::saveFwConfig(OmnString &rslt)
{
	OmnString contents;
	struct fwFilterListEntry * ptr;
	struct fwNatListEntry * nptr;	
	int filRuleNum = 0;
	int natRuleNum = 0;
	

	aos_list_for_each_entry(ptr, &sgFwFilterList, datalist)
	{
		filRuleNum ++;
		contents << "<Cmd>firewall ";
		if ((ptr->type == "fwIpfilAll") || (ptr->type == "fwIpfilIcmp"))
		{
			contents << "ipfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if (ptr->type == "fwIpfilStateNew")
		{
			contents << "ipfil state new " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if (ptr->type == "fwIpfilStateAck")
		{
			contents << "ipfil state ack " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if ((ptr->type == "fwIpfilTcp") || (ptr->type == "fwIpfilUdp"))
		{
			contents << "ipfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " " << ptr->sport << " ";
			contents << ptr->dip << " " << ptr->dmask << " " << ptr->dport << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if ((ptr->type == "fwMacfilAll") || (ptr->type == "fwMacfilIcmp"))
		{
			contents << "macfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->smac << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if ((ptr->type == "fwMacfilTcp") || (ptr->type == "fwMacfilUdp"))
		{
			contents << "macfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->smac << " " << ptr->sport << " ";
			contents << ptr->dip << " " << ptr->dmask << " " << ptr->dport << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if (ptr->type == "fwAntiSyn")
		{
			contents << "antiattack syn " << ptr->dip << " " << ptr->dport << " " << ptr->rate;
			contents << "</Cmd>\n";
		}
		else if (ptr->type == "fwAntiIcmp")
		{
			contents << "antiattack icmp " << ptr->dip << " " << ptr->rate;
			contents << "</Cmd>\n";
		}
		else if (ptr->type == "fwTimefil")   
        {   
           	contents << "timefil " << ptr->chain << " " << ptr->proto << " ";   
            contents << ptr->sip << " " << ptr->smask << " ";   
            contents << ptr->dip << " " << ptr->dmask << " ";   
            contents << ptr->action << " " << ptr->beginTime<< " ";   
            contents << ptr->endTime << " " << ptr->week << " " ;   
            contents << ptr->log << "</Cmd>\n";   
        } 

		 //printf("the cur index:%d, pack_tick: %d\n",index, pack->pack_tick);
		     //printf("the add timer i=%d   entry+++++++:%x    expires:%d\n", i, &ptr->entry, ptr->expires);
		//
	}
	//cout << "The filter rules num is: " << ruleNum << "and the list is:" << contents << endl;

	aos_list_for_each_entry(nptr, &sgFwNatList, datalist)
	{
		natRuleNum ++;
		contents << "<Cmd>firewall ";
	 	if (nptr->type == "fwNatSnat")
		{
			contents << "<Cmd>firewall snat " << nptr->interface << " ";
			contents << nptr->sip << " " << nptr->smask << " ";
			contents << nptr->to;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "fwNatMasq")
		{
			contents << "<Cmd>firewall masq " << nptr->interface << " ";
			contents << nptr->sip << " " << nptr->smask;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "fwNatDnatIP")
		{
			contents << "<Cmd>firewall dnat ip " << nptr->interface << " ";
			contents << nptr->proto << " " << nptr->sip << " " << nptr->smask << " ";
			contents << nptr->dip;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "fwNatDnatPort")
		{
			contents << "<Cmd>firewall dnat port " << nptr->interface << " ";
			contents << nptr->proto << " " << nptr->sip << " " << nptr->smask << " " << nptr->sport << " ";
			contents << nptr->dip << " " << nptr->dport;
			contents << "</Cmd>\n";
		}	
		else if (nptr->type == "fwNatRedi")
		{
			contents << "<Cmd>firewall redi " << nptr->interface << " ";
			contents << nptr->proto << " " << nptr->fromport << " " << nptr->toport;
			contents << "</Cmd>\n";
		}	
		else if (nptr->type == "webwallRedi")
		{
			contents << "<Cmd>webwall redi " << nptr->interface << " ";
			contents << nptr->fromport << " " << nptr->toport << " " << nptr->dip;
			contents << "</Cmd>\n";
		}	
		else
		{	
			cout << "Error: the nat list data is wrong!\n";
			return -1;
		}
		 //printf("the cur index:%d, pack_tick: %d\n",index, pack->pack_tick);
		     //printf("the add timer i=%d   entry+++++++:%x    expires:%d\n", i, &ptr->entry, ptr->expires);
		//
	}
	//cout << "The filter rules num is: " << ruleNum << "and the list is:" << contents << endl;
	rslt = contents;
	
	return 0;
}

int 
AosCli::loadFwConfig(OmnString &rslt)
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

	OmnString loadShellCmd;
	
//	loadShellCmd << "more " << mFileName << " | /sbin/iptables-restore";
//	OmnCliSysCmd::doShell(loadShellCmd, rslt);
	
	// 
	// Config files are organized in blocks:
	//
	// ------AosCliBlock: <blockname>------
	// <AosCliCmd> ... </AosCliCmd> 
	// <AosCliCmd> ... </AosCliCmd> 
	// ...
	// ------EndAosCliBlock------
	//	
	// ...
	// 
	// This function loads the block identified by 'moduleName'.
	//
	OmnString type;
	OmnString beginTime;   
    OmnString endTime;   
    OmnString week; 
    OmnString chain;
	OmnString proto;
	OmnString sip;
	OmnString smask;
	OmnString smac;
	OmnString sport;
	OmnString dip;
	OmnString dmask;
	OmnString dport;
	OmnString action;
	OmnString rate;
	OmnString log;
	OmnString interface;
	OmnString to;	
	OmnString fromport;	
	OmnString toport;	

	OmnString contents, cmd, fwPrefix, subPrefix;
	OmnString start = "------AosCliBlock: ";
	start << "firewall" << "------\n";
	OmnString end = "------EndAosCliBlock------\n";
	bool err;

	if (!mFile->readBlock(start, end, contents, err))
	{
		if (err)
		{
			rslt << "********** Failed to read config for: ";
			rslt << "firewall" << "\n";
			return -eAosAlarm_FailedToReadFile;
		}

		return 0;
	}

	try
	{
		// 
		// Clear the module's config first
		//
		clearFwConfig(rslt);
		//cout << "The old config be cleared!" << endl;

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
				curPos = cmd.getWord(curPos,fwPrefix);
				if (fwPrefix == "firewall")
				{
					curPos = cmd.getWord(curPos,subPrefix);
					if(subPrefix == "ipfil")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "all")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,sip);
							curPos = cmd.getWord(curPos,smask);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwIpfilAll(chain, sip, smask, dip, dmask, action, log, rslt);
						}
						else if(subPrefix == "tcp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,sip);
							curPos = cmd.getWord(curPos,smask);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwIpfilTcp(chain, sip, smask, sport, dip, dmask, dport, action, log, rslt);
						}
						else if(subPrefix == "udp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,sip);
							curPos = cmd.getWord(curPos,smask);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwIpfilUdp(chain, sip, smask, sport, dip, dmask, dport, action, log, rslt);
						}
						else if(subPrefix == "icmp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,sip);
							curPos = cmd.getWord(curPos,smask);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwIpfilIcmp(chain, sip, smask, dip, dmask, action, log, rslt);
						}
						else if(subPrefix == "state")
						{
							curPos = cmd.getWord(curPos,subPrefix);
							if(subPrefix == "new")
							{
								curPos = cmd.getWord(curPos,chain);
								curPos = cmd.getWord(curPos,sip);
								curPos = cmd.getWord(curPos,smask);
								curPos = cmd.getWord(curPos,dip);
								curPos = cmd.getWord(curPos,dmask);
								curPos = cmd.getWord(curPos,action);
								curPos = cmd.getWord(curPos,log);
								fwIpfilStateNew(chain, sip, smask, dip, dmask, action, log, rslt);
							}
							else if(subPrefix == "ack")
							{
								curPos = cmd.getWord(curPos,chain);
								curPos = cmd.getWord(curPos,sip);
								curPos = cmd.getWord(curPos,smask);
								curPos = cmd.getWord(curPos,dip);
								curPos = cmd.getWord(curPos,dmask);
								curPos = cmd.getWord(curPos,action);
								curPos = cmd.getWord(curPos,log);
								fwIpfilStateAck(chain, sip, smask, dip, dmask, action, log, rslt);
							}
							else
							{
								cout << "Error:The State SubPrefix is wrong!" << endl;
								continue;
							}
						}
						else 
						{
							cout << "Error:The Ipfil SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else if(subPrefix == "macfil")
					{	
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "all")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,smac);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwMacfilAll(chain, smac, dip, dmask, action, log, rslt);
						}
						else if(subPrefix == "tcp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,smac);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwMacfilTcp(chain, smac, sport, dip, dmask, dport, action, log, rslt);
						}
						else if(subPrefix == "udp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,smac);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwMacfilUdp(chain, smac, sport, dip, dmask, dport, action, log, rslt);
						}
						else if(subPrefix == "icmp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,smac);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwMacfilIcmp(chain, smac, dip, dmask, action, log, rslt);
						}
						else 
						{
							cout << "Error:The Macfil SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else if(subPrefix == "snat")
					{
						curPos = cmd.getWord(curPos,interface);
						curPos = cmd.getWord(curPos,sip);
						curPos = cmd.getWord(curPos,smask);
						curPos = cmd.getWord(curPos,to);
						fwNatSnat(interface, sip, smask, to, rslt);
					}
					else if(subPrefix == "masq")
					{
						curPos = cmd.getWord(curPos,interface);
						curPos = cmd.getWord(curPos,sip);
						curPos = cmd.getWord(curPos,smask);
						fwNatMasq(interface, sip, smask, rslt);
					}
					else if(subPrefix == "dnat")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "ip")
						{
							curPos = cmd.getWord(curPos,interface);
							curPos = cmd.getWord(curPos,proto);
							curPos = cmd.getWord(curPos,sip);
							curPos = cmd.getWord(curPos,smask);
							curPos = cmd.getWord(curPos,dip);
							fwNatDnatIP(interface, proto, sip, dip, rslt);
						}
						else if(subPrefix == "port")
						{
							curPos = cmd.getWord(curPos,interface);
							curPos = cmd.getWord(curPos,proto);
							curPos = cmd.getWord(curPos,sip);
							curPos = cmd.getWord(curPos,smask);
							curPos = cmd.getWord(curPos,sport);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dport);
							fwNatDnatPort(interface, proto, sip, sport, dip, dport, rslt);
						}
						else
						{
							cout << "Error:The Dnat SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else if(subPrefix == "timefil")   
                    {   
                    	curPos = cmd.getWord(curPos,chain);   
                        curPos = cmd.getWord(curPos,proto);   
                        curPos = cmd.getWord(curPos,sip);   
                        curPos = cmd.getWord(curPos,smask);   
                        curPos = cmd.getWord(curPos,dip);   
                        curPos = cmd.getWord(curPos,dmask);   
                        curPos = cmd.getWord(curPos,action);   
                        curPos = cmd.getWord(curPos,beginTime);   
                        curPos = cmd.getWord(curPos,endTime);   
                        curPos = cmd.getWord(curPos,week);   
                        curPos = cmd.getWord(curPos,log);   
 cout << chain << " " << proto << " " <<  sip << " " << smask << " " << dip << " " << dmask \
      << " " << action << " " << beginTime << " " << endTime << " " << week << " " << log << endl;   
                        fwTimefil(chain, proto, sip, smask, dip, dmask, action, beginTime, endTime, week,log, rslt);   
                     }
					else if(subPrefix == "redi")
					{
						curPos = cmd.getWord(curPos,interface);
						curPos = cmd.getWord(curPos,proto);
						curPos = cmd.getWord(curPos,fromport);
						curPos = cmd.getWord(curPos,toport);
						fwNatRedi(interface, proto, fromport, toport, rslt);
					}
					else if(subPrefix == "antiattack")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "syn")
						{
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dport);
							curPos = cmd.getWord(curPos,rate);
							fwAntiSyn(dip, dport, rate, rslt);
						}
						else if(subPrefix == "icmp")
						{
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,rate);
							fwAntiIcmp(dip, rate, rslt);
						}
						else
						{
							cout << "Error:The Second SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else 
					{
						cout << "Error:The First SubPrefix is wrong!" << endl;
						continue;
					}
				}
				else if (fwPrefix == "webwall")
				{
						curPos = cmd.getWord(curPos,subPrefix);
						curPos = cmd.getWord(curPos,interface);
						curPos = cmd.getWord(curPos,fromport);
						curPos = cmd.getWord(curPos,toport);
						curPos = cmd.getWord(curPos,dip);
						webwallRedi(interface, fromport, toport, dip, rslt);
				}
				else
				{	
					cout << "Error:This is a bad firewall command!" << endl;
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
			<< "firewall" << endl;
		return -eAosAlarm_FailedToLoadConfig;
	}

	mFile->closeFile();
	mFile = 0;

	return 0;
}



int 
AosCli::clearFwConfig(OmnString &rslt)
{
	//
	// Clear the firewall's configurations 
	//
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
	struct fwNatListEntry * nptr;
    struct fwNatListEntry * ntmp;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        aos_free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgFwFilterList);
	
	aos_list_for_each_entry_safe(nptr, ntmp, &sgFwNatList, datalist)
    {       
       	aos_list_del(&nptr->datalist);
        aos_free(nptr);
    }
	AOS_INIT_LIST_HEAD(&sgFwNatList);
    
	OmnCliSysCmd::doShell("/sbin/iptables -F", rslt);
	//cout <<"clear fw test ===result:" << rslt;
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -F", rslt);
	//cout <<"clear fw test ===result:" << rslt;
	//OmnCliSysCmd::doShell("/sbin/iptables -L", rslt);
	return 0;
}

int 
AosCli::showFwConfig(OmnString &rslt)
{	
	//
	// Show the firewall current configurations 
	//
	OmnString contents;
	struct fwFilterListEntry * ptr;
	struct fwNatListEntry * nptr;	
	int filRuleNum = 0;
	int natRuleNum = 0;
	
	contents = "\nFilter Rules:\n";
	aos_list_for_each_entry(ptr, &sgFwFilterList, datalist)
	{
		filRuleNum ++;
		contents << "<Cmd>firewall ";
		if ((ptr->type == "fwIpfilAll") || (ptr->type == "fwIpfilIcmp"))
		{
			contents << "ipfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if (ptr->type == "fwIpfilStateNew")
		{
			contents << "ipfil state new " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if (ptr->type == "fwIpfilStateAck")
		{
			contents << "ipfil state ack " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if ((ptr->type == "fwIpfilTcp") || (ptr->type == "fwIpfilUdp"))
		{
			contents << "ipfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " " << ptr->sport << " ";
			contents << ptr->dip << " " << ptr->dmask << " " << ptr->dport << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if ((ptr->type == "fwMacfilAll") || (ptr->type == "fwMacfilIcmp"))
		{
			contents << "macfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->smac << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if ((ptr->type == "fwMacfilTcp") || (ptr->type == "fwMacfilUdp"))
		{
			contents << "macfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->smac << " " << ptr->sport << " ";
			contents << ptr->dip << " " << ptr->dmask << " " << ptr->dport << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if (ptr->type == "fwAntiSyn")
		{
			contents << "antiattack syn " << ptr->dip << " " << ptr->dport << " " << ptr->rate;
			contents << "</Cmd>\n";
		}
		else if (ptr->type == "fwAntiIcmp")
		{
			contents << "antiattack icmp " << ptr->dip << " " << ptr->rate;
			contents << "</Cmd>\n";
		}
		else if (ptr->type == "fwTimefil")   
        {   
            contents << "timefil " << ptr->chain << " " << ptr->proto << " ";   
            contents << ptr->sip << " " << ptr->smask << " ";   
            contents << ptr->dip << " " << ptr->dmask << " ";   
            contents << ptr->action << " " << ptr->beginTime<< " ";   
            contents << ptr->endTime << " " << ptr->week << " " ;   
            contents << ptr->log << "</Cmd>\n";   
        } 

		
		 //printf("the cur index:%d, pack_tick: %d\n",index, pack->pack_tick);
		     //printf("the add timer i=%d   entry+++++++:%x    expires:%d\n", i, &ptr->entry, ptr->expires);
		//
	}
	//cout << "The filter rules num is: " << ruleNum << "and the list is:" << contents << endl;

	contents << "\nNat Rules:\n";
	aos_list_for_each_entry(nptr, &sgFwNatList, datalist)
	{
		natRuleNum ++;
	 	if (nptr->type == "fwNatSnat")
		{
			contents << "<Cmd>firewall snat " << nptr->interface << " ";
			contents << nptr->sip << " " << nptr->smask << " ";
			contents << nptr->to;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "fwNatMasq")
		{
			contents << "<Cmd>firewall masq " << nptr->interface << " ";
			contents << nptr->sip << " " << nptr->smask;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "fwNatDnatIP")
		{
			contents << "<Cmd>firewall dnat ip " << nptr->interface << " ";
			contents << nptr->proto << " " << nptr->sip << " ";
			contents << nptr->dip;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "fwNatDnatPort")
		{
			contents << "<Cmd>firewall dnat port " << nptr->interface << " ";
			contents << nptr->proto << " " << nptr->sip << " " << nptr->sport << " ";
			contents << nptr->dip << " " << nptr->dport;
			contents << "</Cmd>\n";
		}	
		else if (nptr->type == "fwNatRedi")
		{
			contents << "<Cmd>firewall redi " << nptr->interface << " ";
			contents << nptr->fromport << " " << nptr->toport << " " << nptr->dip;
			contents << "</Cmd>\n";
		}	
		else if (nptr->type == "webwallRedi")
		{
			contents << "<Cmd>webwall redi " << nptr->interface << " ";
			contents << nptr->fromport << " " << nptr->toport << " " << nptr->dip;
			contents << "</Cmd>\n";
		}	
		else
		{	
			cout << "Error: the nat list data is wrong!\n";
		}
	}
	cout << "------config : firewall------\n" << contents;
	
	//cout << "----This is the Filter Rules----\n" << endl;
//	OmnCliSysCmd::doShell("/sbin/iptables -L -v", rslt);
	//cout << "\n----This is the NAT Rules----\n" << endl;
//	OmnCliSysCmd::doShell("/sbin/iptables -L -v -t nat", rslt);
	//cout <<"clear fw test ===result:" << rslt;
	
	return 0;
}

int
AosCli::showFwLog(OmnString &rslt)
{
	// for Gentoo FC, and Debian
	OmnCliSysCmd::doShell("/bin/more /var/log/messages | grep AOS_FW_LOG | more", rslt);
	return 0;
}


int 
AosCli::initFwConfig(OmnString &rslt)
{
	//
	// Init the firewall's configurations 
	//
	//Step 1: Load Modules
	//Step 2: Set the params NOTE:must be ROOT
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_forward", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_dynaddr", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/tcp_syncookies", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts", rslt);
	//Step 3: Set the policies
	OmnCliSysCmd::doShell("/sbin/iptables -P INPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P FORWARD ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P PREROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P POSTROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P PREROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P OUTPUT ACCEPT", rslt);
	
	//Step 4: Clear the rules
	OmnCliSysCmd::doShell("/sbin/iptables -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -X", rslt);
	//cout <<"clear fw test ===result:" << rslt;

	//step 5: Clear the LIST
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
	struct fwNatListEntry * nptr;
    struct fwNatListEntry * ntmp;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        aos_free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgFwFilterList);
	
	aos_list_for_each_entry_safe(nptr, ntmp, &sgFwNatList, datalist)
    {       
       	aos_list_del(&nptr->datalist);
        aos_free(nptr);
    }
	AOS_INIT_LIST_HEAD(&sgFwNatList);

    //
	//Show the current config
	//OmnCliSysCmd::doShell("/sbin/iptables -L", rslt);
	return 0;
}


int 
AosCli::initFwMaxsecConfig(OmnString &rslt)
{
	//
	// Init the firewall's configurations 
	//
	//Step 1: Load Modules
	//Step 2: Set the params NOTE:must be ROOT
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_forward", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_dynaddr", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/tcp_syncookies", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts", rslt);
	//Step 3: Set the policies
	OmnCliSysCmd::doShell("/sbin/iptables -P INPUT DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P OUTPUT DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P FORWARD DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P PREROUTING DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P POSTROUTING DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P OUTPUT DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P PREROUTING DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P OUTPUT DROP", rslt);
	
	//Step 4: Clear the rules
	OmnCliSysCmd::doShell("/sbin/iptables -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -X", rslt);
	//cout <<"clear fw test ===result:" << rslt;
    
	//step 5: Clear the LIST
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
	struct fwNatListEntry * nptr;
    struct fwNatListEntry * ntmp;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        aos_free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgFwFilterList);
	
	aos_list_for_each_entry_safe(nptr, ntmp, &sgFwNatList, datalist)
    {       
       	aos_list_del(&nptr->datalist);
        aos_free(nptr);
    }
	AOS_INIT_LIST_HEAD(&sgFwNatList);

	//Show the current config
	//OmnCliSysCmd::doShell("/sbin/iptables -L", rslt);
	return 0;
}

int 
AosCli::initFwMidsecConfig(OmnString &rslt)
{
	//
	// Init the firewall's configurations 
	//
	//Step 1: Load Modules
	//Step 2: Set the params NOTE:must be ROOT
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_forward", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_dynaddr", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/tcp_syncookies", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts", rslt);
	//Step 3: Set the policies
	OmnCliSysCmd::doShell("/sbin/iptables -P INPUT DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P FORWARD DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P PREROUTING DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P POSTROUTING DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P PREROUTING DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P OUTPUT ACCEPT", rslt);
	
	//Step 4: Clear the rules
	OmnCliSysCmd::doShell("/sbin/iptables -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -X", rslt);
	//cout <<"clear fw test ===result:" << rslt;

    //step 5: Clear the LIST
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
	struct fwNatListEntry * nptr;
    struct fwNatListEntry * ntmp;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        aos_free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgFwFilterList);
	
	aos_list_for_each_entry_safe(nptr, ntmp, &sgFwNatList, datalist)
    {       
       	aos_list_del(&nptr->datalist);
        aos_free(nptr);
    }
	AOS_INIT_LIST_HEAD(&sgFwNatList);

	//Show the current config
	//OmnCliSysCmd::doShell("/sbin/iptables -L", rslt);
	return 0;
}


int 
AosCli::initFwMinsecConfig(OmnString &rslt)
{
	//
	// Init the firewall's configurations 
	//
	//Step 1: Load Modules
	//Step 2: Set the params NOTE:must be ROOT
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_forward", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_dynaddr", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/tcp_syncookies", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts", rslt);
	//Step 3: Set the policies
	OmnCliSysCmd::doShell("/sbin/iptables -P INPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P FORWARD ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P PREROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P POSTROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P PREROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P OUTPUT ACCEPT", rslt);
	
	//Step 4: Clear the rules
	OmnCliSysCmd::doShell("/sbin/iptables -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -X", rslt);
	//cout <<"clear fw test ===result:" << rslt;

	//step 5: Clear the LIST
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
	struct fwNatListEntry * nptr;
    struct fwNatListEntry * ntmp;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        aos_free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgFwFilterList);
	
	aos_list_for_each_entry_safe(nptr, ntmp, &sgFwNatList, datalist)
    {       
       	aos_list_del(&nptr->datalist);
        aos_free(nptr);
    }
	AOS_INIT_LIST_HEAD(&sgFwNatList);

	//Show the current config
	//OmnCliSysCmd::doShell("/sbin/iptables -L", rslt);
	return 0;
}
#endif

