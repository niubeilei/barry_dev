////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliProc.cpp
// Description:
//	To add a CLI command:
//		1. Define the command in the cmd.txt file
//		2. Add a line in OmnKernelApi_addCliCmds(...) (aos_core/aosKernelApi.cpp file)
//		3. Write the CLI processing function.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliUtil/CliUtilProc.h"

#include "CliUtil/CliUtilCmd.h"
#include "CliUtil/Ptrs.h"
#include "Alarm/Alarm.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "UtilComm/CommGroup.h"
#include "UtilComm/UdpComm.h"
#include "UtilComm/TcpCommSvr.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"
#include "XmlParser/Ptrs.h"
#include "UtilComm/TcpClient.h"

#define CLI_DEBUG 0

static OmnString sgDefaultCmdName = "/usr/local/rhc/bin/cmd.txt";

OmnSingletonImpl(OmnCliUtilProcSingleton,
				 OmnCliUtilProc,
				 OmnCliUtilProcSelf,
				 "OmnCliUtilProc");


OmnCliUtilProc::OmnCliUtilProc()
:
mTcpIndex(0),
mNum(0),
mListen(NULL)
{
	//
	// Li Jing, 02/08/2006
	// Check whether the license is good and set the flag.
	//
	// sgLicenseValid = AosCheckLicense();
}


bool
OmnCliUtilProc::addCmd(const OmnString &def)
{
	OmnCliUtilCmd* cmd = OmnNew OmnCliUtilCmd(def);
	
	if (!checkRelated(cmd->getModId())) 
	{
		OmnDelete cmd;
		return true;
	}
	
	OmnString rslt;
	if (!cmd->checkDef(rslt))
	{
		OmnAlarm << "Command not good: " << rslt << enderr;
		return false;
	}

	//
	// We should check whether the command has already been defined
	// in the tree. If yes, we should report an error and return.
	//
	// To get the command.
	//
	// Not implemented yet
	//
	if (aosCharPtree_insert(mCmdTree,cmd->getPrefix().data(), 
			cmd->getPrefix().length(), (void *)cmd, 1))
	{
		OmnAlarm << "Failed to add command" << enderr;
		return false;
	}
	OmnTrace << "Command added: " << cmd->getPrefix() << endl;

	return true;
}

bool
OmnCliUtilProc::start()
{
	return true;
}

bool
OmnCliUtilProc::stop()
{
	return true;
}

OmnCliUtilProc::~OmnCliUtilProc()
{
}

OmnRslt
OmnCliUtilProc::config(const OmnXmlParserPtr &conf)
{
	char buf[256], *p;
	unsigned int len;

	len = readlink("/proc/self/exe", buf, sizeof buf);
	if (len < 0 || len > (sizeof(buf)-4))
		return false;
	
	p = strrchr(buf, '/');
	if (p == NULL)
		return false;
	
	*p = 0;
	strcat(buf, "/cmd.txt");
	mFilename = buf;

	if (access(mFilename.getBuffer(), R_OK) < 0)
	{
		mFilename = sgDefaultCmdName;
		if (access(mFilename.getBuffer(), R_OK) < 0)
		{
			OmnTrace << mFilename << " doesn't exist" << endl;
			return false;
		}
	}
	
	return true;
}

OmnString	
OmnCliUtilProc::getTcpListenerName() const
{
	return "CliProc";
}

void
OmnCliUtilProc::msgRecved(const OmnConnBuffPtr &buff,
						   const OmnTcpClientPtr &conn)
{
	OmnString rslt;
	runCli(buff->getString(), rslt);
	if (rslt.length() == 5)
	{
		rslt.setChar(0, 0);
	}
	mListen->writeTo(rslt, conn);
}

bool
OmnCliUtilProc::runCli(const OmnString &str, OmnString &rslt,const AosCliLvl::CliLevel level)
{
	int foundIndex;
	OmnCliUtilCmd *cmd = 0;
	OmnString clearStr = str;
	
	if (!mCmdTree)
	{
		rslt << "Ptree is null";
		return false;
	}

	aosCharPtree_get(mCmdTree, clearStr.data(), &foundIndex, (void **)&cmd);
    if (!cmd)
	{
		rslt << "Command not found from ptree(userland): " << str;
		return false;
	}

	if(level < cmd->getLevel())
	{
		rslt = "permission deny";
		return false;
	}
	clearStr.removeTailWhiteSpace();
	return cmd->run(clearStr,foundIndex+1, rslt);
}


