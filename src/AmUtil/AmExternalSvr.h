////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
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
#ifndef Aos_AmUtil_AmExternalSvr_h
#define Aos_AmUtil_AmExternalSvr_h

#include "aosUtil/Types.h"
#include "AmUtil/AmMsgId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosAmExternalSvr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	typedef struct MsgTypeEntry
	{
//		int		mMsgId;		// corresponding message Id
		int		mPriority; 	// corresponding message process priority

		// If the mPriority < 0, means this entry don't support any message processing
		MsgTypeEntry()
		:
		mPriority(-1)
		{
		}
	}MsgTypeEntry_t;
private:
	char 					mName[30];
	u16 					mTypeId;
	char 					mIp[128];
	int 					mPort;
	char 					mConnStr[256];
	char 					mDesc[256];
	MsgTypeEntry_t 			mMsg[AosAmMsgId::eMaxMsgId];

public:
	AosAmExternalSvr(const char *name, 
					 const u16 	server_type_id, 
					 const char *ip, 
					 const int port, 
					 const char *connStr, 
					 const char *desc);
	~AosAmExternalSvr();

	bool 	setMsg(const int msgId, const int priority);
	bool 	removeMsg(const int msgId);
	bool 	isMsgProcess(const int msgId);
	int 	getMsgProcessPriority(const int msgId);
	u16 	getServerTypeId();
	bool 	getServerName(char *name, int &len);
	char * 	getServerName();
	// return number dot format string
	char * 	getServerIp() {	return mIp;};
	// return port number
	int 	getServerPort() {	return mPort;};
	// return port number
	char *	getConnStr() {	return mConnStr;};
};


#endif // Aos_AmUtil_AmExternalSvr_h

