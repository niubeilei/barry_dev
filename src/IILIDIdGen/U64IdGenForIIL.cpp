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
// 11/07/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILIDIdGen/U64IdGenForIIL.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "IdGen/IdGenDef.h"
#include "IdGen/IdGenMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "SiteMgr/SyncServer.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"



AosU64IdGenForIIL::AosU64IdGenForIIL(
		const u32 virtual_id, 
		const u64 &docid, 
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mVirtualId(virtual_id),		// Ketty 2012/07/30
mDocid(docid),
mCrtId(0),
mNextId(0),
mStartId(0)
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


AosU64IdGenForIIL::~AosU64IdGenForIIL()
{
}


bool
AosU64IdGenForIIL::start(const AosRundataPtr &rdata)
{
	// It retrieves the definition from the database and
	// initializes itself.
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	
	mDoc = vfsMgr->getConfigDoc(mVirtualId, mDocid, rdata.getPtr());
	aos_assert_r(mDoc, false);
	mCrtId = mDoc->getAttrU64(AOSTAG_CRTID, 0);
	aos_assert_rr(mCrtId != 0, rdata, false);

	mBlockSize = mDoc->getAttrInt(AOSTAG_BLOCKSIZE, 0);
	aos_assert_rr(mBlockSize != 0, rdata, false);

	// Chen Ding, 07/11/2012
	mStartId = mCrtId;
	mNextId = mCrtId + mBlockSize;
	mCrtId = mNextId;

	return true;
}


u64
AosU64IdGenForIIL::getNextId(const AosRundataPtr &rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);
	
	OmnString ss;
	mLock->lock();
	while (1)
	{
		if (mCrtId >= mNextId)
		{
			// Need to load new IILIDs
			aos_assert_rl(mDoc, mLock, 0);
			aos_assert_rl(mBlockSize > 0, mLock, 0);
			mNextId = mCrtId + mBlockSize;
			mStartId = mCrtId;
			mDoc->setAttr(AOSTAG_CRTID, mNextId);
			bool rslt = vfsMgr->saveConfigDoc(mVirtualId, mDocid, mDoc, rdata.getPtr());
			aos_assert_rl(rslt, mLock, 0);
			aos_assert_rl(mCrtId <= mNextId, mLock, 0);
		}
		u64 id = mCrtId++;
		mLock->unlock();
		return id;
	}

	mLock->unlock();
	OmnShouldNeverComeHere;
	return 0;
}


AosU64IdGenForIILPtr
AosU64IdGenForIIL::createNewIdGen(
		const u32 virtual_id,	// Ketty 2012/07/30
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
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	AosXmlTagPtr doc  = vfsMgr->getConfigDoc(virtual_id, docid, rdata.getPtr());
	//if (doc)
	//{
	//	OmnScreen << "IIL IdGen has exsiting . check your config!" << endl;
	//	exit(0);
	//}
	if (!doc)
	{
		OmnString str = "<u64iddef />";
		AosXmlParser parser;
		doc = parser.parse(str, "" AosMemoryCheckerArgs);
	}
	aos_assert_rr(doc, rdata, 0);

	if (doc->getAttrU64(AOSTAG_CRTID, 0) < crtid)
	{	
		doc->setAttr(AOSTAG_CRTID, crtid);
	}
	doc->setAttr(AOSTAG_MAXID, maxid);
	doc->setAttr(AOSTAG_BLOCKSIZE, blocksize);
	doc->setAttr(AOSTAG_DOCID, docid);

	bool rslt = vfsMgr->saveConfigDoc(virtual_id, docid, doc, rdata.getPtr());
	aos_assert_rr(rslt, rdata, 0);

	try
	{
		return OmnNew AosU64IdGenForIIL(virtual_id, docid, rdata);
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

