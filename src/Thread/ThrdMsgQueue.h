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
// 4/6/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Thread_ThrdMsgQueue_h
#define Aos_Thread_ThrdMsgQueue_h

#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "Util/DynArray.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"



class AosThrdMsgQueue : virtual public OmnRCObject
{
	OmnDefineRCObject;

private: 
	struct Requester
	{
		OmnMutexPtr		mLock;
		OmnCondVarPtr	mCondVar;
		u32				mTransId;
		bool			mUsed;
		OmnThrdTransPtr	mTrans;
	};

	struct Entry
	{
		u32 			mTransId;
		OmnConnBuffPtr	mBuff;

		Entry(const OmnConnBuffPtr &buff, const u32 transId)
		:
		mTransId(transId),
		mBuff(buff)
		{
		}

		Entry()
		:
		mTransId(0)
		{
		}
	};

	OmnMutexPtr				mLock;	
	OmnVList<Entry> 		mMsgs;
	OmnMutexPtr				mRequesterLock;
	OmnDynArray<Requester, 100, 100, 10000>	mRequesters;

public:
	AosThrdMsgQueue();
	~AosThrdMsgQueue();

	bool 	waitForMsg(const OmnThrdTransPtr &trans, const u32 timer);
	bool	msgRcved(const OmnConnBuffPtr &buff, const u32 transId);
	
private:
};
#endif

