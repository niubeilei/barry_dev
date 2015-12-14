////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ApplicationProxyTester.cpp
// Description:
//   
//

//1. secure authcmd status <on|off>
//2. secure authcmd show status
//3. secure authcmd command add <command_path>
//4. secure authcmd command del <command_path>
//5. secure authcmd command show
//6. secure authcmd command reset
//7. secure authcmd policy <accept|deny>
//
//
// Modification History:
// 	11/16/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "SystemCli/SCATester/SCATorturer.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Types.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Porting/Sleep.h"
#include "Random/RandomInteger.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "Util/Random.h"
#include "UtilComm/Ptrs.h"
#include "KernelSimu/timer.h"

#include "Tester/Test.h"
#include "Tester/TestMgr.h"

// 
// CLI Command selector definition
//
static OmnString sgCliSelectorDef =	
		"<Elements>"		
		    "<Element>1,  10</Element>"		// set status
			"<Element>2,  0</Element>"		// show status
	        "<Element>3,  42</Element>"		// add command
	        "<Element>4,  35</Element>"		// del command 
	        "<Element>5,  1</Element>"		// reset command
	        "<Element>6,  2</Element>"		// show conmmand
	       	"<Element>7,  10</Element>"		// set policy
		"</Elements>";

OmnString cmd_incorrect[16];
OmnString cmd_correct[70];

AosSCATorturer::AosSCATorturer()
{
	mName = "SCATorturer";
}

bool 
AosSCATorturer::start()
{
	do_init();
	basicTest();
	return true;
}

bool 
AosSCATorturer::basicTest()
{
	OmnBeginTest << "Test SCA";

	AosRandomInteger cliSelector("CliSelector", sgCliSelectorDef, 100);

	int count = 0;
	bool rslt;
	
	/*define excute times*/
	while (count < eRunTimes)
	{
		int index = cliSelector.nextInt();
		
		switch (index)
		{
			case eSetStatus:
				rslt = runSetStatus();
				break;
			case eShowStatus:
				rslt = runShowStatus();
				break;
			case eSetPolicy:
				rslt = runSetPolicy();
				break;
			case eAddCommand:
				rslt = runAddCommand();
				break;
			case eDelCommand:
				rslt = runDelCommand();
				break;
			case eResetCommand:
				rslt = runResetCommand();
				break;
			case eShowcommand:
				rslt = runShowCommand();
				break;		
			default:
				OmnAlarm << "Invalid index: " << index << enderr;
				break;					
		}
		count++;
	}	
}

