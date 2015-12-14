////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/04/19 Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_NotifyToClient_h
#define Aos_DocTrans_NotifyToClient_h

#include "TransUtil/TaskTrans.h"

class AosNotifyToClient : virtual public AosTaskTrans 
{

private:
	u64			mDocid;
	u32			mSiteid;

public:
	AosNotifyToClient(const bool regflag);
	AosNotifyToClient(
			const int serverid,
			const u64 &docid,
			const u32 siteid); 

	~AosNotifyToClient();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

