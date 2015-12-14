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
// 08/09/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Snapshot/TestersClt/IILProcTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEUtil/IILIdx.h"
#include "Random/RandomBuffArray.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Tester/TestMgr.h"

AosIILProcTester::AosIILProcTester(
		const AosRundataPtr &rdata,
		const OmnString &iilname,
		const int idx)
:
mRundata(rdata),
mIILName(iilname),
mNumDocs(0),
mIdx(idx)
{
	createIIL();
}


AosIILProcTester::~AosIILProcTester()
{
}


bool
AosIILProcTester::basicTest()
{
	addEntries();
	return true;
}

bool
AosIILProcTester::createIIL()
{
	// 1. random illname
	u32 length = random()%5 + 5;
	AosIILClientObj::getIILClient()->createIILPublic(mIILName, mIILID, eAosIILType_Str, mRundata);
	aos_assert_r(mIILID > 0, false);
	return true;
}


bool 
AosIILProcTester::addEntries()
{
	// 2. append entry to ill
	u32 num = rand() % eNumAddIILEntries;
	for (u32 j=0; j<num; j++)
	{
		bool rslt = addEntry();
		aos_assert_r(rslt, false);

	}
	return true;
}


OmnString
AosIILProcTester::getRandomStr(u32 length)
{
	OmnString str = "";
	for (u32 i=0; i<length; i++)
	{
		str << (char)(random()%26 + 'a');	
	}

	return str;
}


bool
AosIILProcTester::addEntry()
{
	OmnString str = getRandomStr(5);
	u64 docid = random()%100000000;
	AosIILClientObj::getIILClient()->addStrValueDoc(mIILName, str, docid, false, false, mRundata);
	addKey(str, docid);
	mNumDocs ++;
	return true;
}


bool
AosIILProcTester::createSnapShot()
{
	mSts = AosSnapShot::eAddSnap;
	AosIILClientObj::getIILClient()->createSnapShot(mIILName, eAosIILType_Str, mRundata);
}


bool
AosIILProcTester::commitSnapShot()
{
	mSts = AosSnapShot::eCommit;
	AosIILClientObj::getIILClient()->commitSnapShot(mIILName, mRundata);
}


bool
AosIILProcTester::rollBackSnapShot()
{
	mSts = AosSnapShot::eRollback;
	AosIILClientObj::getIILClient()->rollBackSnapShot(mIILName, mRundata);
	OmnScreen << "roll Back SnapShot, numDocs: " << mNumDocs << endl;
}


bool
AosIILProcTester::isCommit()
{
	return mSts == AosSnapShot::eCommit?true:false;
}


bool
AosIILProcTester::checkValue()
{
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_an);                                               
	query_context->setStrValue("*");                                               
	//query_context->setBlockSize(1000000);
	query_context->setBlockSize(50000);

	int num_docs = 0;
	while(!query_context->finished())
	{
		AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
		queryRslt->setWithValues(true);
		AosIILClientObj::getIILClient()->querySafe3(mIILName, queryRslt, 0, query_context, mRundata);
		aos_assert_r(queryRslt, false);
		if (queryRslt->getNumDocs() == 0) break;

		u64 docid;
		OmnString key;                                                
		bool finished = false;
		while(queryRslt->nextDocidValue(docid, key, finished, mRundata))
		{
			if (finished) break;
			checkKey(key, docid);
			num_docs ++;
		}
OmnScreen << "num_docs :" << num_docs << " , mNumDocs: " << mNumDocs << endl;
	}
	aos_assert_r(num_docs == mNumDocs, false);

	verify();
	return true;
}


bool
AosIILProcTester::checkKey(const OmnString &key, const u64 docid)
{
	LKey k(key, docid);
	mapitr_t itr = mMaps.find(k);
	aos_assert_r(itr != mMaps.end(), false);
	itr->second.checknum++;
	//OmnScreen << "checkkey (" << key << " , " << docid << ")" << "(" << itr->second.num << " , " << itr->second.checknum << ")" << endl;
	return true;
}


bool
AosIILProcTester::verify()
{
	mapitr_t itr = mMaps.begin();
	int num = 0;
	for (; itr != mMaps.end(); ++itr)
	{
		if (itr->second.num != itr->second.checknum)
		{
			num ++;
			OmnScreen << "key(" << itr->first.key << "," << itr->first.docid << ")" 
				<< " num(" << itr->second.num << "," << itr->second.checknum << ")"
				<< endl;
		}
		itr->second.checknum = 0;
	}
	aos_assert_r(num == 0, false);
	OmnScreen << "verify verify verify verify total: " << mNumDocs 
		<< " , missing: " << num << " , map size: " << mMaps.size() << endl;
	return true;
}


bool
AosIILProcTester::addKey(const OmnString &key, const u64 docid)
{
	LKey k(key, docid);
	mapitr_t itr = mMaps.find(k);
	if (itr == mMaps.end())
	{
		LValue v(1, 0);
		mMaps.insert(make_pair(k, v));
		//OmnScreen << "addkey (" << key << " , " << docid << ")" << " (" << v.num  << "," << v.checknum << ")" << endl;
		return true;
	}

	itr->second.num++;
	//OmnScreen << "addkey (" << key << " , " << docid << ")" << " (" << itr->second.num  << "," << itr->second.checknum << ")" << endl;
	return true;
}


bool
AosIILProcTester::serializeTo(const AosBuffPtr &conf_buff)
{
	conf_buff->setU32(mNumDocs);
	conf_buff->setU32(mMaps.size());
	mapitr_t itr = mMaps.begin();
	for (; itr != mMaps.end(); ++ itr)
	{
		conf_buff->setOmnStr(itr->first.key);
		conf_buff->setU64(itr->first.docid);
		conf_buff->setInt(itr->second.num);
		conf_buff->setInt(itr->second.checknum);
	}
	return true;
}


bool
AosIILProcTester::serializeFrom(const AosBuffPtr &conf_buff)
{
	mNumDocs = conf_buff->getU32(0);
	u32 size = conf_buff->getU32(0);
	for (u32 i = 0; i < size; i++)
	{
		OmnString key = conf_buff->getOmnStr("");
		aos_assert_r(key != "", false);
		u64 docid = conf_buff->getU64(0);
		aos_assert_r(docid, false);
		LKey k(key, docid);

		int num = conf_buff->getInt(0);
		int numcheck = conf_buff->getInt(0);
		LValue v(num, numcheck);
		mMaps.insert(make_pair(k, v));
	}
	return true;
}


bool
AosIILProcTester::clearMap()
{
	mNumDocs = 0;
	mMaps.clear();
	return true;
}
