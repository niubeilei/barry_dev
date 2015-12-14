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
// 03/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NetworkMgr_NetworkTrans_h
#define Aos_NetworkMgr_NetworkTrans_h

#include "NetworkMgr/NetworkTransType.h"
#include "NetworkMgrUtil/NetCommand.h"
#include "Rundata/Rundata.h"
#include "UtilData/ModuleId.h"
#include "XmlUtil/XmlTag.h"

class AosNetworkTrans : public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxServers = 1000000		// 1 million
	};

	OmnMutexPtr				mLock;
	u64						mTransId;
	int						mVirtualId;
	AosModuleId::E			mModuleId;
	AosNetCommand::E		mCommand;
	AosNetworkTransType::E 	mType;
	AosXmlTagPtr			mRequest;
	bool					mFinishedFlag;
	u32						mSendTimeSec;
	AosNetListenerPtr		mCaller;

public:
	AosNetworkTrans(
			const u64 &transid, 
			const AosXmlTagPtr &request,
			const u32 total_servers);
	AosNetworkTrans(
			const u64 &transid, 
			const int virtual_id,
			const AosModuleId::E module_id,
			const AosNetCommand::E command);
	~AosNetworkTrans();

	//bool isAllFinished() const
	//{
	//	mLock->lock();
	//	bool rslt = mTotalFinished >= mTotalServers;
	//	mLock->unlock();
	//	return rslt;
	//}

	//bool transFinished(
	//		const u32 sid, 
	//		bool &all_finished, 
	//		const AosRundataPtr &rdata);

	bool transAcked(
			const AosRundataPtr &rdata);

	bool transFinished(
			const AosRundataPtr &rdata);

	bool isTransFinished() const {return mFinishedFlag;}
};
#endif

