////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliClient.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "CliClient/CliClient.h"


#include "aos/aosReturnCode.h"
#include "KernelInterface/CliCmd.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/StrParser.h"
#include "Util1/Wait.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"



AosCliClient::AosCliClient()
:
mLock(new OmnMutex()),
mCondVar(new OmnCondVar())
{
	mHbMsg = "heartbeat\n";
}

AosCliClient::~AosCliClient()
{
	
}


bool		
AosCliClient::start()
{
	mCmdTree = aosCharPtree_create();
	mCallbackTree = aosCharPtree_create();

	if(!addCmds(mFilename))
		addCmds("/usr/local/AOS/Bin/cmd.txt");

	return true;
}


bool		
AosCliClient::stop()
{
	return true;
}


OmnRslt		
AosCliClient::config(const OmnXmlParserPtr &conf)
{

	OmnXmlItemPtr client = conf->tryItem(OmnXmlItemName::eCliClientConfig);
	if(!client || !setTcpClient(client))
	{
		return false;
	}

	OmnThreadedObjPtr thisPtr(this, false);
	mThread = new OmnThread(thisPtr, "CliClient", 0, true, true);
	mThread->start();

	mFilename = client->getStr(OmnXmlItemName::eCmdFileName,"");
	if(mFilename == "")
	{
		mFilename = "cmd.txt";
	}
	return true;
}

bool			
AosCliClient::setTcpClient(OmnXmlItemPtr &client_config)
{
	OmnIpAddr 	remoteIpAddr  	= client_config->getIpAddr(OmnXmlItemName::eRemoteIpAddr, OmnIpAddr::eInvalidIpAddr);
	int			remotePort 		= client_config->getInt(OmnXmlItemName::eRemotePort, -1);
	int 		remoteNumPorts	= client_config->getInt(OmnXmlItemName::eRemoteNumPorts, 1);
	OmnIpAddr 	localIpAddr  	= client_config->getIpAddr(OmnXmlItemName::eLocalAddr, OmnIpAddr::eInvalidIpAddr);
	int			localPort 		= client_config->getInt(OmnXmlItemName::eLocalPort, -1);
	int 		localNumPorts	= client_config->getInt(OmnXmlItemName::eLocalNumPorts, 1);
	if(!remoteIpAddr.isValid() 	|| 
	   !localIpAddr.isValid() 		||
	   remotePort <= 0			||
	   localPort <= 0)
	{
		OmnTrace << "Incorrect params" 
			<< client_config->toString() << endl;
		return false;
	}

	mLock->lock();
	mNewClientConn = new OmnTcpClient(remoteIpAddr,
									 remotePort,
									 remoteNumPorts,
									 localIpAddr,
									 localPort,
									 localNumPorts,
									 "CliClient");
	OmnTcpMsgReaderPtr thisPtr(this, false);
	mNewClientConn->setMsgReader(thisPtr);
	mLock->unlock();
	return true;
}


bool	
AosCliClient::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	
 	OmnTrace << "Enter AosCliClient::threadFunc." << endl;
	while ( state == OmnThrdStatus::eActive )
	{
		//
		// If we set a new client, replace the older one
		//
		mLock->lock();
		if(mNewClientConn)
		{
			mClientConn = mNewClientConn;
			mNewClientConn = 0;

			OmnString err;
			mClientConn->closeConn();
			if(!mClientConn->connect(err))
			{
				OmnTrace << "can not connect to server:" << err << endl;
			}
		}
		mLock->unlock();
		
		// 
		// If the client conn is null , wait for while
		//
		if(!mClientConn)
		{
			OmnWaitSelf->wait(eTcpWaitTime,0);
			continue;
		}

		// 
		// If the client conn is broken,we reconnect it ,if still broken, wait for while
		//
		if(!mClientConn->isConnGood())
		{
			OmnString err;
			mClientConn->closeConn();
			if(!mClientConn->connect(err))
			{
				OmnTrace << "can not connect to server:" << err << endl;
				mLock->lock();
				mThreadStatus = true;
				mCondVar->wait(mLock);
				mThreadStatus = true;
				mLock->unlock();
				continue;
			}
		}

		// read on client conn, if get data, process it.
		OmnConnBuffPtr readBuff;
		bool	timeout = false;
		bool	connBroken = false;
		mClientConn->readFrom(readBuff,eAosCliClient_thread_waiting_time,timeout,connBroken);
		if(readBuff)
		{
			procCli(readBuff);
		}
		else
		{
//			mClientConn->writeTo(mHbMsg.getBuffer(),mHbMsg.length());	
			if(!mClientConn->isConnGood())
			{
				OmnString err;
				mClientConn->closeConn();
				if(!mClientConn->connect(err))
				{
					OmnTrace << "can not connect to server:" << err << endl;
					continue;
				}
			}
		}
	}

	OmnTrace << "Leaving AosCliClient::threadFunc." << endl;
	return true;
}

