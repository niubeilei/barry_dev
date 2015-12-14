////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PMIServer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "PMIServer/PMIServer.h"

#include "CliClient/CliClient.h"
#include "CliClient/ModuleOprId.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "TcpAppServer/TcpAppServer.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/Ptrs.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"
#include <netdb.h>

OmnSingletonImpl(AosPMIServerSingleton,
				 AosPMIServer,
				 AosPMIServerSelf,
				 "AosPMIServer");

AosPMIServer::AosPMIServer()
:
mStartFlag(true),
mRespFlag(true)
{
	mAmmConfig.AZN_C_ORG_NAME 		= "AZN_ORG_NAME" ;
	mAmmConfig.AZN_C_APPSYS_NAME 	= "AZN_APPSYS_NAME";
    mAmmConfig.src_orig_prefix		= "org";
    mAmmConfig.src_sys_prefix		= "sys";
    mAmmConfig.src_prefix           = "obj";
    mAmmConfig.opr_prefix           = "opr";

	mServer = new AosTcpAppServer();
	OmnTcpMsgReaderPtr thisPtr(this, false);
	mServer->setReader(thisPtr);
}


AosPMIServer::~AosPMIServer()
{
}


bool
AosPMIServer::equal_to_AMM_MSG(const AMM_MSG &msg1,const AMM_MSG &msg2)
{
	return (msg1.userOrig ==   msg2.userOrig&&   
			msg1.userSys ==    msg2.userSys&&    
			msg1.userSn ==     msg2.userSn&&     
			msg1.srcOrig ==    msg2.srcOrig&&    
			msg1.srcSys ==     msg2.srcSys&&     
			msg1.src ==        msg2.src&&        
			msg1.operation ==  msg2.operation);
}

OmnString
AosPMIServer::getTcpMsgReaderName() const
{
	return "PMIServer";
}

int			
AosPMIServer::nextMsg(const OmnConnBuffPtr &buff,
					    const OmnTcpClientPtr &conn)
{

	// If we get a whole msg , call procMsg()
	char *startPos = buff->getBuffer();
	unsigned int  dataLen = buff->getDataLength();

	if(dataLen < sizeof(AMM_PROTO_HEADER))
	{
		// the data is less than a header
		return 0;
	}
	
	AMM_PROTO_HEADER *header = (AMM_PROTO_HEADER *)startPos;
	
	unsigned int msgLen = header->length;
	if(msgLen <= dataLen)
	{
		procMsg(startPos,msgLen,conn);
		return msgLen;
	}
	return 0;
}

