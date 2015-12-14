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
#include "Snapshot/Testers/DfmIILStrTester.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/BitmapObj.h"

AosDfmIILStrTester::AosDfmIILStrTester()
{
}


AosDfmIILStrTester::AosDfmIILStrTester(const AosRundataPtr &rdata)
:
AosDfmTester("str"),
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
	mLen = eDftStrLen + sizeof(u64) + eZeroLen;
}



AosDfmIILStrTester::AosDfmIILStrTester(
		const u32 logicid,
		const int num,
		const AosRundataPtr &rdata)
:
AosDfmTester("str"),
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
	mIILName << "_zt44" << mLogicid << "a" << num;
	u32 hash_key = AosGetHashKeyByIILName(mIILName);
	mVirtualId = AosGetCubeId(hash_key);
OmnScreen << "New New New New "<< mIILName << endl;
	mLen = eDftStrLen + sizeof(u64) + eZeroLen;
}

AosDfmIILStrTester::~AosDfmIILStrTester()
{
}



bool
AosDfmIILStrTester::createSnapshot()
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
AosDfmIILStrTester::rollbackSnapshot()
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
AosDfmIILStrTester::commitSnapShot()
{
	aos_assert_r(mSnapId, false);
	aos_assert_r(mSts.count(mBatchAddNum) != 0, false);
	mSts[mBatchAddNum].end = mValue -1;
	mSts[mBatchAddNum].sts = eCommit;
	AosIILClientObj::getIILClient()->commitSnapshot(mVirtualId, mSnapId, mTaskDocid, mRdata);
	mTotalNumDocs += mNumDocs; 
//OmnScreen << "================:" << mSnapId << ";" << mTotalNumDocs << endl;
	OmnScreen << "BBBBBBBBBBBBBBBB logic:" << mLogicid << ";iilname:" << mIILName << ";commitSnap virtual_id:" << mVirtualId << ";snapid:" << mSnapId << endl;
}


bool
AosDfmIILStrTester::StrBatchAdd()
{
	int r = rand() %100 +1;
	AosBuffPtr buff = OmnNew AosBuff(r * mLen AosMemoryCheckerArgs);
	char *record = buff->data();
	int64_t data_len = 0;
	for (int i = 0; i < r; i++)
	{
		int pos = data_len;
		OmnString ss;
		ss << mValue;
		aos_assert_r(ss.length() <= eDftStrLen, false);
	  	memcpy(&record[pos], ss.data(), ss.length());
		pos += ss.length();
		aos_assert_r(ss.length() < eDftStrLen, false);
		record[pos] = 0;
		aos_assert_r(pos < (data_len + eDftStrLen), false);

		pos = data_len + eDftStrLen + eZeroLen;
		*(u64*)&record[pos] = mValue;

		char *entries = &record[data_len];
		OmnString value(entries, strlen(entries));
		aos_assert_r(value == ss, false);
		u64 docid = *(u64 *)&entries[mLen - sizeof(u64)];
		aos_assert_r(docid == mValue, false);

		data_len += mLen;

		mValue ++;
		mNumDocs ++;
	}
	buff->setDataLen(data_len);

	u64 executor_id = 0;
	bool rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
			mIILName, mLen, buff, executor_id, mSnapId, mTaskDocid,
			mRdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDfmIILStrTester::basicTest()
{
	if (mTotalNumDocs >= 1000000) 
	{
OmnScreen << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
		return true;
	}
	createSnapshot();
	int r = rand() % 400 +1;
	OmnScreen << "tries: " << r << endl;
	for (int i = 0; i < r; i++)
	{
		//OmnScreen << "tries: " << i << endl;
		StrBatchAdd();
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
AosDfmIILStrTester::runQuery()
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
		queryRslt->setWithValues(true);
		AosIILClientObj::getIILClient()->querySafe3(mIILName, queryRslt, 0, query_context, mRdata);
		aos_assert_r(queryRslt, false);
		if (queryRslt->getNumDocs() == 0) break;

		u64 docid;
		OmnString key;                                                
		bool finished = false;
		while(queryRslt->nextDocidValue(docid, key, finished, mRdata))
		{
			if (finished) break;
//OmnScreen << "key:" << key << ";docid:" << docid << ";" << endl;
			aos_assert_r(atoll(key.data()) == docid, false);
			aos_assert_r(mVerify.count(docid) == 0, false);
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
AosDfmIILStrTester::verify()
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
AosDfmIILStrTester::serializeTo(const AosBuffPtr &buff)
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
AosDfmIILStrTester::serializeFrom(const AosBuffPtr &buff)
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
		st.sts = (AosDfmIILStrTester::E)buff->getInt(0);
		mSts.insert(make_pair(kk, st));
	}

	u32 hash_key = AosGetHashKeyByIILName(mIILName);
	mVirtualId = AosGetCubeId(hash_key);
	return true;
}
