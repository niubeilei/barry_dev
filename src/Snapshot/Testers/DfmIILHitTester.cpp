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
#include "Snapshot/Testers/DfmIILHitTester.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/BitmapObj.h"

AosDfmIILHitTester::AosDfmIILHitTester()
{
}


AosDfmIILHitTester::AosDfmIILHitTester(const AosRundataPtr &rdata)
:
AosDfmTester("hit"),
mLogicid(0),
mIILName(""),
mTaskDocid(0),
mSnapId(0),
mVirtualId(-1),
mRdata(rdata),
mValue(1),
mBatchAddNum(0),
mTotalNumDocs(0),
mNumDocs(0)
{
	mTaskDocid = rand() % 39999 +1;
}



AosDfmIILHitTester::AosDfmIILHitTester(
		const u32 logicid,
		const int num,
		const AosRundataPtr &rdata)
:
AosDfmTester("hit"),
mLogicid(logicid),
mIILName(""),
mTaskDocid(0),
mSnapId(0),
mVirtualId(-1),
mRdata(rdata),
mValue(1),
mBatchAddNum(0),
mTotalNumDocs(0),
mNumDocs(0)
{

	mTaskDocid = rand() % 39999 +1;
	//for (u32 i=0; i<length; i++)
	//{
	//	mIILName << (char)(random()%26 + 'a');
	//}
	mIILName << "_zt22" << mLogicid << "a" << num;
	u32 hash_key = AosGetHashKeyByIILName(mIILName);
	mVirtualId = AosGetCubeId(hash_key);
OmnScreen << "New New New New "<< mIILName << endl;
}

AosDfmIILHitTester::~AosDfmIILHitTester()
{
}



bool
AosDfmIILHitTester::createSnapshot()
{
	mNumDocs = 0;
	mBatchAddNum ++;
	Status st;        
	st.start = mValue;
	st.end = 0;
	st.sts = eInvalid;
	mSts.insert(make_pair(mBatchAddNum, st));
//OmnScreen << "mBatchAddNum " << mBatchAddNum << ";start:" << mSts[mBatchAddNum].start << endl;

	mSnapId = AosIILClientObj::getIILClient()->createSnapshot(mVirtualId, mTaskDocid, mRdata);
	OmnScreen << "BBBBBBBBBBBBBBBB logic:" << mLogicid << ";iilname:" << mIILName << ";createSnap virtual_id:" << mVirtualId << ";snapid:"<< mSnapId << endl;
}


bool
AosDfmIILHitTester::rollbackSnapshot()
{
	aos_assert_r(mSnapId, false);
	aos_assert_r(mSts.count(mBatchAddNum) != 0, false);
	mSts[mBatchAddNum].end = mValue -1;
	mSts[mBatchAddNum].sts = eRollBack;
//OmnScreen << "mBatchAddNum " << mBatchAddNum << ";end:" << mSts[mBatchAddNum].end << endl;
	AosIILClientObj::getIILClient()->rollBackSnapshot(mVirtualId, mSnapId, mTaskDocid, mRdata);
	OmnScreen << "BBBBBBBBBBBBBBBB logic:" << mLogicid  << ";iilname:" << mIILName << ";rollbackSnap virtual_id:" << mVirtualId << ";snapid:" << mSnapId << endl;
}


bool
AosDfmIILHitTester::commitSnapShot()
{
	aos_assert_r(mSnapId, false);
	aos_assert_r(mSts.count(mBatchAddNum) != 0, false);
	mSts[mBatchAddNum].end = mValue -1;
	mSts[mBatchAddNum].sts = eCommit;
	AosIILClientObj::getIILClient()->commitSnapshot(mVirtualId, mSnapId, mTaskDocid, mRdata);
//OmnScreen << "mBatchAddNum " << mBatchAddNum << ";end:" << mSts[mBatchAddNum].end << endl;
//OmnScreen << "================:" << mSnapId << ";" << mTotalNumDocs << endl;
	//mTotalNumDocs += (mValue - mSts[mBatchAddNum].start);
u64 ii = mValue - mSts[mBatchAddNum].start;
	aos_assert_r(mNumDocs == ii,  false);
	mTotalNumDocs += mNumDocs; 
//OmnScreen << "================:" << mSnapId << ";" << mTotalNumDocs << endl;
	OmnScreen << "BBBBBBBBBBBBBBBB logic:" << mLogicid << ";iilname:" << mIILName << ";commitSnap virtual_id:" << mVirtualId << ";snapid:" << mSnapId << endl;
}