bool
OmnCliUtilProc::addCmds(const OmnString &filename)
{
	// 
	// All commands are defined in the file 'filename'. 
	// This function reads the definition and creates these
	// commands. 
	//
	
	OmnFile file(filename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		
		OmnAlarm << "Failed to read the command definition file: " 
			<< filename << enderr;
		return false;
	}
	OmnString buffer;
	while (1)
	{
		buffer = "";
		if (!file.skipTo("\n<Cmd>", false))
		{
			// 
			// Finished.
			//
			return true;
		}

		file.skipChar();	// Skip the '\n' in "\n<Cmd>"
		if (!file.readUntil(buffer, "\n</Cmd>", 
				eMaxCommandLen, true))
		{
			OmnAlarm << "Failed to read the next command after: " 
				<< buffer << enderr;
			return false;
		}

		// 
		// Read the command. Create the command now.
		// 
		addCmd(buffer);
	}
}

bool
OmnCliUtilProc::getRelatedCmds(char** names, int num)
{
	mRelated = names;
	mNum = num;
	
	mCmdTree = aosCharPtree_create();
	if (!mCmdTree)
	{
		OmnTrace << "Can't create ptree" << endl;
		return false;
	}
	
	if (addCmds(mFilename))
	{
		return true;
	}

	return false;
}

bool
OmnCliUtilProc::startListen(short port)
{
	if (mListen)
		return true;

#if 0 
	mListen = OmnNew OmnTcpServer(OmnIpAddr("127.0.0.1"), port, 1, "CliUtil",OmnTcp::eNoLengthIndicator);
#else
	mListen = OmnNew OmnTcpServer("/var/run/.clisock", "CliUtil", 
			OmnTcp::eNoLengthIndicator);
#endif

	OmnString err;
	if (!mListen->connect(err))
	{
		OmnAlarm << "Failed to start NMS TCP Server" 
			<< err << enderr;
		return false;
	}

	OmnCliUtilProcPtr listener(this, false);

	mListen->setListener(listener);
	mListen->startReading();
	return true;
}


unsigned short
OmnCliUtilProc::getPortByAppname(char* name)
{
	OmnFile file(mFilename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to read the command definition file: " 
			<< mFilename << enderr;
		return 0;
	}

	OmnString buffer = "";
	//Not found, error happened
	if (!file.skipTo("\n<ModDef>", false))
		return 0;	

	file.skipChar();	// Skip the '\n' in "\n<ModDef>"
	if (!file.readUntil(buffer, "\n</ModDef>", 
			eMaxCommandLen, true))
	{
		OmnAlarm << "Failed to read mod denfinition " << enderr;
		return 0;
	}

	OmnXmlParser parser(buffer);
	OmnXmlItemPtr mods = parser.getItem("ModDef");
	if (!mods)
	{
		OmnAlarm << "Program error. mods is null" << enderr;
		return 0;
	}
	
	mods->reset();
	while (mods->hasMore())
	{
		OmnXmlItemPtr mod = mods->next();
		if (!mod)
		{
			OmnAlarm << "Program error. mod is null" << enderr;
			return 0;
		}
		if (mod->getStr("ModId", "") == OmnString(name))
			return mod->getInt("ModPort", 0);
	}

	return 0;
}

bool
OmnCliUtilProc::startListen(char* appname)
{
	unsigned short port;

	if (mListen)
		return true;

	if (appname == NULL || appname[0] == 0)
		return false;
	
	port = getPortByAppname(appname);
	if (port == 0)
		return false;
	
	return startListen(port);
}

bool 
OmnCliUtilProc::checkRelated(const char* def)
{
	if ( mNum == 0)
	{	
		return true;
	}
	for (int i = 0; i < mNum; i++)
	{
		if (strcmp(def, mRelated[i]) == 0)
		{
			return true;
		}
	}
	return false;
}

bool 
OmnCliUtilProc::OmnUserLand_addCliCmd(char *id, aosCliFunc func)
{
	int foundIndex;
	OmnCliUtilCmd* cmd;

	if (!mCmdTree)
	{
		OmnTrace << "Ptree is null" << endl;
		return false;
	}
	
	aosCharPtree_get(mCmdTree, id, &foundIndex, (void **)&cmd);
	if (!cmd)
	{
		return false;
	}

	cmd->mFunc = func;
	return true;
}

