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
// 03/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/SizeIdIDGen.h"

#include "API/AosApi.h"
#include "IILUtil/IILId.h"
#include "IILUtil/IILSystemKeys.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


static u64 sgInitSizeid = 0;
static u64 sgIncSizeid = 0;
static u64 sgInitDocids = 0;
static u64 sgIncDocids = 0;
static bool sgInited = false;
static bool sgShowLog = false;

AosSizeIdIDGen::AosSizeIdIDGen(const u32 siteid)
:
mLock(OmnNew OmnMutex()),
mSiteid(siteid),
mCrtSizeid(0),
mSizeidNum(0)
{
}


AosSizeIdIDGen::~AosSizeIdIDGen()
{
}


bool 
AosSizeIdIDGen::config(const AosXmlTagPtr &sizeid)
{
	aos_assert_r(sizeid, false);

	sgInitSizeid = sizeid->getAttrU64("initSizeid", eInitSizeId);
	sgIncSizeid = sizeid->getAttrU64("incSizeid", eIncSizeId);
	if(sgIncSizeid == 0) sgIncSizeid = eIncSizeId;
	
	u32 virtualNum = AosGetNumCubes();
	sgInitDocids = sizeid->getAttrU64("initDocids", eInitDocids);
	if(sgInitDocids <= virtualNum)
	{
		sgInitDocids += virtualNum;
	}

	sgIncDocids = sizeid->getAttrU64("incDocids", eIncDocids);
	if(sgIncDocids == 0) sgIncDocids = eIncDocids;
	
	sgShowLog = sizeid->getAttrBool(AOSCONFIG_SHOWLOG, false);

	sgInited = true;
	return true;
}


u64
AosSizeIdIDGen::createSizeId(
		const u64 &record_docid,
		const AosRundataPtr &rdata)
{
	// SizeIds are managed through the system IIL 'AOSIILID_SIZEIDS'. 
	// It is a u64 IIL. This function does the following:
	// 1. Get a SizeID
	// 2. Add an entry [SizeId, docid] to AOSIILID_SIZEIDS
	aos_assert_r(sgInited, 0);
	aos_assert_r(record_docid > 0, 0);
	
	mLock->lock();
	u64 sizeid = getNextSizeId(record_docid, rdata);
	mLock->unlock();

	return sizeid;
}


u64
AosSizeIdIDGen::getNextDocid(
		const u64 &record_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sgInited, 0);
	aos_assert_r(record_docid > 0, 0);

	bool rslt;
	bool overflow;
	u64 sizeid;
	u64 docid = 0;
	u64 crt_docid = 0;
	int num_docids = 0;

	mLock->lock();
	map<u64, DocidAllocator>::iterator itr = mDocidAllocator.find(record_docid);
	if (itr == mDocidAllocator.end())
	{
		sizeid = getCrtSizeIdPriv(record_docid, rdata);
		aos_assert_r(sizeid, false);

		rslt = getDocidsPriv(sizeid, crt_docid, num_docids, overflow, rdata);
		aos_assert_r(rslt, false);

		if (overflow)
		{
			sizeid = getNextSizeId(record_docid, rdata);
			aos_assert_r(sizeid, false);

			crt_docid = 0;
			num_docids = 0;
			rslt = getDocidsPriv(sizeid, crt_docid, num_docids, overflow, rdata);
			aos_assert_r(rslt, false);
			aos_assert_r(!overflow, false);
		}

		docid = crt_docid++;
		num_docids--;

		DocidAllocator dal;
		dal.mSizeid = sizeid;
		dal.mRecordDocid = record_docid;
		dal.mCrtDocid = crt_docid;
		dal.mNumDocids = num_docids;

		mDocidAllocator[record_docid] = dal;
	}
	else
	{
		if (itr->second.mNumDocids <= 0)
		{
			sizeid = itr->second.mSizeid;
			rslt = getDocidsPriv(sizeid, crt_docid, num_docids, overflow, rdata);
			aos_assert_r(rslt, false);

			if (overflow)
			{
				sizeid = getNextSizeId(record_docid, rdata);
				aos_assert_r(sizeid, false);

				crt_docid = 0;
				num_docids = 0;
				rslt = getDocidsPriv(sizeid, crt_docid, num_docids, overflow, rdata);
				aos_assert_r(rslt, false);
				aos_assert_r(!overflow, false);
			}

			docid = crt_docid++;
			num_docids--;

			itr->second.mSizeid = sizeid;
			itr->second.mCrtDocid = crt_docid;
			itr->second.mNumDocids = num_docids;
		}

		docid = itr->second.mCrtDocid++;
		itr->second.mNumDocids--;
	}

	mLock->unlock();
	return docid;
}


u64 
AosSizeIdIDGen::getCrtSizeId(
		const u64 &record_docid,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	u64 sizeid = getCrtSizeIdPriv(record_docid, rdata);
	mLock->unlock();
	return sizeid;
}


