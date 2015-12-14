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
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataStructs/StatIdExtIDGen.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataStructs/DataStructsUtil.h"
#include "DocClient/DocidMgr.h"
#include "DocTrans/AddStatBinaryDocidTrans.h"
#include "DocTrans/GetStatBinaryDocidTrans.h"
#include "IILUtil/IILId.h"

AosStatIdExtIDGen::AosStatIdExtIDGen(
		const AosXmlTagPtr &control_doc,
		const u32 nums)
:
mLock(OmnNew OmnMutex()),
mKey(""),
mCrtDocids(0),
mNumDocids(0),
mDocidNums(nums),
mNumVirtuals(0),
mExtStatIdMgr(0),
mDocsPerDistBlock(0)
{
	mKey = control_doc->getAttrStr(AOSTAG_OBJID, "");
	mKey << "_ctrl";
	mNumVirtuals = AosGetNumCubes();
	for (int i = 0; i < mNumVirtuals; i++)
	{
		mCrtDocids.push_back(0);
		mNumDocids.push_back(0);
		mGroupids.push_back(0);
		mSanityCheck.push_back(0);
	}

	mExtStatIdMgr = OmnNew AosExtStatIdMgr(control_doc);

	mDocsPerDistBlock = mExtStatIdMgr->getDocsPerDistBlock();
	if (mDocsPerDistBlock > 0 && mDocidNums > (u32)mDocsPerDistBlock)
	{
		mDocidNums = mDocsPerDistBlock;
	}
}


AosStatIdExtIDGen::~AosStatIdExtIDGen()
{
}


u64
AosStatIdExtIDGen::nextDocid(const int vid, const AosRundataPtr &rdata)
{
	aos_assert_r(vid >= 0, 0);
	mLock->lock();
	if (mNumDocids[vid] <= 0)
	{
		bool rslt = retrieveDocids(vid, rdata);
		if (!rslt)
		{
			mLock->unlock();
			return 0;
		}
	}
	u64 docid = mCrtDocids[vid];

u32 group_id = getGroupId(docid);
aos_assert_r(group_id == mGroupids[vid], 0);
aos_assert_rr(sanityCheck(vid, docid), rdata, false);

	mCrtDocids[vid] ++;
	mNumDocids[vid] --;

	mLock->unlock();
	aos_assert_r(mExtStatIdMgr, 0);
	mExtStatIdMgr->appendStatId(rdata, vid, docid);
//OmnScreen << "AosStatIdExtIDGen vid:" << vid << ";docid:" << docid << endl;
	return docid;
}


bool
AosStatIdExtIDGen::retrieveDocids(const int vid, const AosRundataPtr &rdata)
{
	aos_assert_r(mKey != "", false);
	aos_assert_r(mDocsPerDistBlock > 0, false);

	u64 initDocid = vid * mDocsPerDistBlock;
	if (initDocid == 0) initDocid = 1;

	OmnString key;
	key << mKey << "_" << vid;

	bool rslt;
	u64 docid = 0;
	if (mCrtDocids[vid] == 0)
	{
		bool found, is_unique;
		rslt = AosIILClient::getSelf()->getDocid(
				AOSIILID_STATID, key, docid, 0, found, is_unique, rdata);

		aos_assert_r(found, false); 
		if (docid != 0)
		{
			mCrtDocids[vid] = docid;
			mGroupids[vid] = getGroupId(docid); 
		}
		else
		{
			mCrtDocids[vid] = initDocid;
			mGroupids[vid] = getGroupId(initDocid);
		}
	}

	u64 incValue = 0;
	u64 next_group_id_startid = (mGroupids[vid] + 1) * mDocsPerDistBlock;
	u32 num_docids = mDocidNums;
	if (mCrtDocids[vid] == next_group_id_startid)
	{
		if (mDocidNums == (u32) mDocsPerDistBlock)
		{
			incValue = (mNumVirtuals -1) * mDocsPerDistBlock;
		}
	}
	else
	{
		if (mCrtDocids[vid] + num_docids >= next_group_id_startid)
		{
			aos_assert_r(mCrtDocids[vid] != 0, false);
			aos_assert_r(next_group_id_startid > mCrtDocids[vid], false);

			u32 delta = next_group_id_startid - mCrtDocids[vid];
			aos_assert_r(delta != 0, false);

			num_docids = delta;
			incValue = (mNumVirtuals -1) * mDocsPerDistBlock;
		}
	}
	incValue += num_docids;
	aos_assert_r(num_docids, false);

	//print(rdata, vid, key, incValue, initDocid);
	
	rslt = AosIILClient::getSelf()->incrementDocidToTable(
			AOSIILID_STATID, key, docid, incValue, initDocid, true, rdata);
	aos_assert_rr(rslt, rdata, false);
	aos_assert_rr(docid, rdata, false);
	aos_assert_rr(sanityCheck(vid, docid), rdata, false);
	mCrtDocids[vid] = docid;
	mNumDocids[vid] = num_docids;

	u32 groupid = getGroupId(docid);
	aos_assert_r(groupid == mGroupids[vid] || groupid == mGroupids[vid] + mNumVirtuals, false);
OmnScreen << "AosStatIdExtIDGen retrieveDocids vid:" << vid << ";num_docids:" << num_docids << ";docid:" << docid << ";groupid:" << groupid << ";"<< endl;

	mGroupids[vid] = groupid; 

	aos_assert_r(docid > mSanityCheck[vid], false);
	mSanityCheck[vid] = docid;
	return true;
}


