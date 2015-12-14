////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ModuleCliServer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "CliClient/ModuleCliServer.h"

#include "KernelInterface/CliProc.h"
#include "KernelInterface/CliCmd.h"
#include "TcpAppServer/TcpAppServer.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/File.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/Ptrs.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"

AosModuleCliServer::AosModuleCliServer()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mModulesNum(0),
mTimeOut(false)
{
	mServer = new AosTcpAppServer();

	OmnTcpMsgReaderPtr thisPtr(this, false);
	mServer->setReader(thisPtr);
}


AosModuleCliServer::~AosModuleCliServer()
{
}

OmnString
AosModuleCliServer::getTcpMsgReaderName() const
{
	return "ModuleCliServer";
}

int			
AosModuleCliServer::nextMsg(const OmnConnBuffPtr &buff,
					    const OmnTcpClientPtr &conn)
{

	// If we get a whole msg , call procMsg()
	char *startPos = buff->getBuffer();
	char *curPos = startPos;
	char *endPos = startPos + buff->getDataLength();

	int	emptyLineNum = 0;
	while(curPos <= endPos )
	{
		switch(*curPos)
		{
			case '\n':
				if(emptyLineNum > 0)// end of a whole message
				{
					int msgLen = curPos - startPos + 1;
					procMsg(OmnString(startPos,msgLen),conn);
					return msgLen;
				}
				else// only find one line
				{
					emptyLineNum ++;
				}
				break;
			case '\r':
				break;
			default:
				emptyLineNum = 0;
				break;
		}
		curPos ++;
	}
	return 0;
}

void		
AosModuleCliServer::procMsg(const OmnString & rslt,
		  				   const OmnTcpClientPtr &conn)
{
cout << "PROC:" << rslt;
	// 1 run the command
	if(rslt == "heartbeat\n")
	{
		return;
	}
	mLock->lock();
	mRslt = rslt;
	mCondVar->signal();
	mLock->unlock();

	return;
}

bool		
AosModuleCliServer::getCmd(const OmnConnBuffPtr &buff , OmnString &cmd)
{
	int msgLen = buff->getDataLength();
	char* msgHeader = buff->getBuffer();
	char *msgTail =  msgHeader + msgLen;
	while (msgLen > 0 && (*msgTail == '\r' || *msgTail == '\n'))
	{
		msgLen --;
		msgTail --;
	}
	cmd = OmnString(msgHeader, msgLen);
	return true;

}

bool		
AosModuleCliServer::sendResp(OmnString &rslt)
{
//	if(mServer)
//	{
//		mServer->writeTo(rslt,conn);
//	}
	mLock->lock();
	mRslt = rslt;
	mCondVar->signal();
	mLock->unlock();
	
//	mCondVar->wait(mLock);
	return true;
}


bool
AosModuleCliServer::config(const OmnXmlParserPtr &conf)
{
	mFilename = "cmd.txt";
	
	OmnXmlItemPtr def = conf->tryItem(OmnXmlItemName::eModuleCliServer);
	if (!def)
	{
		return false;
	}

	if(mServer)
	{
		mFilename = def->getStr(OmnXmlItemName::eCmdFileName,"cmd.txt");
		return mServer->config(def);
	}
	cout << "\nNo TcpAppServer\n" << endl;
	
	mFilename = "cmd.txt";
	return false;
}

bool
AosModuleCliServer::start()
{
	if(!addModules(mFilename))
		addModules("/usr/local/AOS/Bin/cmd.txt");
	
	if(mServer)
	{
		return mServer->start();
	}
	cout << "\nNo TcpAppServer\n" << endl;
	return false;
	
}

bool
AosModuleCliServer::runCli(const OmnString &modId, const OmnString &cmd,OmnString &rslt)
{
	OmnIpAddr addr = OmnIpAddr::eInvalidIpAddr;
	int port = -1;
	if(!getModule(modId,addr,port))
	{
		rslt << "can not find the module:" << modId;
		return false;
	}

	OmnTcpClientPtr client;

	mServer->getClientByRemoteAddr(addr,port,client);
	if(!client)
	{
		rslt = "The module is offline";
		return false;
	}

	mServer->writeTo(cmd,client);
	
	mLock->lock();
	mTimeOut = false;
	mLock->unlock();

	OmnTimerObjPtr thisPtr(this, false);
	OmnTimerSelf->startTimer("ModuleCliServer",eAosMCS_WaitTime,0,thisPtr,0);
	mLock->lock();
	mCondVar->wait(mLock);
	mLock->unlock();
cout << "==============" << endl;
cout << "rslt is :" << mRslt << endl;
cout << "==============" << endl;
	if(mTimeOut)
	{
		rslt = "Module timeout";
	}
	else
	{
		mLock->lock();
		rslt = mRslt;
		mLock->unlock();
	}	
	return true;
}

bool
AosModuleCliServer::getModule(const OmnString &modId,OmnIpAddr &addr , int &port)
{
	addr = OmnIpAddr::eInvalidIpAddr;
	port = -1;
	for(int i = 0;i < mModulesNum;i ++)
	{
		if(mModules[i].moduleName == modId)
		{
			addr = mModules[i].addr;
			port = mModules[i].port;
			return true;
		}
	}

	return false;
}



bool				
AosModuleCliServer::addModule(const OmnString &config)
{
	if(mModulesNum >= eAosMCS_MaxModuleNum)
	{
		return false;
	}

	OmnXmlItem theDef(config);
	OmnString 	moduleName	= theDef.getStr(OmnXmlItemName::eModId,"");
	OmnIpAddr 	remoteIpAddr 	= theDef.getIpAddr(OmnXmlItemName::eModAddr, OmnIpAddr::eInvalidIpAddr);
	int			remotePort 		= theDef.getInt(OmnXmlItemName::eModPort, -1);
	if(moduleName == "" ||
	   !remoteIpAddr.isValid() 	|| 
	   !remotePort > 0)
	{
		return false;
	}
	
	mModules[mModulesNum].moduleName = moduleName;
	mModules[mModulesNum].addr = remoteIpAddr;
	mModules[mModulesNum].port = remotePort;
	mModulesNum ++;
	
	return true;
}

bool				
AosModuleCliServer::removeModule(const OmnString &moduleName)
{
	bool findModule = false;
	for(int i = 0;i < mModulesNum;i ++)
	{
		if((!findModule) && moduleName == mModules[i].moduleName)
		{
			findModule = true;
			continue;
		}

		if(findModule && i > 0)
		{
			mModules[i-1].moduleName = mModules[i].moduleName ;
			mModules[i-1].addr = mModules[i].addr ;
			mModules[i-1].port = mModules[i].port ;
		}
	}

	return findModule;
}


bool
AosModuleCliServer::addModules(const OmnString &filename)
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
		if (!file.skipTo("<Mod>", false))
		{
			// 
			// Finished.
			//
			return true;
		}

		if (!file.readUntil(buffer, "</Mod>", 
				OmnCliCmd::eMaxCommandLen, true))
		{
			OmnAlarm << "Failed to read the next command after: " 
				<< buffer << enderr;
			return false;
		}

		// 
		// Read the command. Create the command now.
		// 
		addModule(buffer);
	}

}

void		
AosModuleCliServer::timeout(const int timerId, 
								const OmnString &timerName,
								void *parm)
{
	mLock->lock();
	mTimeOut = true;
	mCondVar->signal();
	mLock->unlock();
}





