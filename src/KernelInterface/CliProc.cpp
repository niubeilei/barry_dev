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

#include "KernelInterface/CliProc.h"

#include "Alarm/Alarm.h"
#include "alarm/Alarm.h"
#include "aosUtil/Memory.h"
#include "aosUtil/ReturnCode.h"
#include "CliClient/ModuleCliServer.h"
#include "Debug/Debug.h"
#include "KernelInterface/Ptrs.h"
#include "KernelInterface/CliCmd.h"
#include "KernelInterface/CliSysCmd.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "UtilComm/CommGroup.h"
#include "UtilComm/UdpComm.h"
#include "UtilComm/TcpCommSvr.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"

#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"

#include "KernelAPI/KernelAPI.h"

static OmnString sgDefaultConfigName = "/usr/local/rhc/config/rhc.conf";
static OmnString sgDefaultCmdName = "/usr/local/rhc/bin/cmd.txt";

static char *sgModuleNames[] = 
{
	"firewall",
	"dnsmasq",
	"vlan",
	"mac",
	"dmz",
	"wan",
	//"dns proxy",
	//"dhcp server",
	"pppoe",
	"bridge",
	"inetctrl",
	"pptp",
	"webwall",
	"qos",
	"disk mgr quota",
	"res mgr",
	""
};

OmnSingletonImpl(OmnCliProcSingleton,
				 OmnCliProc,
				 OmnCliProcSelf,
				 "OmnCliProc");


// 
// Li Jing, 02/08/2006
//
static bool sgLicenseValid = true;

OmnCliProc::OmnCliProc()
:
mTcpIndex(0),
mModDefs(NULL),
mConfFile(NULL),
mConnClient(NULL)
{
	//
	// Li Jing, 02/08/2006
	// Check whether the license is good and set the flag.
	//
	// sgLicenseValid = AosCheckLicense();
}


bool
OmnCliProc::addCmd(const OmnString &def)
{
	OmnCliCmd *cmd = 0;
	if(def == "syscmd")
	{
		cmd = OmnNew OmnCliSysCmd("shell");
	}
	else
	{
		cmd = OmnNew OmnCliCmd(def);
	}
	OmnString rslt;
	if (!cmd->checkDef(rslt))
	{
		OmnAlarm << "Command not good: " << rslt << enderr;
		return false;
	}
	
	// now set the address and port belonging to module from cmd.txt
	mModDefs->reset();
	while (mModDefs->hasMore())
	{
		OmnXmlItemPtr mod = mModDefs->next();
		if (!mod)
	    {
		    OmnAlarm << "Program error. mod is null" << enderr;
			return 0;
		}
		
		if (mod->getStr("ModId", "") == "local")
			continue;

		if (mod->getStr("ModId", "") == "kernel")
			continue;

		if (mod->getStr("ModId", "") == cmd->getModId())
		{
			cmd->setModAddr(mod->getStr("ModAddr", "127.0.0.1"));
			cmd->setModPort(mod->getInt("ModPort", 0));
			break;
		}
	}
	
	//
	// We should check whether the command has already been defined
	// in the tree. If yes, we should report an error and return.
	//
	// To get the command.
	//
	// Not implemented yet
	//

	if (aosCharPtree_insert(mCmdTree, cmd->getPrefix().data(), 
			cmd->getPrefix().length(), (void *)cmd, 1))
	{
		OmnAlarm << "Failed to add command" << enderr;
		return false;
	}

	OmnTrace << "Command added: " << cmd->getPrefix() << endl;

	return true;
}


bool
OmnCliProc::start()
{
	mCmdTree = aosCharPtree_create();
	if (!mCmdTree) 
	{
		OmnTrace << "Can't create the ptree" << endl;
		return false;
	}

	if (!mCmdTree)
		return false;

	if(!addCmds(mFilename))
		return false;

	OmnTcpListenerPtr tcpListener(this, false);
	for (int i=0; i<mTcpIndex; i++)
	{
		mTcpComm[i] = OmnNew OmnTcpCommSvr(mTcpAddr[i], 
			mTcpPort[i], "CliProc");
		mTcpComm[i]->startReading(tcpListener);
	}
	return true;
}