bool
AosDfmIILHitTester::hitBatchAdd()
{
	vector<u64> values;
	int r = rand() %100 +1;
	for (int i = 0; i < r; i++)
	{
		values.push_back(mValue);
		mValue ++;
		aos_assert_r(mValue, false);
	}
	mNumDocs += values.size();

	bool rslt = AosIILClientObj::getIILClient()->HitBatchAdd(mIILName, values, mSnapId, mTaskDocid, mRdata);
	aos_assert_r(rslt, false);
}


bool
AosDfmIILHitTester::basicTest()
{
	if (mTotalNumDocs >= 1000000) 
	{
OmnScreen << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
		return true;
	}
	/*Status st;
	st.start = mValue;
	hitBatchAdd();
	st.end = mValues - 1;
	st.sts = eNormal;
	mSts.insert(make_pair(mBatchAddNum, sts));
	*/

	createSnapshot();
	int r = rand() % 400 +1;
	OmnScreen << "tries: " << r << endl;
	for (int i = 0; i < r; i++)
	{
		//OmnScreen << "tries: " << i << endl;
		hitBatchAdd();
	}

	r = rand() %2;
	if (r)
	{
		commitSnapShot();
	}
	else
	{
		rollbackSnapshot();
	}
	runQuery();
	verify();
}


bool
AosDfmIILHitTester::runQuery()
{
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_an);                                               
	query_context->setStrValue("*");                                               
	//query_context->setBlockSize(50000);
	query_context->setBlockSize(1000000);

	u64 num_docs = 0;
	u64 num_docs1 = 0;
	while(!query_context->finished())
	{
		AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
		AosIILClientObj::getIILClient()->querySafe3(mIILName, queryRslt, 0, query_context, mRdata);
		aos_assert_r(queryRslt, false);
		if (queryRslt->getNumDocs() == 0) break;

		u64 docid;
		OmnString key;                                                
		bool finished = false;
		while(docid = queryRslt->nextDocid(finished))
		{
			if (finished) break;
//OmnScreen << "docid:" << docid << endl;
			//aos_assert_r(mVerify.count(docid) == 0, false);
			if(mVerify.count(docid) != 0)
			{
				OmnScreen << "kkkk:" << docid << ";vv:" << mValue << endl;
			}
			mVerify.insert(make_pair(docid, 0));
			num_docs ++;
		}
		num_docs1 += queryRslt->getNumDocs();
//OmnScreen << "iilname:" << mIILName << ";num_docs :" << num_docs << " , mTotalNumDocs: " << mTotalNumDocs << " , numdos:" << num_docs1 << endl;
	}
	aos_assert_r(num_docs == mTotalNumDocs, false);
	//if(num_docs != mTotalNumDocs)
	//{
	//	OmnMark;
	//	runQuery();
	//}
	return true;
}


bool
AosDfmIILHitTester::verify()
{
	map<int, Status>::iterator itr = mSts.begin();
	for (; itr != mSts.end(); ++itr)
	{
		for (u64 i = itr->second.start; i<= itr->second.end; i++)
		{
			if (mVerify.count(i) != 0)
			{
				mVerify[i] ++;
			}
			else
			{
				aos_assert_r(itr->second.sts == eRollBack, false); 
			}
		}
	}
	map<u64, int>::iterator itr1 = mVerify.begin();
	for (; itr1 != mVerify.end(); ++itr1)
	{
		if (itr1->second != 1)
		{
			OmnScreen << "iilname:" << mIILName << ";docid(" << itr1->first << "," << itr1->second << ")"  << endl;
			aos_assert_r(itr1->second == 1, false);
		}
	}
	mVerify.clear();
	return true;
}

bool
AosDfmIILHitTester::serializeTo(const AosBuffPtr &buff)
{
	buff->setU32(mLogicid);
	buff->setOmnStr(mIILName);
	buff->setU64(mValue);
	buff->setU64(mTotalNumDocs);
	buff->setInt(mBatchAddNum);

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
AosDfmIILHitTester::serializeFrom(const AosBuffPtr &buff)
{
	mLogicid = buff->getU32(0);
	mIILName = buff->getOmnStr("");
	aos_assert_r(mIILName != "", false);
	mValue = buff->getU64(0);
	mTotalNumDocs = buff->getU64(0);
	mBatchAddNum = buff->getInt(0);
	for (u64  i = 1; i<= mBatchAddNum; i++)
	{
		u64 kk = buff->getU64(0); 
		aos_assert_r(kk == i, false);
		Status st;        
		st.start = buff->getU64(0);
		st.end = buff->getU64(0);
		st.sts = (AosDfmIILHitTester::E)buff->getInt(0);
		mSts.insert(make_pair(kk, st));
	}

	u32 hash_key = AosGetHashKeyByIILName(mIILName);
	mVirtualId = AosGetCubeId(hash_key);
	return true;
}