bool	
AosCliClient::procCli(const OmnConnBuffPtr msg)
{
	// 1. trim '\r' , '\n'
	trimEmptyLine(msg);	
	// 2. runcli
	OmnString rslt;
	runCli(msg,rslt);
		
	// 3. send back the response.
	rslt << "\n\n";
	if(!mClientConn->isConnGood())
	{
		OmnString err;
		mClientConn->closeConn();
		if(!mClientConn->connect(err))
		{
			OmnTrace << "can not connect to server:" << err << endl;
			return false;
		}
	}
	
	return mClientConn->writeTo(rslt.data(),rslt.length());	
}

bool	
AosCliClient::trimEmptyLine(const OmnConnBuffPtr msg)
{
	char *startPos = msg->getBuffer();
	char *endPos  = startPos +msg->getDataLength() - 1 ;
	char *curPos = endPos;
	while(curPos >= startPos)
	{
		if(*curPos == '\r' || *curPos == '\n')
		{
			curPos --;
			continue;
		}
		msg->setDataLength(curPos - startPos + 1);
		return true;
	}
	return false;
}

bool	
AosCliClient::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}

bool    
AosCliClient::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

OmnString	
AosCliClient::getTcpMsgReaderName() const
{
	return "CliClient";
}

int			
AosCliClient::nextMsg(const OmnConnBuffPtr &buff,
		   const OmnTcpClientPtr &conn)
{
	char *startPos = buff->getBuffer();
	char *curPos = startPos;
	char *endPos  = startPos +buff->getDataLength();
	
	int	emptyLineNum = 0;
	while(curPos <= endPos )
	{
		switch(*curPos)
		{
			case '\n':
				if(emptyLineNum > 0)// end of a whole message
				{
					int msgLen = curPos - startPos;
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

bool
AosCliClient::addCliFunc(const aosCliFunc &func , const OmnString &opr_id)
{
	if (aosCharPtree_insert(mCallbackTree, opr_id.data(), 
			opr_id.length(), (void *)func, 1))
	{
		OmnAlarm << "Failed to add callback func:" << opr_id << enderr;
		return false;
	}
	return true;
}

bool
AosCliClient::addCmd(const OmnString &def)
{
	if(!mCmdTree)
	{
		OmnAlarm << "The Cmd Tree is not init." << enderr;
		return false;
	}

	OmnCliCmd *cmd = OmnNew OmnCliCmd(def);
	OmnString rslt;
	if (!cmd->checkDef(rslt))
	{
		OmnAlarm << "Command not good: " << rslt << enderr;
		return false;
	}
	
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
AosCliClient::addCmds(const OmnString &filename)
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



// 
// This function is called when the client receives a complete 
// command from the connection.
//
bool 
AosCliClient::runCli(const OmnConnBuffPtr &msg, OmnString &rslt)
{
	// 
	// Now, we have put all data into 'data'. We need to convert it to 
	// 
	
	// 
	// 1. Look up the command
	//    This is the same as OmnCliProc::runCli();

	//
	// As the result, we found an OmnCliCmd.
	//
	int foundIndex_cmd;
	OmnCliCmd *cmd = 0;

	aosCharPtree_get(mCmdTree, msg->getBuffer(), &foundIndex_cmd, (void **)&cmd);
	if (!cmd)
	{
		rslt << "Command not found (userland)";
		return false;
	}

	// 
	// 2. Call OmnCliCmd::runApp(...);
	//
	char *data;
	unsigned int totalLen;
	OmnString str_parms(msg->getBuffer() + foundIndex_cmd+1 , msg->getDataLength() - foundIndex_cmd -1);

	// trim tail '\r'
	while(str_parms.length() > 0)
	{
		if(('\r' == *(str_parms.getBuffer()+str_parms.length())) ||
		   ('\n' == *(str_parms.getBuffer()+str_parms.length())))
		{
			str_parms.setLength( str_parms.length() - 1);
		}
		else
		{
			break;
		}
	}	

	if(!cmd->runAppPreparing(str_parms, &data, &totalLen,rslt))
	{
		// rslt string is written by cmd
		return false;
	}

	// 
	// 3. Convert the data into aosKernelApiParms
	//
	struct aosKernelApiParms parms;
	// same as aosKernelApi_convert
	convertParam(data, totalLen+1, &parms);

	
	// 
	// 4. Look up the function (refer to aos_core/aosKernelApi.cpp
	// 
	OmnString opr_id = cmd->getOprId();
	aosCliFunc func;   
	int foundIndex_func;
	aosCharPtree_get(mCallbackTree, opr_id.data(), &foundIndex_func, (void **)&func);
	// check whether the cmd is full fix . If it's longer,return cmd not found
	if(! foundIndex_func == opr_id.length() -1)
	{
		rslt << "Command not found (userland)";
		return false;
	}
	if(0 == func)
	{
		rslt << "Command not found (userland)";
		return false;
	}


	// 
	// Invoke the function
	//
	OmnString errmsg;
	int errlen = eCliFuncErrMsgLen;
	unsigned int inputLen = totalLen;
	int ret = func(data, &totalLen, &parms, errmsg.getBuffer(), errlen);
	if (ret)
	{
		rslt << "Command run error(userland)";
		return false;	
	}


	rslt = OmnString(data,totalLen);
	if (totalLen >= inputLen)
	{
		rslt << ". Returned data too long: " << totalLen 
			<< ". Max: " << inputLen << ". Truncated!";
	}
	
	return true;
}


int 
AosCliClient::convertParam(char *data,
				 			unsigned int datalen,
				 			struct aosKernelApiParms *parms)
{
	//
	// oprId
	// number of integers (integer)
	// number of strings (integer)
	// integer parm
	// integer parm
	// ...
	// string parm
	// string parm
	// ...
	//
	// Index counts from 1.
	//
	unsigned int i, ii, index;
	char *dd;
	int len = strlen(data);
	int *iv = (int *)&data[len+1];

	if ((ii = len + 1 + (iv[0] + 2) * 4) >datalen)
	{
		return eAosRc_IndexOutBound;
	}

	//
	// Retrieve the integers
	//
	parms->mNumIntegers = iv[0];
	parms->mNumStrings = iv[1];
	if (parms->mNumIntegers < 0 ||
		parms->mNumIntegers > eAosMaxKernelApiIntegers)
	{
		return eAosRc_IndexOutBound;
	}

	for (index = 0; index < parms->mNumIntegers; index++)
	{
		parms->mIntegers[index] = iv[index+2];
	}

	//
	// Retrieve the strings
	//

	dd = &data[ii];
	index = 0;
	for (i=0; i<parms->mNumStrings; i++)
	{
		parms->mStrings[index++] = &data[ii];

		ii += strlen(dd) + 1;
		if (ii > datalen)
		{
			return eAosRc_IndexOutBound;
		}
		dd = &data[ii];
	}

	return 0;
}

