////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2007-05-30: Created by Frank
////////////////////////////////////////////////////////////////////////////

#include "AmUtil/AmExternalSvr.h"
#include "AmUtil/AmRequestTypeId.h"
#include <string.h>


AosAmExternalSvr::AosAmExternalSvr(const char *name, 
				 const u16 	server_type_id, 
				 const char *ip, 
				 const int 	port, 
				 const char *connStr, 
				 const char *desc)
:
mTypeId(server_type_id),
mPort(port)
{
	if(!AosAmRequestSvrTypeId::isValid(server_type_id))
	{
		mTypeId = AosAmRequestSvrTypeId::eSvrTypeLocal;
	}

	if(name)
	{
		strncpy(mName, name, 30);
		mName[29] = 0;
	}
	else
	{
		mName[0] = 0;
	}

	if(ip)
	{
		strncpy(mIp, ip, 128);
		mIp[127] = 0;
	}
	else
	{
		mIp[0] = 0;
	}

	if(connStr)
	{
		strncpy(mConnStr, connStr, 256);
		mConnStr[255] = 0;
	}
	else
	{
		mConnStr[0] = 0;
	}

	if(desc)
	{
		strncpy(mDesc, desc, 256);
		mDesc[255] = 0;
	}
	else
	{
		mDesc[0] = 0;
	}
}


AosAmExternalSvr::~AosAmExternalSvr()
{}

bool AosAmExternalSvr::setMsg(const int msgId, const int priority)
{
	if(msgId < 1 || msgId >= AosAmMsgId::eMaxMsgId)
	{
		return false;
	}
	if(priority < 0 || priority > 5)
	{
		return false;
	}
	mMsg[msgId].mPriority = priority;
	return true;
}


bool AosAmExternalSvr::removeMsg(const int msgId)
{
	if(msgId < 1 || msgId >= AosAmMsgId::eMaxMsgId)
	{
		return false;
	}
	mMsg[msgId].mPriority = -1;
	return true;
}


bool AosAmExternalSvr::isMsgProcess(const int msgId)
{
	if(msgId < 1 || msgId >= AosAmMsgId::eMaxMsgId)
	{
		return false;
	}
	return (mMsg[msgId].mPriority >= 0);
}


int AosAmExternalSvr::getMsgProcessPriority(const int msgId)
{
	if(msgId < 1 || msgId >= AosAmMsgId::eMaxMsgId)
	{
		return -1;
	}
	return mMsg[msgId].mPriority;
}


u16 
AosAmExternalSvr::getServerTypeId()
{
	return mTypeId;
}


bool 
AosAmExternalSvr::getServerName(char *name, int &len)
{
	if(!name)
	{
		len = 30;
		return false;
	}
	if(len < (int)strlen(mName))
	{
		len = strlen(mName) + 1;
		return false;
	}
	strncpy(name, mName, len);
	return true;
}


char * 	
AosAmExternalSvr::getServerName() 
{
	return mName;
}
