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
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataScanner/IILDataScanner.h"

#include "API/AosApi.h"
#include "DataScanner/Ptrs.h"
#include "DbQuery/Query.h"
#include "SEUtil/IILName.h"
#include "Util/ValueRslt.h"
#include "Util/KeyValuePair.h"
#include "ValueSel/ValueSel.h"
#include "Thread/Sem.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/QueryContextObj.h"

#include "CompressUtil/Compress.h"

AosIILDataScanner::AosIILDataScanner(const bool flag)
:
AosDataScanner(AOSDATASCANNER_IIL, AosDataScannerType::eIIL, flag),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mReadingNum(0),
mBlockSize(0),
mTotalEntries(0),
mTotalReadEntries(0)
{
}


AosIILDataScanner::AosIILDataScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataScanner(AOSDATASCANNER_IIL, AosDataScannerType::eIIL, false),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mReadingNum(0),
mLimitMaxNum(eLimitMaxNum),
mBlockSize(0),
mTotalEntries(0),
mTotalReadEntries(0),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	bool rslt = config(conf, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosIILDataScanner::~AosIILDataScanner()
{
}


bool
AosIILDataScanner::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	return true;
}


bool
AosIILDataScanner::getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	if (mTotalEntries <= 0)
	{
		buff = 0;
		return true;
	}

	mLock->lock();
	while( mQueue.size() == 0)
	{
		if (mTotalReadEntries == mTotalEntries)
		{
			buff = 0;
			mLock->unlock();
			return true;
		}

		bool timeout = false;
		mCondVar->timedWait(mLock, timeout, 1);
	}

	AosBuffPtr xml_buff = mQueue.front();
	aos_assert_r(xml_buff, false);
	mQueue.pop();
	mLock->unlock();

	u32 num_entries = xml_buff->getU32(0);
	u32 bufflen = xml_buff->getU32(0);
	if (bufflen > 0)
	{
		buff = xml_buff->getBuff(bufflen, true AosMemoryCheckerArgs);
	}
	else
	{
		buff = 0;
		aos_assert_r(num_entries == 0, false);
		aos_assert_r(mTotalReadEntries == mTotalEntries, false);
	}

	mLock->lock();
	mTotalReadEntries += num_entries;
	aos_assert_rl(mTotalReadEntries <= mTotalEntries, mLock, false);
	mLock->unlock();
OmnScreen << "  cccccccccc  " << endl;
	return true;
}


bool                                  
AosIILDataScanner::initIILDataScanner(      
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)   
{
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);

/*
	OmnString contents = conf->getNodeText("zky_context");
	aos_assert_r(contents != "", false);

	AosKeyValuePairPtr kvpair = OmnNew AosKeyValuePair(rdata, contents);
	aos_assert_r(kvpair, false);

	mIILName = kvpair->getAttrStr(AOSTAG_IILNAME);
	aos_assert_r(mIILName != "", false);

	mBlockSize = kvpair->getAttrU32("zky_blocksize", eDftBlockSize);
*/

	mIILName = conf->getAttrStr(AOSTAG_IILNAME);
	aos_assert_r(mIILName != "", false);

	mBlockSize = conf->getAttrU32("zky_blocksize", eDftBlockSize);

	mTotalEntries = AosIILClient::getSelf()->getTotalNumDocs(mIILName, rdata);


	if (mBlockSize > mTotalEntries)
	{
		mBlockSize = mTotalEntries;
	}

	AosXmlTagPtr query_cond = conf->getFirstChild("query_cond");
	aos_assert_r(query_cond, false);

	AosOpr opr = AosOpr_toEnum(query_cond->getAttrStr("zky_opr"));
	if (!AosOpr_valid(opr))
	{
		rdata->setError() << "Operator is invalid: " << opr;        
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool reverse = query_cond->getAttrBool("zky_reverse", false);

	OmnString value = query_cond->getAttrStr("zky_value");
	if(value == "*")
	{
		opr = eAosOpr_an;
	}

	OmnString value2 = query_cond->getAttrStr("zky_value2");

	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	aos_assert_r(query_context, false);
	
	query_context->setOpr(opr);
	query_context->setStrValue(value);                       
	query_context->setU64Value(atoll(value.data()));
	query_context->setReverse(reverse);
	query_context->setBlockSize(mBlockSize);

	if (value2 != "")
	{
		query_context->setStrValue2(value2);
    	query_context->setU64Value2(atoll(value2.data()));
	}

	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt->setWithValues(true);

	readData(query_context, query_rslt, rdata);
	return true;	
}


