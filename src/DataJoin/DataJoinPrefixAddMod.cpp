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
// IIL joins are over-simplified 'table joins', where IILs are special 
// form of 'tables'. There are only two columns in IILs: Key and Value.
// Two IIls can be joined in a number of ways:
// 		eKey
// 		eValue
// 		eFamily
// 		eMember
// 		eSpecificMember
//
// This action supports only prefix-joins. Further, this action assumes the
// matching column is sorted. 
//
// Processing:
// 1. LHS exists but RHS does not exist:
// 2. LHS not exist but RHS exists:
// 3. Both LHS and RHS exists:
//
// Modification History:
// 2013/12/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataJoin/DataJoinPrefixAddMod.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterUtil/CounterUtil.h"
#include "DataJoin/DataJoinCtlr.h"
#include "DataRecord/DataRecord.h"
#include "DataAssembler/DataAssembler.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "UtilTime/TimeFormat.h"
#include "XmlUtil/XmlTag.h"


AosDataJoinPrefixAddMod::AosDataJoinPrefixAddMod()
:
AosDataJoin(),
mIILScanner1(0),
mIILScanner2(0),
mIILAsm(0)

{
}


AosDataJoinPrefixAddMod::AosDataJoinPrefixAddMod(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata),
mIILScanner1(0),
mIILScanner2(0),
mIILAsm(0)
{
}


AosDataJoinPrefixAddMod::~AosDataJoinPrefixAddMod()
{
}


bool
AosDataJoinPrefixAddMod::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosTaskObjPtr task = mCtlr->getTask();
	aos_assert_r(task, false);

	mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	aos_assert_r(mSep != "", false);
	AosConvertAsciiBinary(mSep);

	// Retrieve IIL Scanners
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	aos_assert_r(scanners, false);

	AosXmlTagPtr scanner = scanners->getFirstChild(AOSTAG_IIL_SCANNER);
	aos_assert_r(scanner, false);
	AosIILScannerListenerPtr thisptr(this, false);	
	mIILScanner1 = AosIILScannerObj::createIILScannerStatic(thisptr, 0, scanner, rdata);
	aos_assert_r(mIILScanner1, false);

	scanner = scanners->getNextChild();
	aos_assert_r(scanner, false);
	mIILScanner2 = AosIILScannerObj::createIILScannerStatic(thisptr, 0, scanner, rdata);
	aos_assert_r(mIILScanner2, false);

	mIILAsm = createIILAsm(def->getFirstChild("iilasm"), task, rdata);
	aos_assert_r(mIILAsm, false);

	return true;
}


bool
AosDataJoinPrefixAddMod::run()
{
	mStartTime = OmnGetSecond();
	OmnScreen << "join iil start , time:" << mStartTime << endl;
	
	AosRundataPtr rdata = mRundata;
	mLock->lock();
	bool rslt = sendStart(rdata);
	if(rslt)
	{
		rslt = runJoin(rdata);
		sendFinish(rdata.getPtrNoLock());
	}
	mLock->unlock();
	mFinished = true;
	mSuccess = rslt;
	
	mEndTime = OmnGetSecond();
	OmnScreen << "join iil finished, time:" << mEndTime
		<< ", success:" << mSuccess
		<< ", spend:" << (mEndTime - mStartTime) << endl;
	
	AosDataJoinPtr thisptr(this, true);
	mCtlr->joinFinished(thisptr, rdata);

	clear();
	return true;
}


bool
AosDataJoinPrefixAddMod::sendStart(const AosRundataPtr &rdata)
{
	aos_assert_r(mIILAsm, false);
	
	bool rslt = mIILAsm->sendStart(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	return true;
}

	
bool
AosDataJoinPrefixAddMod::sendFinish(const AosRundataPtr &rdata)
{
	aos_assert_r(mIILAsm, false);
	
	bool rslt = mIILAsm->sendFinish(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataJoinPrefixAddMod::runJoin(const AosRundataPtr &rdata)
{
	bool rslt = mIILScanner1->start(rdata);
	aos_assert_r(rslt, false);

	rslt = mIILScanner2->start(rdata);
	aos_assert_r(rslt, false);

	OmnString k, key, value, iilname1, iilname2;
	bool has_more;
	AosDataProcStatus::E status;
	map<OmnString, OmnString> key_map;
	int idx;

	bool stat = true;
	while (1 && stat)
	{
		status = mIILScanner1->moveNext(k, has_more, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk:
			 idx = k.indexOf(mSep, 0);
			 key = k.substr(0, idx-1);
			 value = k.substr(idx+1);
			 aos_assert_r(key != "", false);
			 aos_assert_r(value != "", false);
			 key_map[key] = value;
			 break;

		case AosDataProcStatus::eRetrievingData:
			 mSem->wait();
			 if(!mDataRetrieveSuccess) return false;
			 break;

		case AosDataProcStatus::eNoMoreData:
			 stat = false;
			 break;

		default:
			 break;
		}
	}

	stat = true;
	while (1 && stat)
	{
		status = mIILScanner2->moveNext(k, has_more, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk:
			 idx = k.indexOf(mSep, 0);
			 key = k.substr(0, idx-1);
			 value = k.substr(idx+1);
			 aos_assert_r(key != "", false);
			 aos_assert_r(value != "", false);
			 key_map[key] = value;
			 break;

		case AosDataProcStatus::eRetrievingData:
			 mSem->wait();
			 if(!mDataRetrieveSuccess) return false;
			 break;

		case AosDataProcStatus::eNoMoreData:
			 stat = false;
			 break;

		default:
			 break;
		}
	}

	iilname1 = mIILScanner1->getIILName();
	aos_assert_r(iilname1 != "", false);

	iilname2 = mIILScanner2->getIILName();
	aos_assert_r(iilname2 != "", false);

	rslt = AosIILClientObj::getIILClient()->deleteIIL(iilname1, true, rdata);
	aos_assert_r(rslt, false);
	
	rslt = AosIILClientObj::getIILClient()->deleteIIL(iilname2, true, rdata);
	aos_assert_r(rslt, false);

	map<OmnString, OmnString>::iterator it;
	AosValueRslt value_rslt;
	OmnString str;
	for (it = key_map.begin(); it != key_map.end(); it++)
	{
		str = it->first;
		str << mSep << it->second;
		value_rslt.setKeyValue(str.data(), str.length(), true, 1);
		rslt = mIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}
	return true;
}

	
bool
AosDataJoinPrefixAddMod::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{

	if (mIILScanner1->setQueryContext(context, rdata) &&
		mIILScanner2->setQueryContext(context, rdata))
	{
		return true;
	}
	return false;
}


int
AosDataJoinPrefixAddMod::getProgress() 
{
	if (mFinished)
	{
		mProgress = 100;
		return mProgress;
	}
	
	aos_assert_r(mIILScanner1, 0);
	aos_assert_r(mIILScanner2, 0);
	int progress1 = mIILScanner1->getProgress();
	int progress2 = mIILScanner2->getProgress();
	return mProgress = (progress1 + progress2)/2;
}


void
AosDataJoinPrefixAddMod::clear()
{
	mCtlr = 0;
	mIILScanner1->clear();
	mIILScanner2->clear();
	mIILScanner1 = 0;
	mIILScanner2 = 0;
	mIILAsm = 0;
}


AosDataJoinPtr
AosDataJoinPrefixAddMod::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinPrefixAddMod * join = OmnNew AosDataJoinPrefixAddMod(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinPrefixAddMod::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	return true;
}

