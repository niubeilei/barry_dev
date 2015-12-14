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
// This action supports only equi-joins. Further, this action assumes the
// matching column (either 'key' or 'value' column) is sorted. For instance, 
// if the matching column is the key-column, the normal IIL is used; if
// the matching column is the value-column, the companion IIL is used. 
//
// Modification History:
// 2014/01/18 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataJoin/DataJoinCarrier.h"

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


AosDataJoinCarrier::AosDataJoinCarrier()
:
AosDataJoin(),
mCrtTime(0),
mCrtNum(0)
{
}


AosDataJoinCarrier::AosDataJoinCarrier(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata),
mCrtTime(0),
mCrtNum(0)
{
}


AosDataJoinCarrier::~AosDataJoinCarrier()
{
}


bool
AosDataJoinCarrier::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosTaskObjPtr task = mCtlr->getTask();
	aos_assert_r(task, false);

	OmnString time = def->getAttrStr("time_flag");
	aos_assert_r(time == "day" || time == "mnt", false);

	u16 key_head = AosCounterUtil::composeTimeEntryType(
		1, 0, AosStatType::toEnum("sum"), AosTimeGran::toEnum(time));
	mKeyHead.assign((char *)&key_head, sizeof(u16));

	mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	aos_assert_r(mSep != "", false);
	AosConvertAsciiBinary(mSep);

	// Retrieve IIL Scanners
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	aos_assert_r(scanners, false);
	AosXmlTagPtr scanner = scanners->getFirstChild(AOSTAG_IIL_SCANNER);
	aos_assert_r(scanner, false);
	AosIILScannerListenerPtr thisptr(this, false);	
	mIILScanner = AosIILScannerObj::createIILScannerStatic(thisptr, 0, scanner, rdata);
	aos_assert_r(mIILScanner, false);

	mIILAsm = mCtlr->getIILAssembler();
	aos_assert_r(mIILAsm, false);

	return true;
}


bool
AosDataJoinCarrier::run()
{
	mStartTime = OmnGetSecond();
	OmnScreen << "join iil start , time:" << mStartTime << endl;
	
	AosRundataPtr rdata = mRundata;
	mLock->lock();
	bool rslt = runJoin(rdata);
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
AosDataJoinCarrier::getNextKey(
		u64 &time,
		const AosRundataPtr &rdata)
{
	time = 0;

	bool has_more;
	OmnString k, kk;
	vector<OmnString> str;
	AosDataProcStatus::E status;
	
	while (1)
	{
		status = mIILScanner->moveNext(k, has_more, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk:
			 kk = AosCounterUtil::getAllTerm2(k);
			 aos_assert_r(kk !="", false);

			 str.clear();
			 AosCounterUtil::splitTerm2(kk, str);
			 aos_assert_r(str.size() == 2, false);

			 time = str[0].toU64();
			 return true;
		
		case AosDataProcStatus::eRetrievingData:
			 mSem->wait();
			 if(!mDataRetrieveSuccess) return false;
			 break;

		case AosDataProcStatus::eNoMoreData:
			 return true;

		default:
			 return false;
		}
	}
	return false;
}


bool
AosDataJoinCarrier::runJoin(const AosRundataPtr &rdata)
{
	bool rslt = mIILScanner->start(rdata);
	aos_assert_r(rslt, false);

	u64 time = 0;
	
	AosRundata *rdata_raw = rdata.getPtrNoLock();
	while (1)
	{
		rslt = getNextKey(time, rdata);
		aos_assert_r(rslt, false);

		if (time == 0)
		{
			break;
		}

		if (time != mCrtTime)
		{
			if (mCrtTime != 0)
			{
				AosValueRslt value_rslt;
				OmnString str = mKeyHead;
				str << mCrtTime;
				value_rslt.setKeyValue(str.data(), str.length(), true, mCrtNum);
				rslt = mIILAsm->appendEntry(value_rslt, rdata_raw);
				aos_assert_r(rslt, false);
			}
			mCrtTime = time;
			mCrtNum = 1;
		}
		else
		{
			mCrtNum++;
		}
	}

	AosValueRslt value_rslt;
	OmnString str = mKeyHead;
	str << mCrtTime;
	value_rslt.setKeyValue(str.data(), str.length(), true, mCrtNum);
	rslt = mIILAsm->appendEntry(value_rslt, rdata_raw);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataJoinCarrier::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	return mIILScanner->setQueryContext(context, rdata);
}


int
AosDataJoinCarrier::getProgress() 
{
	if(mFinished)
	{
		mProgress = 100;
		return mProgress;
	}
	
	aos_assert_r(mIILScanner, 0);
	mProgress = mIILScanner->getProgress();
	//OmnScreen << "join:[" << this << "], mProgress:" << mProgress;
	return mProgress;
}


void
AosDataJoinCarrier::clear()
{
	mCtlr = 0;
	mIILScanner->clear();
	mIILScanner = 0;
	mIILAsm = 0;
}


AosDataJoinPtr
AosDataJoinCarrier::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinCarrier * join = OmnNew AosDataJoinCarrier(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinCarrier::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	return true;
}

