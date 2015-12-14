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
// 09/02/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Snapshot/Testers/DfmNormalDocTester.h"

#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocClientObj.h"
#include "Snapshot/Testers/NormalDoc.h"

AosDfmNormalDocTester::AosDfmNormalDocTester()
{
}

AosDfmNormalDocTester::AosDfmNormalDocTester(
		const AosRundataPtr &rdata)
:
AosDfmTester("normal"),
mLogicid(0),
mTaskDocid(0),
mRdata(rdata),
mTotalNumDocs(0),
mNumDocs(0),
mBatchAddNum(0),
mValue(0)
{
	mTaskDocid = rand() % 39999 +1;
	mVirtualIds = AosGetTotalCubeIds();
	mNormalObj = OmnNew AosNormalDoc(mTaskDocid, rdata);
}


AosDfmNormalDocTester::AosDfmNormalDocTester(
		const u32 logicid,
		const AosRundataPtr &rdata)
:
AosDfmTester("normal"),
mLogicid(logicid),
mTaskDocid(0),
mRdata(rdata),
mTotalNumDocs(0),
mNumDocs(0),
mBatchAddNum(0),
mValue(0)
{
	mTaskDocid = rand() % 39999 +1;
	mVirtualIds = AosGetTotalCubeIds();
	mNormalObj = OmnNew AosNormalDoc(mTaskDocid, rdata);
	mValue = logicid * 100000;
	if (logicid == 0)
	{
		mValue = 5500;
	}
}

AosDfmNormalDocTester::~AosDfmNormalDocTester()
{
}



bool
AosDfmNormalDocTester::createSnapshot()
{
	mSnapIds.clear();
	mNormalObj->resetDocid();
	mNumDocs = 0;
	mBatchAddNum ++;
	Status st;        
	st.start = 0;
	st.end = 0;
	st.sts = eInvalid;
	mSts.insert(make_pair(mBatchAddNum, st));

	OmnString ss;
	u64 snap_id = 0;
	for (u32 i = 0; i < mVirtualIds.size(); i++)
	{
		snap_id = AosDocClientObj::getDocClient()->createSnapshot(mRdata, mVirtualIds[i], AosDocType::eNormalDoc, mTaskDocid);
		mSnapIds.insert(make_pair(mVirtualIds[i], snap_id));
		ss << ";virtual_id:" << mVirtualIds[i] << ";snap_id:" << snap_id << ";";
	}
	mNormalObj->setSnapIds(mSnapIds);
	OmnScreen << "BBBBBBBBBBBBBBBB logic:" << mLogicid << ";create snap:" << ss << endl;
}


bool
AosDfmNormalDocTester::rollbackSnapshot()
{
	aos_assert_r(mSts.count(mBatchAddNum) != 0, false);
	mSts[mBatchAddNum].start = mNormalObj->getStartDocid();
	mSts[mBatchAddNum].end = mNormalObj->getEndDocid();
	mSts[mBatchAddNum].sts = eRollBack;
	u64 snap_id = 0;
	map<int, u64>::iterator itr = mSnapIds.begin();
	OmnString ss;
	for (; itr != mSnapIds.end(); ++itr)
	{
		snap_id = itr->second;
		AosDocClientObj::getDocClient()->rollbackSnapshot(mRdata, itr->first, AosDocType::eNormalDoc, snap_id, mTaskDocid); 
		ss << ";virtual_id:" << itr->first << ";snap_id:" << snap_id << ";";
	}
	OmnScreen << "BBBBBBBBBBBBBBBB logic:" << mLogicid << ";rollback snap:" << ss
		<< "docid[" << mNormalObj->getStartDocid() << "," << mNormalObj->getEndDocid() << "]"<< endl;
}


bool
AosDfmNormalDocTester::commitSnapShot()
{
	aos_assert_r(mSts.count(mBatchAddNum) != 0, false);
	mSts[mBatchAddNum].start = mNormalObj->getStartDocid();
	mSts[mBatchAddNum].end = mNormalObj->getEndDocid();
	mSts[mBatchAddNum].sts = eCommit;

	u64 snap_id = 0;
	map<int, u64>::iterator itr = mSnapIds.begin();
	OmnString ss;
	for (; itr != mSnapIds.end(); ++itr)
	{
		snap_id = itr->second;
		AosDocClientObj::getDocClient()->commitSnapshot(mRdata, itr->first, AosDocType::eNormalDoc, snap_id, mTaskDocid); 
		ss << ";virtual_id:" << itr->first << ";snap_id:" << snap_id << ";";
	}
	mTotalNumDocs += mNumDocs; 
	OmnScreen << "BBBBBBBBBBBBBBBB logic:" << mLogicid << ";commit snap:" << ss 
		<< "docid[" << mNormalObj->getStartDocid() << "," << mNormalObj->getEndDocid() << "]"<< endl;
}


bool
AosDfmNormalDocTester::createDocs()
{
	vector<u64> values;
	int r = rand() %100 +1;
	for (int i = 0; i < r; i++)
	{
		mNormalObj->createData(++mValue, mRdata);
		mNumDocs ++;
	}
	return true;
}


bool
AosDfmNormalDocTester::basicTest()
{
	createSnapshot();
	int r = rand() % 100 +1;
	OmnScreen << "tries: " << r << endl;
	for (int i = 0; i < r; i++)
	{
		createDocs();
	}

	r = rand() %4;
	if (r)
	{
		commitSnapShot();
	}
	else
	{
		rollbackSnapshot();
	}
	OmnSleep(10);
	verify();
}


bool
AosDfmNormalDocTester::verify()
{
	//map<int, Status>::iterator itr = mSts.begin();
	//for (; itr != mSts.end(); ++itr)
	map<int, Status>::iterator itr = mSts.find(mBatchAddNum);
	if (itr->second.sts == eCommit)
	{
		mNormalObj->checkData(mRdata, itr->second.start, itr->second.end);
	}
	else
	{
		for (u64 i = itr->second.start; i<= itr->second.end; i++)
		{
			AosXmlTagPtr dd = AosDocClientObj::getDocClient()->getDocByDocid(i, mRdata);
			aos_assert_r(!dd, false);
		}
	}
	return true;
}



bool
AosDfmNormalDocTester::serializeTo(const AosBuffPtr &buff)
{
	buff->setU32(mLogicid);
	buff->setU64(mTotalNumDocs);
	buff->setInt(mBatchAddNum);
	buff->setU64(mValue);

	map<int, Status>::iterator itr = mSts.begin();
	for (; itr != mSts.end(); ++itr)
	{
		buff->setU64(itr->first);
		buff->setU64(itr->second.start);
		buff->setU64(itr->second.end);
		buff->setInt(itr->second.sts);
	}
	return true;
}


bool
AosDfmNormalDocTester::serializeFrom(const AosBuffPtr &buff)
{
	mLogicid = buff->getU32(0);
	mTotalNumDocs = buff->getU64(0);
	mBatchAddNum = buff->getInt(0);
	mValue = buff->getU64(0);
	for (u64  i = 1; i<= mBatchAddNum; i++)
	{
		u64 kk = buff->getU64(0); 
		aos_assert_r(kk == i, false);
		Status st;        
		st.start = buff->getU64(0);
		st.end = buff->getU64(0);
		st.sts = (AosDfmNormalDocTester::E)buff->getInt(0);
		mSts.insert(make_pair(kk, st));
	}

	return true;
}