bool
AosPMIServer::getMsgBody(AMM_MSG &msgBody,const char *msg,const int msgLen)
{
	int strLen = 0;
	int lenLeft = msgLen;

	const char *curPos = msg;

	curPos += sizeof(AMM_PROTO_HEADER);

	// 1. get ORG_NAME prefix
	OmnString orig_name_prefix;
	if(!getStringBy(orig_name_prefix,curPos,lenLeft,'\0' ))return false;
	if(!(orig_name_prefix == mAmmConfig.AZN_C_ORG_NAME)) return false;
	strLen = orig_name_prefix.length()+1;
	curPos += strLen;
	lenLeft -= strLen;

	// 2. get org_name 
	if(!getStringBy(msgBody.userOrig,curPos,lenLeft,'\0' ))return false;
	strLen = msgBody.userOrig.length()+1;
	curPos += strLen;
	lenLeft -= strLen;
	
	// 3. get APPSYS_NAME prefix
	OmnString sys_name_prefix;
	if(!getStringBy(sys_name_prefix,curPos,lenLeft,'\0' ))return false;
	if(!(sys_name_prefix == mAmmConfig.AZN_C_APPSYS_NAME)) return false;
	strLen = sys_name_prefix.length()+1;
	curPos += strLen;
	lenLeft -= strLen;

	// 4. get appsys_name 
	if(!getStringBy(msgBody.userSys,curPos,lenLeft,'\0'  ))return false;
	strLen = msgBody.userSys.length()+1;
	curPos += strLen;
	lenLeft -= strLen;
	
	// 5. get sn
	if(!getStringBy(msgBody.userSn,curPos,lenLeft,'\0'   ))return false;
	strLen = msgBody.userSn.length()+1;
	curPos += strLen;
	lenLeft -= strLen;

/*
	if(lenLeft < ePMIServer_SN_Length)return false;
	unsigned char snChar = 0;
	for(int i = 0;i < ePMIServer_SN_Length;i++)
	{
		snChar = *((unsigned char*)curPos) >> 4;
		if(snChar < 10)
		{
			snChar += 48;
		}
		else
		{
			snChar += 54;
		}
		msgBody.userSn << snChar;

		snChar = *((unsigned char*)curPos);
		if(snChar < 10)
		{
			snChar += 48;
		}
		else
		{
			snChar += 54;
		}
		msgBody.userSn << snChar;

		curPos ++;
	}
	curPos ++;
	lenLeft -= ePMIServer_SN_Length;
*/	
	
	// 6. get SRC_ORG prefix
	OmnString src_orig_prefix;
	if(!getStringBy(src_orig_prefix,curPos,lenLeft,'='  ))return false;
	if(!(src_orig_prefix == mAmmConfig.src_orig_prefix)) return false;
	strLen = src_orig_prefix.length()+1;
	curPos += strLen;
	lenLeft -= strLen;

	// 7. get src_org 
	if(!getStringBy(msgBody.srcOrig,curPos,lenLeft,','  ))return false;
	strLen = msgBody.srcOrig.length()+1;
	curPos += strLen;
	lenLeft -= strLen;
	
	// 8. get SRC_SYS prefix
	OmnString src_sys_prefix;
	if(!getStringBy(src_sys_prefix,curPos,lenLeft,'='  ))return false;
	if(!(src_sys_prefix == mAmmConfig.src_sys_prefix)) return false;
	strLen = src_sys_prefix.length()+1;
	curPos += strLen;
	lenLeft -= strLen;

	// 9. get src_sys 
	if(!getStringBy(msgBody.srcSys,curPos,lenLeft,','   ))return false;
	strLen = msgBody.srcSys.length()+1;
	curPos += strLen;
	lenLeft -= strLen;
	
	// 10. get SRC prefix
	OmnString src_prefix;
	if(!getStringBy(src_prefix,curPos,lenLeft,'='  ))return false;
	if(!(src_prefix == mAmmConfig.src_prefix)) return false;
	strLen = src_prefix.length()+1;
	curPos += strLen;
	lenLeft -= strLen;
	
	// 11. get src 
	if(!getStringBy(msgBody.src,curPos,lenLeft,'\0'      ))return false;
	strLen = msgBody.src.length()+1;
	curPos += strLen;
	lenLeft -= strLen;
	
	// 12. get opr prefix
	OmnString opr_prefix;
	if(!getStringBy(opr_prefix,curPos,lenLeft,'='  ))return false;
	if(!(opr_prefix == mAmmConfig.opr_prefix)) return false;
	strLen = opr_prefix.length()+1;
	curPos += strLen;
	lenLeft -= strLen;

	// 13. get opr 
	if(!getStringBy(msgBody.operation,curPos,lenLeft,'\0'))return false;
	strLen = msgBody.operation.length()+1;
	curPos += strLen;
	lenLeft -= strLen;
	
	return true;
}

bool
AosPMIServer::checkPermission(AMM_MSG &msgBody)
{
	showAMMMsg(msgBody);
	
	AMM_MSG *msgFound = 0;
	OmnString key(msgBody.userSn);
	key << msgBody.src << msgBody.operation << msgBody.userOrig << msgBody.userSys;
	int index = 0;
	bool ret = aosCharPtree_get(mPermTree, key.data(), &index, (void **)&msgFound);
	if (ret || !msgFound)
	{
		cout << "can not find the object" << endl;
		return false;
	}
	showAMMMsg(*msgFound);
	if(index+1 == key.length())
	{
		// check status
		return equal_to_AMM_MSG(*msgFound,msgBody);
	}
	else // longer or shorter than key
	{
		cout << "find it ,but length error" << endl;
		return false;
	}
	
	return false;
}


