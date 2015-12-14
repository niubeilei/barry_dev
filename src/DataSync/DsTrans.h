////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/12/2010	Created by Chen Ding
// 2014/01/28 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_DataSync_DsTrans_h
#define AOS_DataSync_DsTrans_h

#include "DataSync/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"

class AosDsTrans : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	OmnMutexPtr     mLock;
	u64				mTransId;
	OmnString       mRequest;
	u64				mTime;
	AosDataSyncCltPtr mDataSync;

public:
	AosDsTrans(const AosDataSyncCltPtr &data_sync, const OmnString &request);
	~AosDsTrans();

	u64		getTransId() const {return mTransId;}
	bool    sendRequest(u32 &logid);
};
#endif
#endif