bool
OmnCliProc::stop()
{
	return true;
}


OmnCliProc::~OmnCliProc()
{
	mConnClient->closeConn();
	mConnClient = NULL;
}

bool
OmnCliProc::getModDefs()
{
	if (mFilename == "")
		return false;
	
	OmnFile file(mFilename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to read the module definition file: " 
			<< mFilename << enderr;
		return 0;
	}

	OmnString buffer = "";
	//Not found, error happened
	if (!file.skipTo("\n<ModDef>", false))
		return 0;	

	file.skipChar();	// Skip the '\n' in "\n<ModDef>"
	if (!file.readUntil(buffer, "\n</ModDef>", 
			20000, true))
	{
		OmnAlarm << "Failed to read mod denfinition " << enderr;
		return 0;
	}

	OmnXmlParser parser(buffer);
	mModDefs = parser.getItem("ModDef");
	
	if (!mModDefs)
		return false;

	return true;
}

OmnRslt
OmnCliProc::config(const OmnXmlParserPtr &conf)
{
	//
	// <CliProc>
	//		<CmdDef>
	//		...
	// </CliProc>
	//
	
	OmnKernelApi::init();
	
	//important, for save config
	mConfFileName = sgDefaultConfigName;
	
	char buf[256];
	size_t len;

	len = readlink("/proc/self/exe", buf, sizeof(buf)-4);
	if (len < 0 || len > (sizeof(buf)-4))
	{
		return false;
	}
	
	buf[len] = 0;
	mFilename = buf;

	len = mFilename.find('/', true);
	if (len < 0)
		return false;
	
	mFilename.setLength(len);
	mFilename += "/cmd.txt";
	
	if (access(mFilename.getBuffer(), R_OK) < 0)
	{
		mFilename = sgDefaultCmdName;
		if (access(mFilename.getBuffer(), R_OK) < 0)
		{
			cout << "CLI definition file doesn't exist. Please check with it." << endl;
			return false;
		}
	}
	
	if (!getModDefs())
		return false;

	if (!conf)
	{
		return true;
	}

    OmnXmlItemPtr def = conf->tryItem("CliProc");
    if (!def)
    {
		return true;
    }

	mFilename = def->getStr("CmdDef", "cmd.txt");
	OmnXmlItemPtr tcpConfig = def->tryItem("TcpConfig");
	if (tcpConfig)
	{
		// 
		// Need to start the UDP connection
		//
		tcpConfig->reset();
		mTcpIndex = 0;
		while (tcpConfig->hasMore() && mTcpIndex < eMaxTcpConns)
		{
			OmnXmlItemPtr tcpInfo = tcpConfig->next();
			OmnIpAddr addr = tcpInfo->getIpAddr("Addr", OmnIpAddr::eInvalidIpAddr);
			int port = tcpInfo->getInt("Port", -1);
			if (!addr.isValid() || port <= 0)
			{
				OmnAlarm << "Invalid TCP connection: " 
					<< tcpInfo->toString() << enderr;
			}
			else
			{
				mTcpAddr[mTcpIndex] = addr;
				mTcpPort[mTcpIndex] = port;
				mTcpIndex++;
			}
		}
    }

	return true;
}


OmnString	
OmnCliProc::getTcpListenerName() const
{
	return "CliProc";
}


void
OmnCliProc::msgRecved(const OmnConnBuffPtr &buff,
						   const OmnTcpClientPtr &conn)
{
	return;
}


bool
OmnCliProc::runCli(const OmnString &str, OmnString &rslt,const AosCliLvl::CliLevel level)
{
	return procCli(str,rslt,false,level);
}

bool
OmnCliProc::runCliAsClient(const OmnString &str, OmnString &rslt,const AosCliLvl::CliLevel level)
{
	return procCli(str,rslt,true,level);
}