bool
AosStatIdExtIDGen::sanityCheck(const int vid, const u64 &docid)
{
	u32 group_id = getGroupId(docid);
	//int vvid = group_id % mNumVirtuals;
	int vvid = AosDataStructsUtil::getVirtualIdByGroupid(group_id);
	aos_assert_r(vvid == vid, false);
	return true;
}

u64
AosStatIdExtIDGen::getGroupId(const u64 &docid)
{
	return docid / mDocsPerDistBlock;
}







//////////////////////////////  AosExtStatIdMgr ////////////////////////
//
AosStatIdExtIDGen::AosExtStatIdMgr::AosExtStatIdMgr(const AosXmlTagPtr &control_doc)
:
mDocsPerDistBlock(0),
mCtrlDocid(0),
mCrtIdx(-1)
{
	mDocsPerDistBlock = control_doc->getAttrU64("zky_doc_per_distblock", 0);
	mCtrlDocid = control_doc->getAttrU64(AOSTAG_DOCID, 0);

}

AosStatIdExtIDGen::AosExtStatIdMgr::~AosExtStatIdMgr()
{
}


u64 
AosStatIdExtIDGen::AosExtStatIdMgr::getDocid(const AosRundataPtr &rdata, const u64 &stat_id)
{
	AosTransPtr trans = OmnNew AosGetStatBinaryDocidTrans(mCtrlDocid, stat_id, false, false);
	AosBuffPtr resp;
	bool timeout;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_rr(rslt, rdata, false);

	u64 docid = resp->getU64(0);
	return docid;
}

bool
AosStatIdExtIDGen::AosExtStatIdMgr::addDocid(
		const AosRundataPtr &rdata, 
		const u64 &stat_id, 
		const u64 &binary_docid)
{
	AosTransPtr trans = OmnNew AosAddStatBinaryDocidTrans(mCtrlDocid, stat_id, binary_docid, false, false);
	bool rslt = AosSendTrans(rdata, trans);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosStatIdExtIDGen::AosExtStatIdMgr::appendStatId(
		const AosRundataPtr &rdata,
		const int vid,
		const u64 &stat_id)
{
	aos_assert_r(vid >=0, false);
	aos_assert_r(mDocsPerDistBlock, false);
	u32 idx = stat_id / mDocsPerDistBlock;
	if (mCrtIdx == -1 || idx != mCrtIdx)
	{
		u64 docid = getDocid(rdata, stat_id);
		if (docid == 0)
		{
			OmnString objid; 
			docid = AosDocidMgr::getSelf()->nextDocid(vid, objid, rdata);
			aos_assert_r(AosGetCubeId(docid) == vid, false);
			addDocid(rdata, stat_id, docid);
OmnScreen << "AosStatIdExtIDGen AosStatIdExtIDGen; groupid:" << idx << ";stat_id:" << stat_id << ";" << endl;
		}
		mCrtIdx = idx;
	}
	return true;
}


void
AosStatIdExtIDGen::print(
		const AosRundataPtr &rdata,
		const int vid,
		const OmnString &key,
		const u32 incValue,
		const u32 initDocid)
{
	u64 test_id = 0;
	bool found, is_unique;
	bool rslt = AosIILClient::getSelf()->getDocid(
			AOSIILID_STATID, key, test_id, 0, found, is_unique, rdata);
	aos_assert(rslt);

	if (test_id == 0) test_id = initDocid;

	u32 crt_groupid = getGroupId(test_id);
	u32 test_groupid = getGroupId(test_id + incValue);
	OmnScreen << "AosStatIdExtIDGen next test vid:" << vid 
		<< ";crt_iil_test_id:" << test_id 
		<< ";incValue:" << incValue 
		<< ";next_docid:" << test_id + incValue 
		<< ";crt_groupid:" << crt_groupid 
		<< ";next_groupid:" << test_groupid 
		<< ";mCrtDocids:" << mCrtDocids[vid]
		<< ";"<< endl;
	aos_assert(sanityCheck(vid, test_id + incValue));
}
#endif