void 
AosPMIServer::showAMMMsg(const AMM_MSG &msg)
{
	cout << "userOrig    =" << msg.userOrig  << endl;   
	cout << "userSys     =" << msg.userSys   << endl;   
	cout << "userSn      =" << msg.userSn    << endl;   
	cout << "srcOrig     =" << msg.srcOrig   << endl;   
	cout << "srcSys      =" << msg.srcSys    << endl;   
	cout << "src         =" << msg.src       << endl;   
	cout << "operation   =" << msg.operation << endl;
}

bool
AosPMIServer::addPermission(AMM_MSG * msgBody)
{
	
	// insert into ptree
	if(!msgBody)
	{
		return false;
	}
	
	OmnString key(msgBody->userSn);
	key << msgBody->src << msgBody->operation<< msgBody->userOrig << msgBody->userSys;


	if (aosCharPtree_insert(mPermTree, key.data(), 
			key.length(), (void *)msgBody, 1))
	{
		OmnAlarm << "Failed to add command" << enderr;
		return false;
	}

	return true;
}

bool			
AosPMIServer::getStringBy(OmnString &str,const char *msg,const int msgLen,const char stopchar)
{
	const char* startPos = msg;
	const char* curPos = startPos;
	const char* endPos = msg + msgLen;
	while (curPos <= endPos)
	{
		if(stopchar == *curPos)
		{
			str = OmnString(startPos,curPos - startPos);
			return true;
		}
		curPos ++;
	}
	return false;
}


bool
AosPMIServer::procMsg(const char* msg,const int msgLen,const OmnTcpClientPtr &conn)
{
	// 
	// check the response flag
	//
	if(!mRespFlag)
	{
		return true;
	}
	

	bool permission = false;
	AMM_MSG msgBody;
	if(!getMsgBody(msgBody,msg,msgLen))
	{
		// Error format.
		return false;
	}
	
	//
	// check the permission
	//
	permission = checkPermission(msgBody);
	
	// create a response
	OmnString rslt(msg,sizeof(AMM_PROTO_HEADER) + sizeof(UINT32));
	AMM_PROTO_HEADER *resphdr = (AMM_PROTO_HEADER *)rslt.data();
	resphdr->length = sizeof(AMM_PROTO_HEADER) + sizeof(UINT32);
	if(permission)
	{
		resphdr->data[0] = 0;
		resphdr->content = 0;
	}
	else
	{
		resphdr->content = 0;
		resphdr->data[0] = 1;
	}

	// 2 send back the response
	if(!sendResp(rslt,conn))
	{
		OmnWarn << "send response failed: " << rslt << enderr;
		return false;
	}
	return true;
}

bool		
AosPMIServer::getCmd(const OmnConnBuffPtr &buff , OmnString &cmd)
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
AosPMIServer::sendResp(OmnString &rslt,const OmnTcpClientPtr &conn)
{
	if(mServer)
	{
		mServer->writeTo(rslt,conn);
	}
	return true;
}