bool
OmnCliProc::appendIntToStr(OmnString &rslt, int ret)
{
	int len = rslt.length();
	char *r;
	
	rslt.setSize(len+sizeof(int)+2);
	rslt.setLength(len+sizeof(int)+1);
	r = rslt.getBuffer() + len + 1;
	memcpy(r, &ret, sizeof(int));

	return true;
}

bool
OmnCliProc::procCli(const OmnString &str, OmnString &rslt,const bool isClient,const AosCliLvl::CliLevel level)
{
	int foundIndex;
	OmnCliCmd *cmd = 0;
    
	OmnString modId;
	OmnString errMsg;
	//
	// 02/08/2006, Li Jing
	// 
	if (!sgLicenseValid)
	{
		rslt << "License is not valid";

		appendIntToStr(rslt, -1);
		return false;
	}
	
	if (!mCmdTree) {
		rslt << "Tree is null";
		appendIntToStr(rslt, -1);
		return false;
	}
	
	aosCharPtree_get(mCmdTree, str.data(), &foundIndex, (void **)&cmd);
    if (!cmd)
	{
		rslt << "Command not found from ptree(userland): " << str;

		appendIntToStr(rslt, -1);
		return false;
	}

	if(level < cmd->getLevel())
	{
		rslt = "permission deny";

		appendIntToStr(rslt, -1);
		return false;
	}

	// 
	// For the commands: 
	// 	"save config"
	// 	"load config"
	// 	"clear config"
	// we will process it here. 
	//
	if (cmd->getModId() == "local")
	{
		int ret;
 		if (cmd->getOprId() == "save_config")
		{
			ret = saveConfig(rslt);
			appendIntToStr(rslt, ret);

			return (ret == 0);
		}
		
		if (cmd->getOprId() == "load_config")
		{
			ret = loadConfig(rslt);
			appendIntToStr(rslt, ret);

			return (ret == 0);
		}
		
		if (cmd->getOprId() == "clear_config")
		{
			ret = clearConfig(rslt);
			appendIntToStr(rslt, ret);

			return (ret == 0);
		}
		
		appendIntToStr(rslt, 0);
		return true;
	}

	// run as client
	if(isClient)
	{
    	modId = cmd->getModId();
		if(modId == "kernel")
		{
			return cmd->run(str,foundIndex+1, rslt,mModuleServer);
		}

    	// connect to cli deamon
		
		return (sendCmd(str, cmd, rslt, errMsg) == 0);
	}

	return cmd->run(str,foundIndex+1, rslt,mModuleServer);
}


bool
OmnCliProc::addCmds(const OmnString &filename)
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
	addCmd("syscmd");

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
				OmnCliCmd::eMaxCommandLen, true))
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

void		
OmnCliProc::setModuleCliServer(const AosModuleCliServerPtr	moduleServer)
{
	mModuleServer = moduleServer;
}

int 
OmnCliProc::saveConfig(OmnString &rslt)
{
	if (!mConfFile)
	{
		OmnString fn = mConfFileName;
		fn << ".active";
		mConfFile = OmnNew OmnFile(fn, OmnFile::eCreate);
		if (!mConfFile || !mConfFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << fn;
			return aos_alarm("%s", rslt.data());
		}
	}

	//
	// Configurations are saved in blocks, one for each module.
	// Module names are defined in sgModuleNames[].
	//
	int ret;
	int index = 0;
	while (strlen(sgModuleNames[index]) != 0)
	{
		if (saveConfig(sgModuleNames[index], rslt))
		{
			// 
			// Failed to save the configuration.
			//
			OmnTrace << rslt << endl;
		}

		index++;
	}

	// 
	// Back up the current config
	//
	OmnFile file(mConfFileName, OmnFile::eReadOnly);
	OmnString cmd;
	if (file.isGood())
	{
		OmnString newFn = mConfFileName;
		newFn << ".bak";
		cmd << "cp " << mConfFileName << " " << newFn;
		ret = system(cmd);
		OmnTrace << "To run command: " << cmd << "Return: " << ret << endl;
	}

	mConfFile->closeFile();
	mConfFile = 0;

	cmd = "mv -f ";
	cmd << mConfFileName << ".active " << mConfFileName;
	ret = system(cmd);
	OmnTrace << "To run command: " << cmd << "Return: " << ret << endl;
	
	rslt = "";
	return 0;
}

