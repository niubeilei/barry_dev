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
// This is a utility to select docs.
//
// Modification History:
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_CounterServer_StatProc_h
#define AOS_CounterServer_StatProc_h

#include "CounterServer/Ptrs.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/CounterOperations.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/BuffArray.h"
#include "Util/ValueRslt.h"
#include "Util/Orders.h"
#include "Util/Ptrs.h"

class AosStatProc : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eDftMaxDocs = 0 
		//eDftMaxDocs = 10000
	};

protected:
	AosCounterOperation::E  mType; 
	u64						mIILID;
	AosBuffArrayPtr			mValues;
	AosBuffPtr				mBuff;
	u32						mMaxDocs;
	OmnMutexPtr         	mLock;
	static bool				smShowLog; 
	AosValueRslt 			mValueRslt;


public:
	AosStatProc(const u64 &iilid, const AosCounterOperation::E &type);
	AosStatProc(){};
	~AosStatProc() {}

	virtual bool proc(const OmnString &counter_id, 
			const AosBuffPtr &buff, 
			AosVirCtnrSvr* vserver, 
			const AosRundataPtr &rdata) = 0;

	static AosStatProcPtr createStatProc(
				const u64 &iilid, 
				const AosOrder::E order,
				const AosCounterOperation::E &opr,
				const AosRundataPtr &rdata);

	u64 getIILID() const {return mIILID;}
	static void setShowLog(bool b) {smShowLog = b;}
};
#endif