bool
AosPMIServer::config(const OmnXmlParserPtr &conf)
{

	mPermTree = aosCharPtree_create();

	OmnXmlItemPtr def = conf->tryItem(OmnXmlItemName::ePMIServerConfig);
	if (!def)
	{
		return false;
	}
	
	AMM_MSG* amm_msg;

	OmnXmlItemPtr pmi_data = def->tryItem(OmnXmlItemName::ePMIData);
	if (pmi_data)
	{
		while (pmi_data->hasMore())
		{
			OmnXmlItemPtr data_block = pmi_data->next();
			amm_msg = new AMM_MSG;				
			amm_msg->userOrig 	= data_block->getStr(OmnXmlItemName::eUserOrg 	, "");
			amm_msg->userSys		= data_block->getStr(OmnXmlItemName::eUserSys	, "");
			amm_msg->userSn		= data_block->getStr(OmnXmlItemName::eUserSN	, "");
			amm_msg->srcOrig		= data_block->getStr(OmnXmlItemName::eSrcOrg	, "");
			amm_msg->srcSys		= data_block->getStr(OmnXmlItemName::eSrcSys	, "");
			amm_msg->src			= data_block->getStr(OmnXmlItemName::eSrc		, "");
			amm_msg->operation	= data_block->getStr(OmnXmlItemName::eOperation	, "");

	
			if(!addPermission(amm_msg))
			{
				return false;
			}
		}
		
	}

	
	OmnXmlItemPtr pmi_tcp_server = def->tryItem(OmnXmlItemName::ePMITcpServer);
	if(pmi_tcp_server.isNull())
	{
		return false;
	}

	OmnIpAddr localIP = pmi_tcp_server->getIpAddr(OmnXmlItemName::eLocalAddr,OmnIpAddr::eInvalidIpAddr);
	if(OmnIpAddr::eInvalidIpAddr == localIP)
	{
		// 
		// localIP not set
		//
    	char hostname[256] = "PMISERVER_ADDR";
		//gethostname(hostname,256);
		hostent * host = gethostbyname(hostname);
		if(host)
		{
			unsigned long iplong;
			memcpy(&iplong,*(host->h_addr_list),4);
			localIP = iplong;
		}
		else
		{
			// can not get local ip
			cout << "can not get local IP" << endl;
			return false;
		}
	}
	
	if(mServer)
	{
		return mServer->config(pmi_tcp_server,localIP);
	}
	else
	{
		cout << "mServer is Null" << endl;
	}
	
	return false;
}

bool
AosPMIServer::start()
{
	if(mServer)
	{
		return mServer->start();
	}
	return false;
	
}

bool
AosPMIServer::stop()
{
	return true;
}



bool			
AosPMIServer::setCliClient(const AosCliClientPtr cliClient)
{
	mCliClient = cliClient;
	return true;
}

int 
AosPMIServer::startModule(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	//
	// real operation
	//
	*optlen = 0;
	AosPMIServerSelf->setStartFlag(true);
	AosPMIServerSelf->setRespFlag(true);

	//
	// set response
	//
	OmnString resp;
	if(*optlen > 50)
	{
		resp = "PMI Module on";
	}
	else
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();
	return 0;
}


int 
AosPMIServer::stopModule(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	//
	// real operation
	//
	*optlen = 0;
	AosPMIServerSelf->setStartFlag(false);
	AosPMIServerSelf->setRespFlag(true);

	//
	// set response
	//
	OmnString resp;
	if(*optlen > 50)
	{
		resp = "PMI Module off";
	}
	else
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();
	return 0;
}


int 
AosPMIServer::responseOn(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	//
	// real operation
	//
	*optlen = 0;
	AosPMIServerSelf->setRespFlag(true);

	//
	// set response
	//
	OmnString resp;
	if(*optlen > 50)
	{
		resp = "PMI response on";
	}
	else
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();
	return 0;
}

int 
AosPMIServer::responseOff(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	//
	// real operation
	//
	*optlen = 0;
	AosPMIServerSelf->setRespFlag(false);

	//
	// set response
	//
	OmnString resp;
	if(*optlen > 50)
	{
		resp = "PMI response off";
	}
	else
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();
	return 0;
}

void		
AosPMIServer::setStartFlag(const bool flag)
{

	mStartFlag = flag;
	if(mServer)
	{
		mServer->setConnFlag(flag);
	}
	return;
}

void		
AosPMIServer::setRespFlag(const bool flag)
{
	mRespFlag = flag;
	return;
}

bool				
AosPMIServer::registerCliCallbackFunc()
{
	if(!mCliClient)
	{
		return false;
	}
	mCliClient->addCliFunc(startModule,AosModuleOprId::ePMIStartModule);
	mCliClient->addCliFunc(stopModule,AosModuleOprId::ePMIStopModule);
	mCliClient->addCliFunc(responseOn,AosModuleOprId::ePMIResponseOn);
	mCliClient->addCliFunc(responseOff,AosModuleOprId::ePMIResponseOff);
	return true;
}


			 