bool 
AosSCATorturer::do_init()
{
	cmd_correct[0] = "/bin/arch";
	cmd_correct[1] = "/bin/dd";
	cmd_correct[2] = "/bin/install";
	cmd_correct[3] = "/bin/mknod";
	cmd_correct[4] = "/bin/sed";
	cmd_correct[5] = "/bin/touch";
	cmd_correct[6] = "/bin/bzmore";
	cmd_correct[7] = "/bin/df";
	cmd_correct[8] = "/bin/fuser";
	cmd_correct[9] = "/bin/kill";
	cmd_correct[10] = "/bin/mktemp";
	cmd_correct[11] = "/bin/printenv";
	cmd_correct[12] = "/bin/seq";
	cmd_correct[13] = "/bin/tr";
	cmd_correct[14] = "/bin/basename";
	cmd_correct[15] = "/bin/cat";
	cmd_correct[16] = "/bin/dir";
	cmd_correct[17] = "/bin/grep";
	cmd_correct[18] = "/bin/killall";
	cmd_correct[19] = "/bin/more";
	cmd_correct[20] = "/bin/ps";
	cmd_correct[21] = "/bin/setfont";
	cmd_correct[22] = "/bin/true";
	cmd_correct[23] = "/bin/zdiff";
	cmd_correct[24] = "/bin/bash";
	cmd_correct[25] = "/bin/chattr";
	cmd_correct[26] = "/bin/dircolors";
	cmd_correct[27] = "/bin/link";
	cmd_correct[28] = "/bin/pstree";
	cmd_correct[29] = "/bin/tty";
	cmd_correct[30] = "/bin/chmod";
	cmd_correct[31] = "/bin/dmesg";
	cmd_correct[32] = "/bin/groups";
	cmd_correct[33] = "/bin/loadkeys";
	cmd_correct[34] = "/bin/nano";
	cmd_correct[35] = "/bin/ptx";
	cmd_correct[36] = "/bin/sort";
	cmd_correct[37] = "/bin/uname";
	cmd_correct[38] = "/bin/zforce";
	cmd_correct[39] = "/bin/chown";
	cmd_correct[40] = "/bin/login";
	cmd_correct[41] = "/bin/netstat";
	cmd_correct[42] = "/bin/pwd";
	cmd_correct[43] = "/bin/split";
	cmd_correct[44] = "/bin/uniq";
	cmd_correct[45] = "/bin/zgrep";
	cmd_correct[46] = "/bin/chroot";
	cmd_correct[47] = "/bin/logname";
	cmd_correct[48] = "/bin/nice";
	cmd_correct[49] = "/bin/stat";
	cmd_correct[50] = "/bin/unlink";
	cmd_correct[51] = "/bin/zless";
	cmd_correct[52] = "/bin/cksum";
	cmd_correct[53] = "/bin/du";
	cmd_correct[54] = "/bin/gzexe";
	cmd_correct[55] = "/bin/ls";
	cmd_correct[56] = "/bin/sync";
	cmd_correct[57] = "/bin/wc";
	cmd_correct[58] = "/bin/tar";
	cmd_correct[59] = "/bin/mkdir";
	cmd_correct[60] = "/bin/cut";
	cmd_correct[61] = "/sbin/arp";
	cmd_correct[62] = "/sbin/halt";
	cmd_correct[63] = "/sbin/init";
	cmd_correct[64] = "/sbin/rc";
	cmd_correct[65] = "/sbin/route";
	cmd_correct[66] = "/sbin/grub";
	cmd_correct[67] = "/sbin/iptables";
	
	
	
	cmd_incorrect[0] = "/home";
	cmd_incorrect[1] = "/etc";
	cmd_incorrect[2] = "/dsf/";
	cmd_incorrect[3] = "/root";
	cmd_incorrect[4] = "/dev/ls";
	cmd_incorrect[5] = "/bin/";
	cmd_incorrect[6] = "/sbin/sdfg";
	cmd_incorrect[7] = "/proc/bus";
	cmd_incorrect[8] = "/proc/;lj;l";
	cmd_incorrect[9] = "//bin";
	cmd_incorrect[10] = "/lib/";
	cmd_incorrect[11] = "/opt/lkjlj.exe";
	cmd_incorrect[12] = "/sys/";
	
	
	return true;
}


bool 
AosSCATorturer::runSetStatus()
{
	OmnString cmd = "secure authcmd status ";
	OmnString status;
	OmnString rslt;
	
	if (OmnRandom::nextBool() == true)
		status = "on";
	else
		status = "off";
		
	cmd << status;
	
	aosRunCli_getStr(cmd, true, rslt);	
	
	return true;
}


bool 
AosSCATorturer::runShowStatus()
{
	return true;
}


bool 
AosSCATorturer::runAddCommand()
{
	OmnString rslt;
	OmnString cmd = "secure authcmd command add ";
	OmnString command;
	int index = OmnRandom::nextInt(1, 100);
	
	bool isCorrect;
	/*generate a correct cli(80%)*/
	
	isCorrect = true;

	if(index <= 70)
	{
		genCommand(command);
		
		/*if the command already exist*/
		for(int i = 0; i < mNumCmd; i++)
		{
			if(command == cmd_exist[i])
			{
				return true;
			}
		}
		
		cmd << command ;	
		aosRunCli_getStr(cmd, isCorrect, rslt);
		addCmd(command);
		
		return true;			
	}
	/*generate an incorrect cli(20%)*/
	else if(index <= 90 )
	{
		OmnRandom::nextLetterStr(command.getBuffer(), 1, 20);
	
		/*if random generate an strin match the correct command£¬break*/
		for(int i = 0; i < 67; i++)
		{
			if (command == cmd_correct[i])
			{
				return true;	
			}
		}
	
		cmd << command ; 
		isCorrect = false;
		aosRunCli_getStr(cmd, isCorrect, rslt);
	
		return true;
	}
	
	genCmdIncorrect(command);
	cmd << command;
	aosRunCli_getStr(cmd, false, rslt);		

	return true;
}


