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
// 2012/07/30	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataJoin/DataJoinCellFluctuate.h"

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


static int sgTimeFrame = 6;


AosDataJoinCellFluctuate::AosDataJoinCellFluctuate()
:
AosDataJoin(),
mTimeFrame(sgTimeFrame)
{
}


AosDataJoinCellFluctuate::AosDataJoinCellFluctuate(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata),
mTimeFrame(sgTimeFrame)
{
}


AosDataJoinCellFluctuate::~AosDataJoinCellFluctuate()
{
}


bool
AosDataJoinCellFluctuate::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosTaskObjPtr task = mCtlr->getTask();
	aos_assert_r(task, false);

	u16 key_head = AosCounterUtil::composeTimeEntryType(
		3, 1, AosStatType::toEnum("sum"), AosTimeGran::toEnum("day"));
	mKeyHead.assign((char *)&key_head, sizeof(u16));

	u16 key_head2 = AosCounterUtil::composeTimeEntryType(
		2, 1, AosStatType::toEnum("sum"), AosTimeGran::toEnum("day"));
	mKeyHead2.assign((char *)&key_head2, sizeof(u16));

	mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	aos_assert_r(mSep != "", false);
	AosConvertAsciiBinary(mSep);

	mTimeFrame = def->getAttrInt("time_frame", sgTimeFrame);
	aos_assert_r(mTimeFrame > 0, false);

	// Retrieve IIL Scanners
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	aos_assert_r(scanners, false);
	AosXmlTagPtr scanner = scanners->getFirstChild(AOSTAG_IIL_SCANNER);
	aos_assert_r(scanner, false);
	AosIILScannerListenerPtr thisptr(this, false);	
	mIILScanner = AosIILScannerObj::createIILScannerStatic(thisptr, 0, scanner, rdata);
	aos_assert_r(mIILScanner, false);

	mIILAsm = createIILAsm(def->getFirstChild("iilasm"), task, rdata);
	aos_assert_r(mIILAsm, false);

	mIILAsm2 = createIILAsm(def->getFirstChild("iilasm2"), task, rdata);
	aos_assert_r(mIILAsm2, false);

	OmnString city_iilname = def->getAttrStr("city_iilname");
	aos_assert_r(city_iilname != "", false);
	mKeyCityMap = AosIILEntryMapMgr::retrieveIILEntryMap(city_iilname, rdata);
	aos_assert_r(mKeyCityMap, false);
	
	return true;
}


bool
AosDataJoinCellFluctuate::run()
{
	mStartTime = OmnGetSecond();
	OmnScreen << "join iil start , time:" << mStartTime << endl;
	
	AosRundataPtr rdata = mRundata;
	mLock->lock();
	bool rslt = sendStart(rdata);
	if(rslt)
	{
		rslt = runJoin(rdata);
		sendFinish(rdata);
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
AosDataJoinCellFluctuate::getNextKey(
		OmnString &key,
		u64 &day,
		u64 &value,
		bool &finish,
		const AosRundataPtr &rdata)
{
	key = "";
	day = 0;
	value = 0;
	finish = false;

	bool has_more = true;
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

			 key = str[0];
			 day = str[1].toU64();
			 value = mIILScanner->getCrtValue();
			 return true;
		
		case AosDataProcStatus::eRetrievingData:
			 mSem->wait();
			 if(!mDataRetrieveSuccess) return false;
			 break;

		case AosDataProcStatus::eNoMoreData:
			 finish = true;
			 return true;

		default:
			 return false;
		}
	}
	return false;
}