bool
AosIILDataScanner::readData(
		const AosQueryContextObjPtr &query_context,
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	if (mTotalEntries <= 0)
		return true;

OmnScreen << "  aaaaaaaaaa  " << endl;
	AosAsyncRespCallerPtr thisPtr(this, false);
	bool rslt = AosQueryColumnAsync(mIILName, query_rslt, 0, query_context, thisPtr, rdata);
	aos_assert_r(rslt, false);

	return true;
}


void
AosIILDataScanner::callback(
	const AosTransPtr &trans,
	const AosBuffPtr &resp,
	const bool svr_death)
{
	if(svr_death)
	{
		OmnScreen << "send Trans failed. svr death."
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; death_svr_id:" << trans->getToSvrId()
			<< endl;
		return;
	}
	
	aos_assert(resp);
	bool rslt = resp->getU8(0);
	if(!rslt) return; 
	
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	aos_assert(query_context);
	
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	aos_assert(query_rslt);

	AosXmlTagPtr xml = getXmlFromBuff(resp, mRundata);
	aos_assert(xml);
	rslt = query_context->serializeFromXml(xml);
	aos_assert(rslt);

	if (query_rslt)
	{
		xml = getXmlFromBuff(resp, mRundata);
		aos_assert(xml);
		rslt = query_rslt->serializeFromXml(xml);
		aos_assert(rslt);
	}

	u32 numdocs = query_rslt->getNumDocs();

	OmnString key;
	u64 value;
	bool finished = false;

	AosBuffPtr resp_buff = OmnNew AosBuff(200 AosMemoryCheckerArgs);
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	resp_buff->setU32(numdocs);
	u32 totaldocs = 0;
	while(1)
	{
		rslt = query_rslt->nextDocidValue(value, key, finished, mRundata);
		aos_assert(rslt);

		if (finished)
		{
			break;
		}

		buff->setOmnStr(key);
		buff->setU64(value);
		totaldocs++;
	}
	aos_assert(totaldocs == numdocs);
	//resp_buff->setU32(buff->dataLen()+sizeof(u32));
	resp_buff->setU32(buff->dataLen());
	resp_buff->setBuff(buff);
	resp_buff->reset();
OmnScreen << "  bbbbbbbbbb  " << endl;


	mLock->lock();
	mQueue.push(resp_buff);
	mCondVar->signal();
	aos_assert_l(numdocs + mTotalReadEntries <= mTotalEntries, mLock);
	if (numdocs + mTotalReadEntries < mTotalEntries)
	{
		mLock->unlock();
		query_rslt = AosQueryRsltObj::getQueryRsltStatic();
		query_rslt->setWithValues(true);
		readData(query_context, query_rslt, mRundata);
		return;
	}
	mLock->unlock();
}


AosXmlTagPtr
AosIILDataScanner::getXmlFromBuff(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff, 0);
	
	u32 len = buff->getU32(0);
	aos_assert_r(len, 0);

	AosBuffPtr b = buff->getBuff(len, false AosMemoryCheckerArgs);
	aos_assert_r(b, 0);

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(b->data(), len, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, 0);

	return xml;
}


bool
AosIILDataScanner::signal(const int threadLogicId)
{
	return true;
}


bool
AosIILDataScanner::createIILDataScanner(
		vector<AosDataScannerObjPtr> &scanners,
		const AosRundataPtr &rdata)
{
	try
	{
		AosIILDataScanner* scanner = OmnNew AosIILDataScanner(false);
		scanners.push_back(scanner);
	}
	catch(...)
	{
		OmnAlarm << "Faild to create FileScanner" << enderr;
		return false;
	}
	return true;
}


bool
AosIILDataScanner::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILDataScanner::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosDataScannerObjPtr 
AosIILDataScanner::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


int64_t 
AosIILDataScanner::getTotalFileLength()
{
	return 100;
}

#endif