int 
OmnCliProc::saveConfig(const char *moduleName, OmnString &rslt)
{
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
	// This function saves one block identified by 'moduleName'.
	//

	OmnString cmd = moduleName;
	cmd << " save config";
	int ret = 0;

	OmnString contents;
	if (!OmnCliProc::getSelf()->runCliAsClient(cmd, rslt))
	{
		rslt = "Failed to save config for module: ";
		rslt << moduleName;
		ret = -eAosAlarm_FailedToSaveConfig;
		contents = "";
	}
	else
	{
		contents = rslt;
		rslt = "";
	}

	char local[200];
	sprintf(local, "------AosCliBlock: %s------\n", moduleName);
	if (!mConfFile->put(local, false) ||
		!mConfFile->put("<Commands>\n", false) ||
	    !mConfFile->put(contents, false) ||
		!mConfFile->put("</Commands>\n", false) ||
		!mConfFile->put("------EndAosCliBlock------\n\n", false))
	{
		rslt = "Failed to write to config: ";
		rslt << mConfFileName;
		ret = aos_alarm("%s", rslt.data());
	}

	return ret;
}

// 
// This function loads configurations from the configure
// file 'mFileName'. The caller should have set the file
// name already. Configurations are organized in blocks, 
// one for each module. If one module fails to load the
// configuration, it will abort the operation. 
//
// If errors occur, the error messages are returned through
// 'rslt'.
//
int 
OmnCliProc::loadConfig(OmnString &rslt)
{
	if (!mConfFile)
	{
		mConfFile = OmnNew OmnFile(mConfFileName, OmnFile::eReadOnly);
		if (!mConfFile)
		{
			rslt = "Failed to open configure file: ";
			rslt << mConfFileName;
			return -eAosAlarm_FailedToOpenFile;
		}

		if (!mConfFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << mConfFileName;
			return -eAosAlarm_FailedToOpenFile;
		}
	}

	// 
	// Configurations are saved in blocks, one for each module.
	// These are stored in "sgModuleNames[]". 
	//
	int ret;
	int index = 0;

	OmnCliCmd::setLoadConfig(true);
	while (sgModuleNames[index][0] != 0)
	{
		if ((ret = loadConfig(sgModuleNames[index], rslt))) 
		{
			// 
			// Failed to load the configure for the module. 
			//
			OmnTrace << rslt << endl;
		}

		index++;
	}

	OmnCliCmd::setLoadConfig(false);
	mConfFile->closeFile();
	mConfFile = 0;

	return 0;
}

int 
OmnCliProc::loadConfig(const char *moduleName, OmnString &rslt)
{
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
	
	OmnString contents;
	OmnString start = "------AosCliBlock: ";
	start << moduleName << "------\n";
	OmnString end = "------EndAosCliBlock------\n";
	bool err;

	if (!mConfFile->readBlock(start, end, contents, err))
	{
		if (err)
		{
			rslt << "********** Failed to read config for: ";
			rslt << moduleName << "\n";
			return -eAosAlarm_FailedToReadFile;
		}

		return 0;
	}

	try
	{
		// 
		// Clear the module's config first
		//
		OmnString cmd = moduleName;
		cmd << " clear config";

		if (!OmnCliProc::getSelf()->runCliAsClient(cmd, rslt))
		{
			cout << "Failed to clear the module's config: " << cmd 
				<< ". Error: " << rslt << endl;
		}
		
		OmnXmlItem config(contents);
		config.reset();
		while (config.hasMore())
		{
			OmnXmlItemPtr item = config.next();
			try
			{
				cmd = item->getStr();
				OmnTrace << "Found command: " << cmd << endl;
				if (!OmnCliProc::getSelf()->runCliAsClient(cmd, rslt))
				{
					OmnTrace << "Failed to run command: " << cmd 
						<< ". Error: " << rslt << endl;
					
					return -eAosRc_Fatal;
				}
			}

			catch (const OmnExcept &e)
			{
				rslt = "Failed to read command: ";
				rslt << item->toString();
				return -eAosRc_Fatal;
			}
		}
	}

	catch (const OmnExcept &e)
	{
		OmnTrace << "Failed to load configure for module: " 
			<< moduleName << endl;
		return -eAosRc_Fatal;
	}

	return 0;
}


