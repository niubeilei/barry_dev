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
// 09/25/2012	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocClient_DocidShufflerMgr_h
#define AOS_DocClient_DocidShufflerMgr_h

#include "DocClient/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"

OmnDefineSingletonClass(AosDocidShufflerMgrSingleton,
						AosDocidShufflerMgr,
						AosDocidShufflerMgrSelf,
						OmnSingletonObjId::eDocidShufflerMgr,
						"DocidShufflerMgr");


class AosDocidShufflerMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	map<OmnString, AosDocidShufflerPtr> 	mScanner;
	OmnMutexPtr								mLock;

public:
	AosDocidShufflerMgr();
	~AosDocidShufflerMgr();

    // Singleton class interface
    static AosDocidShufflerMgr *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	bool	shuffle(
			const OmnString &scanner_id,
			const AosBitmapObjPtr &bitmap,
			const int num_thrds,
			const AosRundataPtr &rdata);

	bool	shuffle(
			const OmnString &scanner_id,
			const AosBitmapObjPtr &bitmap,
			const int num_thrds,
			const AosDocClientCallerPtr &caller,
			const AosRundataPtr &rdata);

	bool	sendStart(
			const OmnString &scanner_id,
			const u64 &block_size,
			const AosXmlTagPtr &field_names,
			const AosRundataPtr &rdata);

	bool	sendStart(
			const OmnString &scanner_id,
			const u64 &block_size,
			const AosRundataPtr &rdata);

	bool	sendStart(
			const OmnString &scanner_id,
			const u32 &queue_size,
			const OmnString &read_policy,
			const u64 &block_size,
			const OmnString &batch_type,
			const AosXmlTagPtr &field_names,
			const AosRundataPtr &rdata);

	bool	sendFinished(
			const OmnString &scanner_id,
			const AosRundataPtr &rdata);
};
#endif