u64 
AosSizeIdIDGen::getCrtSizeIdPriv(
		const u64 &record_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sgInited, 0);
	aos_assert_r(record_docid > 0, 0);

	u64 sizeid = 0;
	bool found = false;
	bool is_unique = true;
	bool rslt = AosGetDocid(AOSIILID_SIZEIDS_RECORD,
		record_docid, sizeid, 0, found, is_unique, rdata);	
	aos_assert_r(rslt, 0);

	if (sizeid == 0)
	{
		sizeid = getNextSizeId(record_docid, rdata);
	}
	return sizeid;
}


u64 
AosSizeIdIDGen::getNextSizeId(
		const u64 &record_docid,
		const AosRundataPtr &rdata)
{
	return 1;
	// Sizeids are managed in AOSIILID_SYSTEM_DOCIDS through the 
	// key AOSIILSYSKEY_SIZEIDS
	u64 sizeid = 0;
	if(mSizeidNum <= 0)
	{
		sizeid = getNewSizeIds(rdata);
	}
	else
	{
		mSizeidNum--;
		mCrtSizeid++;
		sizeid = mCrtSizeid;
	}
	aos_assert_r(sizeid, 0);
	
	//OmnScreen << "---start create new sizeid:" << sizeid << "---" << endl;
	bool rslt = AosAddU64ValueDocToTable(
		AOSIILID_SIZEIDS, sizeid, record_docid, true, true, rdata);
	aos_assert_r(rslt, 0);

	u64 iilid = AOSIILID_SIZEIDS_RECORD;
	rslt = AosSetU64ValueDocUniqueToTable(
		iilid, true, record_docid, sizeid, false, rdata);
	//OmnScreen << "---end create new sizeid:" << sizeid << "---" << endl;
	
	aos_assert_r(rslt, 0);
	
	return sizeid;
}


u64 
AosSizeIdIDGen::getNewSizeIds(const AosRundataPtr &rdata)
{
	u64 sizeid = 0;
	
	//OmnScreen << "---start get new sizeids---" << endl;
	bool rslt = AosIncrementKeyedValue(
		AOSIILID_SYSTEM_DOCIDS, AOSIILSYSKEY_SIZEIDS,
		sizeid, sgIncSizeid, sgInitSizeid, true, rdata);
	//OmnScreen << "---end get new sizeids:" << sizeid << "," << sgIncSizeid << "---" << endl;
	
	aos_assert_r(rslt, 0);

	mCrtSizeid = sizeid;
	mSizeidNum = sgIncSizeid - 1;
	return sizeid;
}


bool
AosSizeIdIDGen::getDocids(
		const u64 &sizeid,
		u64 &start_docid,
		int &num_docids,
		bool &overflow,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	bool rslt = getDocidsPriv(sizeid, start_docid, num_docids, overflow, rdata);
	mLock->unlock();
	return rslt;
}

bool
AosSizeIdIDGen::getDocidsPriv(
		const u64 &sizeid,
		u64 &start_docid,
		int &num_docids,
		bool &overflow,
		const AosRundataPtr &rdata)
{
	// This function retrieves 'num_docids'. The system uses the IIL
	// 'AOSIILID_SIZE_DOCIDS' to manage the docids:
	// 			[sizeid, current_docids]
	aos_assert_r(sgInited, false);
	aos_assert_r(sizeid > 0, false);
	
	u32 siteid = rdata->getSiteid();
	aos_assert_r(mSiteid == siteid && siteid > 0, 0);

	overflow = false;
	num_docids = sgIncDocids;
	start_docid = 0;

	u64 docids = 0;
	
	//OmnScreen << "---start get docids from sizeidmgr, num:" << num_docids << endl;
	bool rslt = AosIncrementDocid(
		AOSIILID_SIZE_DOCIDS, sizeid, docids, false,
		num_docids, sgInitDocids, true, 0, rdata);
	//OmnScreen << "---end get docids from sizeidmgr, start_docid:" << start_docid << ", num:" << num_docids << endl;
	
	aos_assert_r(rslt, false);
	aos_assert_r(docids != 0, false);
	
	// Check whether the docid has overflown.
	if (docids > eMaxDocid)
	{
		overflow = true;
		num_docids = 0;
		return true;
	}

	if ((docids + num_docids) > eMaxDocid)
	{
		num_docids = eMaxDocid - docids;
	}

	//start_docid = (((u64)AosDocType::eGroupedDoc) << eDocTypeOffsetSize) +
	//	((sizeid & eSizeIdMask) << eSizeidOffsetSize) + docids;

	start_docid = (((u64)AosDocType::eGroupedDoc) << eDocTypeOffsetSize) + docids;

	if (sgShowLog)
	{
		OmnScreen << "get docids from sizeidmgr, start_docid:" << start_docid << ", num:" << num_docids << endl;
	}

	return true;
}