int 
OmnCliProc::clearConfig(OmnString &rslt)
{
	//
	// Configurations are saved in blocks, one for each module.
	// Module names are defined in sgModuleNames[].
	//
	int ret;
	int index = 0;
	while (strlen(sgModuleNames[index]) != 0)
	{
		if ((ret = clearConfig(sgModuleNames[index], rslt))) 
		{
			OmnTrace << rslt << endl;
		}

		index++;
	}
	return 0;
}


int 
OmnCliProc::clearConfig(const char *moduleName, OmnString &rslt)
{
	// this function clear all the module's configuration

	OmnString cmd = moduleName;
	cmd << " clear config";

	if (!OmnCliProc::getSelf()->runCliAsClient(cmd, rslt))
	{
		if (rslt.length())
			rslt << "\nFailed to clear config:";
		else
			rslt = "Failed to clear config:";
		rslt << moduleName;
		
		return -eAosAlarm_FailedToSaveConfig;
	}

	return 0;
}


int
OmnCliProc::sendCmd(const OmnString& strCmd, OmnCliCmd* cmd, OmnString& rslt, OmnString& errMsg)
{
	OmnConnBuffPtr connBuffPtr;
	OmnString mutexName("cliClient");
	const int dftId = 1;
	char* tmp;
	bool isConnBroken = false;
	
try_again:
	if (!mConnClient)
	{
#if 0
		mConnClient = OmnNew OmnTcpClient(OmnIpAddr(cmd->getModAddr().data()), 
						  cmd->getModPort(),
						  1,
						  OmnIpAddr("localhost"),
						  0,
						  1,
						  mutexName);
#else	
		mConnClient = OmnNew OmnTcpClient("CliClient", "/var/run/.clisock", OmnTcp::eNoLengthIndicator); 
#endif
		if (!mConnClient)
		{
			cout << "Can't connect to server: " << errMsg << endl;
			
			OmnTrace << "Can't connect to server: " << errMsg << endl;
			return -1;
		}
		
		mConnClient->setClientId(dftId);
		if(!mConnClient->connect(errMsg))
		{
			OmnTrace << "Can't connect to server: " << errMsg << endl;
			cout << "Can't connect to server: " << errMsg << endl;
			
			mConnClient = NULL;
			return -1;
		}
	}
	
	if (!mConnClient->isConnGood())
	{
		mConnClient->closeConn();
		mConnClient = NULL;
		
		OmnTrace << "Try to connect to server" << endl;
		goto try_again;
	}
	
	if (!mConnClient->writeTo(strCmd.getBuffer(), strCmd.length()))
	{
		cout << "Failed to send command: " << strCmd << endl;
		
		mConnClient->closeConn();
		mConnClient = NULL;
		return -1;
	}
	
	bool isEndOfFile;
	if (!mConnClient->readFrom1(connBuffPtr, isConnBroken, isEndOfFile, false))
	{
		cout << "Failed to receive command" << endl;
		
		mConnClient->closeConn();
		mConnClient = NULL;
		return -1;
	}
	
	rslt = connBuffPtr->getString();
	tmp = rslt.getBuffer() + rslt.find(0, false) + 1; 
	return *((int*)tmp);
}


