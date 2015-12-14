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
// 03/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocClient/DocidMgrNew.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/ExitHandler.h"
#include "DocServer/DocSvr.h"
#include "IILUtil/IILSystemKeys.h"
#include "IILUtil/IILId.h"
#include "SEUtil/Docid.h"
#include "SEUtilServer/SeIdGenMgr.h"


AosDocidMgrNew::AosDocidMgrNew()
:
mLock(OmnNew OmnMutex()),
mDefaultDocids(0),
mNumDefaultDocids(0),
mInitDocid(AosDocid::getDftInitDocid()),
mNumVirtuals(0),
mDocidIncValue(eDftDocidIncValue),
mNeedVerify(true)
{
}


AosDocidMgrNew::~AosDocidMgrNew()
{
}


bool
AosDocidMgrNew::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	AosXmlTagPtr docconfig = config->getFirstChild("docclient");
	if (!docconfig)
	{
		OmnAlarm << "Missing Doc Client Config tag" << enderr;
		exit(-1);
	}
	
	mNumVirtuals = docconfig->getAttrInt(AOSTAG_NUM_VIRTUALS, -1);
	if (mNumVirtuals < 1)
	{
		OmnAlarm << "Num of Virtuals Incorrect: " << mNumVirtuals << enderr;
		OmnExitApp("DocidMgrNew configuration failed");
		return false;
	}

	AosXmlTagPtr defs = docconfig->getFirstChild("defs");
	if (!defs)
	{
		OmnAlarm << "Missing Doc Client Config tag" << enderr;
		exit(-1);
	}
	AosXmlTagPtr def = defs->getFirstChild();
	if (!def)
	{
		OmnAlarm << "Missing Doc Client Config tag" << enderr;
		exit(-1);
	}

	mInitDocid = def->getAttrU64("crtid", 0);
	aos_assert_r(mInitDocid, false);

	mDocidIncValue = def->getAttrInt("inc_value", eDftDocidIncValue);
	if (mDocidIncValue < 1)
	{
		OmnAlarm << "Invalid Docid Inc Value: " << mDocidIncValue << enderr;
		mDocidIncValue = eDftDocidIncValue;
	}

	mNeedVerify = def->getAttrBool("need_verify", true);
	return true;
}


bool
AosDocidMgrNew::stop()
{
    return true;
}


bool
AosDocidMgrNew::start()
{
	return true;
}


u64
AosDocidMgrNew::nextDocid(
		OmnString &objid, 
		const AosRundataPtr &rdata)
{
	// If 'objid' is empty, it will create an objid that maps to the
	// same virtual as the docid. Otherwise, it allocates a docid on the
	// same virtual to which 'objid' maps.
	aos_assert_rr(mNumVirtuals > 0, rdata, false);

	// 1. Determine the virtual
	mLock->lock();
	int virtual_id = -1;
	if (objid == "")
	{
		objid = createDefaultObjidLocked(rdata);
		aos_assert_rl(objid != "", mLock, false);
	}
		
	virtual_id = convertObjidToVirtualId(objid);
	u64 docid;
	while (1)
	{
		if (mNumDocids[virtual_id] <= 0)
		{
			bool rslt = retrieveDocidsLocked(virtual_id, rdata);
			aos_assert_rl(rslt, mLock, false);
		}

		docid = mCrtDocids[virtual_id]++;
		mNumDocids[virtual_id]--;
		if (!mNeedVerify) 
		{
			mLock->unlock();
			return docid;
		}

		bool rslt = AosDoesDocExist(docid, rdata);
		if (!rslt) 
		{
			mLock->unlock();
			return docid;
		}
	}

	mLock->unlock();
	OmnShouldNeverComeHere;
	return 0;
}


bool
AosDocidMgrNew::retrieveDocidsLocked(const int vid, const AosRundataPtr &rdata)
{
	u64 iilid = AOSIILID_SYSTEM_DOCIDS; 
	u64 docid = 0;
	OmnString key = AOSIILSYSKEY_DOCID;
	key << "_" << vid;
	bool rslt = AosIncrementKeyedValue(iilid, key, 
			docid, mDocidIncValue, mInitDocid, true, rdata);
	aos_assert_rr(rslt, rdata, false);
	aos_assert_rr(docid, rdata, false);

	mCrtDocids[vid] = docid;
	mNumDocids[vid] = mDocidIncValue;
	return true;
}


bool
AosDocidMgrNew::retrieveDftDocidsLocked(const AosRundataPtr &rdata)
{
	u64 iilid = AOSIILID_SYSTEM_DOCIDS; 
	OmnString key = AOSIILSYSKEY_DEFAULT_DOCIDS;
	u64 docid = 0;
	bool rslt = AosIncrementKeyedValue(iilid, key, docid, 
			mDocidIncValue, mInitDocid, true, rdata);
	aos_assert_rr(rslt, rdata, false);
	aos_assert_rr(docid, rdata, false);

	mDefaultDocids = docid;
	mNumDefaultDocids = mDocidIncValue;
	return true;
}


void
AosDocidMgrNew::returnDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
}


OmnString
AosDocidMgrNew::createDefaultObjidLocked(const AosRundataPtr &rdata)
{
	if (mNumDefaultDocids <= 0)
	{
		bool rslt = retrieveDftDocidsLocked(rdata);
		aos_assert_rr(rslt, rdata, "");
	}

	u64 dft_docid = mDefaultDocids++;
	mNumDefaultDocids--;
	return AosObjid::getDftObjid(dft_docid, rdata);
}

