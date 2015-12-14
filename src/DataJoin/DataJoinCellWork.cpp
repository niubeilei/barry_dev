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
#include "DataJoin/DataJoinCellWork.h"

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


static OmnString sgAllProvince = "99";


AosDataJoinCellWork::AosDataJoinCellWork()
:
AosDataJoin()
{
}


AosDataJoinCellWork::AosDataJoinCellWork(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata)
{
}


AosDataJoinCellWork::~AosDataJoinCellWork()
{
}


bool
AosDataJoinCellWork::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosTaskObjPtr task = mCtlr->getTask();
	aos_assert_r(task, false);

	u16 key_head = AosCounterUtil::composeTimeEntryType(
		2, 1, AosStatType::toEnum("sum"), AosTimeGran::toEnum("day"));
	mKeyHead.assign((char *)&key_head, sizeof(u16));

	u16 key_head2 = AosCounterUtil::composeTimeEntryType(
		2, 0, AosStatType::toEnum("sum"), AosTimeGran::toEnum("day"));
	mKeyHead2.assign((char *)&key_head2, sizeof(u16));

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

	mIILAsm = createIILAsm(def->getFirstChild("iilasm"), task, rdata);
	mIILAsm2 = createIILAsm(def->getFirstChild("iilasm2"), task, rdata);

	return true;
}


bool
AosDataJoinCellWork::run()
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
AosDataJoinCellWork::getNextKey(
		OmnString &city,
		u64 &day,
		bool &finish,
		AosRundata *rdata_raw)
{
	city = "";
	day = 0;
	finish = false;

	u64 value;
	bool has_more = true;
	OmnString k, kk;
	vector<OmnString> str;
	AosDataProcStatus::E status;
	
	while (1)
	{
		status = mIILScanner->moveNext(k, has_more, rdata_raw);
		switch (status)
		{
		case AosDataProcStatus::eOk:
			 value = mIILScanner->getCrtValue();
			 if (value == 0) continue;

			 kk = AosCounterUtil::getAllTerm2(k);
			 aos_assert_r(kk !="", false);

			 str.clear();
			 AosCounterUtil::splitTerm2(kk, str);
			 aos_assert_r(str.size() == 3, false);

			 city = str[0];
			 day = str[1].toU64();
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
AosDataJoinCellWork::sendStart(const AosRundataPtr &rdata)
{
	bool rslt = true;
	if (mIILAsm)
	{
		mIILAsm->sendStart(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}

	if (mIILAsm2)
	{
		rslt = mIILAsm2->sendStart(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}

	return true;
}

	
bool
AosDataJoinCellWork::sendFinish(const AosRundataPtr &rdata)
{
	bool rslt = true;
	if (mIILAsm)
	{
		rslt = mIILAsm->sendFinish(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}

	if (mIILAsm2)
	{
		rslt = mIILAsm2->sendFinish(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosDataJoinCellWork::runJoin(const AosRundataPtr &rdata)
{
	bool rslt = mIILScanner->start(rdata);
	aos_assert_r(rslt, false);

	bool finish = false;
	OmnString city, str;
	u64 day, min_day = 50000, max_day = 0;
	AosValueRslt value_rslt;

	set<OmnString> city_set;
	map<OmnString, u64> map1;
	map<OmnString, u64> map2;
	set<OmnString>::iterator set_itr;
	map<OmnString, u64>::iterator map_itr;

	AosRundata *rdata_raw = rdata.getPtr();

	while (!finish)
	{
		rslt = getNextKey(city, day, finish, rdata_raw);
		if (finish) break;
		aos_assert_r(rslt, false);

		if (min_day > day) min_day = day;
		if (max_day < day) max_day = day;
		city_set.insert(city);

		str = mKeyHead;
		str << city << mSep;
		str << day;
		map_itr = map1.find(str);
		if (map_itr == map1.end())
		{
			map1[str] = 1;
		}
		else
		{
			map_itr->second++;
		}

		str = mKeyHead2;
		str << day << mSep << city;
		map_itr = map2.find(str);
		if (map_itr == map2.end())
		{
			map2[str] = 1;
		}
		else
		{
			map_itr->second++;
		}
	}

	set_itr = city_set.begin();
	while (set_itr != city_set.end())
	{
		city = *set_itr;
		for (day = min_day; day <= max_day; day++)
		{
			str = mKeyHead;
			str << city << mSep << day;
			map_itr = map1.find(str);
			if (map_itr == map1.end())
			{
				map1[str] = 0;
			}

			str = mKeyHead2;
			str << day << mSep << city;
			map_itr = map2.find(str);
			if (map_itr == map2.end())
			{
				map2[str] = 0;
			}
		}
		set_itr++;
	}

	u64 value;
	if (mIILAsm)
	{
		map_itr = map1.begin();
		while (map_itr != map1.end())
		{
			str = map_itr->first;
			value = map_itr->second;
			value_rslt.setKeyValue(str.data(), str.length(), true, value);
			rslt = mIILAsm->appendEntry(value_rslt, rdata_raw);
			aos_assert_r(rslt, false);
			map_itr++;
		}
	}

	if (mIILAsm2)
	{
		map_itr = map2.begin();
		while (map_itr != map2.end())
		{
			str = map_itr->first;
			value = map_itr->second;
			value_rslt.setKeyValue(str.data(), str.length(), true, value);
			rslt = mIILAsm2->appendEntry(value_rslt, rdata_raw);
			aos_assert_r(rslt, false);
			map_itr++;
		}
	}

	return true;
}

	
bool
AosDataJoinCellWork::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	return mIILScanner->setQueryContext(context, rdata);
}


int
AosDataJoinCellWork::getProgress() 
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
AosDataJoinCellWork::clear()
{
	mCtlr = 0;
	mIILScanner->clear();
	mIILScanner = 0;
	mIILAsm = 0;
	mIILAsm2 = 0;
}


AosDataJoinPtr
AosDataJoinCellWork::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinCellWork * join = OmnNew AosDataJoinCellWork(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinCellWork::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	return true;
}

