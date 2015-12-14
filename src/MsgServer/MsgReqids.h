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
//
// Modification History:
// 06/15/2011	Created by Joshi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_MsgServer_MsgReqids_h
#define Omn_MsgServer_MsgReqids_h

#include "Util/String.h"

#define AOSMSPROC_CONNECT				"connect"
#define AOSMSPROC_DISCONNECT			"disconnect"
#define AOSMSPROC_GETMESSAGE			"getmessage"
#define AOSMSPROC_SENDMSG				"sendmsg"

class AosMsgReqid
{
public:
	enum E 
	{
		eInvalid,

		eConnect,
		eDisConnect,
		eGetMessage,
		eSendMsg,

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static bool addName(const OmnString &name, const E e);
};
#endif