bool
AosDataJoinCellFluctuate::sendStart(const AosRundataPtr &rdata)
{
	aos_assert_r(mIILAsm, false);
	aos_assert_r(mIILAsm2, false);
	
	bool rslt = mIILAsm->sendStart(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	rslt = mIILAsm2->sendStart(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	return true;
}

	
bool
AosDataJoinCellFluctuate::sendFinish(const AosRundataPtr &rdata)
{
	aos_assert_r(mIILAsm, false);
	aos_assert_r(mIILAsm2, false);
	
	bool rslt = mIILAsm->sendFinish(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	rslt = mIILAsm2->sendFinish(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataJoinCellFluctuate::runJoin(const AosRundataPtr &rdata)
{
	bool rslt = mIILScanner->start(rdata);
	aos_assert_r(rslt, false);
	
	mValueVector.clear();

	AosRundata *rdata_raw = rdata.getPtrNoLock();

	bool finish = false;
	OmnString key;
	u64 day, value;

	while (!finish)
	{
		rslt = getNextKey(key, day, value, finish, rdata);
		if (finish) break;
		aos_assert_r(rslt, false);

		if (key != mCrtKey)
		{
			rslt = appendAllEntry(rdata_raw);
			aos_assert_r(rslt, false);
	
			mValueVector.clear();

			mCrtKey = key;
			mFirstDay = day;
			mValueVector.push_back(value);
		}
		else
		{
			aos_assert_r(day > mFirstDay, false);

			u64 crt = day - mFirstDay;
			while (mValueVector.size() <= crt)
			{
				mValueVector.push_back(0);
			}
			mValueVector[crt] = value;
		}
	}

	rslt = appendAllEntry(rdata_raw);
	aos_assert_r(rslt, false);

	return true;
}

	
bool
AosDataJoinCellFluctuate::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	return mIILScanner->setQueryContext(context, rdata);
}


int
AosDataJoinCellFluctuate::getProgress() 
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


bool
AosDataJoinCellFluctuate::appendAllEntry(AosRundata *rdata_raw)
{
	OmnString city_code;
	mKeyCityMap->lock();
	AosIILEntryMapItr itr = mKeyCityMap->find(mCrtKey);
	if (itr != mKeyCityMap->end()) 
	{
		city_code << itr->second;
	}
	mKeyCityMap->unlock();
	if (city_code == "") return true;

	if (city_code.length() == 1)
	{
		OmnString tmp = "0";
		tmp << city_code;
		city_code = tmp;
	}

	u64 size = mValueVector.size();
	if (size < mTimeFrame) return true;

	bool rslt;
	OmnString str;
	u64 day, value, total, avg, vv;
	for (u64 i = mTimeFrame; i < size; i++)
	{
		day	= mFirstDay + i;
		value =  mValueVector[i];
		
		total = 0;
		for (u64 j = 1; j <= mTimeFrame; j++)
		{
			total += mValueVector[i - j];
		}
		avg = total / mTimeFrame;

		if (avg >= value)
		{
			vv = avg - value;
		}
		else
		{
			vv = value - avg;
		}

		str = mKeyHead;
		str << "99" << mSep << day << mSep << mCrtKey;
		AosValueRslt value_rslt;
		value_rslt.setKeyValue(str.data(), str.length(), true, vv);
		rslt = mIILAsm->appendEntry(value_rslt, rdata_raw);
		aos_assert_r(rslt, false);

		str = mKeyHead;
		str << city_code << mSep << day << mSep << mCrtKey;
		value_rslt.setKeyValue(str.data(), str.length(), true, vv);
		rslt = mIILAsm->appendEntry(value_rslt, rdata_raw);
		aos_assert_r(rslt, false);
	
		str = mKeyHead2;
		str << mCrtKey << mSep << day;
		value_rslt.setKeyValue(str.data(), str.length(), true, vv);
		rslt = mIILAsm2->appendEntry(value_rslt, rdata_raw);
		aos_assert_r(rslt, false);
	}

	return true;
}


void
AosDataJoinCellFluctuate::clear()
{
	mCtlr = 0;
	mIILScanner->clear();
	mIILScanner = 0;
	mIILAsm = 0;
	mIILAsm2 = 0;
}


AosDataJoinPtr
AosDataJoinCellFluctuate::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinCellFluctuate * join = OmnNew AosDataJoinCellFluctuate(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinCellFluctuate::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	return true;
}

