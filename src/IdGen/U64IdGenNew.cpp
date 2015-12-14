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
// 09/16/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IdGen/U64IdGenNew.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "IdGen/IdGenDef.h"
#include "IdGen/IdGenMgr.h"
#include "Rundata/Rundata.h"
#include "StorageMgr/StorageMgr.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "SiteMgr/SyncServer.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


#if 0
AosU64IdGenNew::AosU64IdGenNew(const u64 &docid, const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mDocid(docid),
mCrtId(0),
mMaxId(0),
mNextId(0)
{
	bool rslt = start(rdata);
	if (!rslt)
	{
		OmnString errmsg = "Failed to create IdGen: ";
		errmsg << docid;
		OmnAlarm << errmsg << enderr;
		OmnExcept e(OmnFileLine, errmsg);
		throw e;
	}
}


AosU64IdGenNew::~AosU64IdGenNew()
{
}


bool
AosU64IdGenNew::start(const AosRundataPtr &rdata)
{
	// It retrieves the definition from the database and
	// initializes itself.
	mDoc = AosStorageMgr::getSelf()->getDoc(mDocid, rdata);
	aos_assert_rr(mDoc, rdata, false);

	mCrtId = mDoc->getAttrU64(AOSTAG_CRTID, 0);
	aos_assert_rr(mCrtId != 0, rdata, false);

	mMaxId = mDoc->getAttrU64(AOSTAG_MAXID, 0);
	aos_assert_rr(mMaxId != 0, rdata, false);

	mBlockSize = mDoc->getAttrU32(AOSTAG_BLOCKSIZE, 0);
	aos_assert_rr(mBlockSize != 0, rdata, false);

	mNextId = mCrtId;
	return true;
}


u64
AosU64IdGenNew::getNextId(const AosRundataPtr &rdata)
{
	aos_assert_rr(mCrtId <= mMaxId, rdata, false);
	mLock->lock();
	if (mCrtId > mNextId)
	{
		aos_assert_rl(mDoc, mLock, 0);
		mNextId += mBlockSize;
		mDoc->setAttr(AOSTAG_CRTID, mNextId);
		bool rslt = AosStorageMgr::getSelf()->saveDoc(mDocid, mDoc, rdata);
		aos_assert_rl(rslt, mLock, 0);
		aos_assert_rl(mCrtId <= mNextId, mLock, 0);
	}
	u64 id = mCrtId++;
	mLock->unlock();
	return id;
}

/*
bool
AosU64IdGenNew::setCrtid(const u64 &id, const AosRundataPtr &rdata)
{
	aos_assert_r(mDoc, false);
	mLock->lock();
	mCrtId = id;
	mNextId = id;
	mDoc->setAttr(AOSTAG_CRTID, mCrtId);
	mDoc->setAttr(AOSTAG_MAXID, mNextId);
	bool rslt = AosStorageMgr::getSelf()->saveDoc(mDocid, mDoc, rdata);
	mLock->unlock();
	return rslt;
}
*/

AosU64IdGenNewPtr
AosU64IdGenNew::createNewIdGen(
		const u64 &docid, 
		const u64 &crtid,
		const u64 &maxid, 
		const u32 &blocksize, 
		const AosRundataPtr &rdata)
{
	// This function creates a new id gen definition. For the record
	// format, please refer to the comments in 'readDef(...)'.
	// The caller should have locked the file and the caller should
	// have affirmed that the slot was not used by someone else.
	OmnString str = "<u64iddef />";
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, 0);

	doc->setAttr(AOSTAG_CRTID, crtid);
	doc->setAttr(AOSTAG_MAXID, maxid);
	doc->setAttr(AOSTAG_BLOCKSIZE, blocksize);
	doc->setAttr(AOSTAG_DOCID, docid);

	bool rslt = AosStorageMgr::getSelf()->saveDoc(docid, doc, rdata);
	aos_assert_rr(rslt, rdata, 0);

	try
	{
		return OmnNew AosU64IdGenNew(docid, rdata);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, AosErrmsgId::eExceptionCreateDoc);
		OmnAlarm << rdata->getErrmsg() << ". docid: " << docid << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
#endif