bool 
AosSCATorturer::runDelCommand()
{
	OmnString rslt;
	OmnString cmd = "secure authcmd command del ";
	OmnString command;
	bool isCorrect;
	
	isCorrect = true;
	
	int index = OmnRandom::nextInt(1, 100);
	/*generate a correct cli(80%)*/
	if( index <= 80)
	{
		int i = OmnRandom::nextInt(0,mNumCmd);
		if(cmd_exist[i] != "")
		{	
			command = cmd_exist[i];
		
			cmd << command;		
			aosRunCli_getStr(cmd, isCorrect, rslt);
		
			for (int j=i; j<mNumCmd; j++)
			{
				cmd_exist[j] = cmd_exist[j+1];
			}
			cmd_exist[mNumCmd] = "";
			mNumCmd--;
		}	
		return true;				
	}
	
	/*generate an incorrect cli(20%)*/

	OmnRandom::nextLetterStr(command.getBuffer(),1,20);
	
	/*if random generate an strin match the correct command£¬break*/
//	for(int i = 0; i < 67; i++)
//	{
//		if( command == cmd_correct[i] )
//		{
//			return true;	
//		}
//	}
	
	cmd << command ; 
	aosRunCli_getStr(cmd, false, rslt);
	

	
	return true;

}


bool 
AosSCATorturer::runResetCommand()
{
	OmnString cmd = "secure authcmd command reset";
	OmnString rslt;	
	bool isCorrect;
	
	isCorrect = true;
	
	aosRunCli_getStr(cmd, isCorrect, rslt);
	/*clear exist commands*/
	for(int i=1; i < mNumCmd; i++)
	{
		cmd_exist[i] = "";
	}
	mNumCmd = 0;
	return true;
}	

bool 
AosSCATorturer::runShowCommand()
{
	OmnString cmd = "secure authcmd command show";
	OmnString rslt;
	
	aosRunCli_getStr(cmd, true, rslt);
	
	OmnString command[80];
	int i, j;
	for (i = 0; i < 80; i++)
	{
		for (j=0; j<mNumCmd; j++) {
			if (command[i] == cmd_exist[j])
				command[i] = "";
		}
		
	}
	
	for (i = 0; i < 80; i++)
	{
		if (command[i] != "")
		{
			cout << "error\n" << endl;
			return false;
		}
	}
	
	return true;
}


bool 
AosSCATorturer::runSetPolicy()
{
	OmnString rslt;
	OmnString cmd = "secure authcmd policy ";
	
	if( OmnRandom::nextInt(1, 100) <= 80)
	{
		/*generate a correct cli(80%)*/
		OmnString policy;
		genPolicy(policy);
		cmd << policy ;
		
		aosRunCli_getStr(cmd, true, rslt);
		return true;			
	}
	
	/*generate an incorrect cli(20%)*/
	char policy[20];
	OmnRandom::nextLetterStr(policy,1,20);
	
	/*if random generate an str equal "accept" or "deny"£¬break*/
	if( policy == "accept" || policy == "deny" )
	{
		return true;	
	}
	cmd << policy ; 
	aosRunCli_getStr(cmd, false, rslt);
	return true;
}


bool
AosSCATorturer::genPolicy(OmnString &policy)
{
	//
	//	generate an correct policy
	//
	
	OmnString policies[2];
	policies[0] = "accept";
	policies[1] = "deny";
	
	/*random select from "accept" and "deny"*/
	int i = OmnRandom::nextInt(0,1);
	policy = policies[i];
	
	return true;
}


bool  
AosSCATorturer::genCommand(OmnString &command)
{
	//	
	//	select a correct command from list
	//
	
	int i = OmnRandom::nextInt(0,67);
	
	command = cmd_correct[i];
	return true;
}

bool
AosSCATorturer::addCmd(const OmnString &command)
{
	if (mNumCmd >= eMaxCmd)
	{
		return false;
	}

	cmd_exist[mNumCmd] = command;
	
	mNumCmd++;
	return true;
}


bool 
AosSCATorturer::genCmdIncorrect(OmnString &command)
{
	int i = OmnRandom::nextInt(0,12);
		
	command = cmd_incorrect[i];
	return true;
}

