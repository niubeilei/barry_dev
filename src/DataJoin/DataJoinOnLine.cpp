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
#include "DataJoin/DataJoinOnLine.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterUtil/CounterUtil.h"
#include "DataJoin/DataJoinCtlr.h"
#include "DataRecord/DataRecord.h"
#include "DataAssembler/DataAssembler.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "UtilTime/TimeFormat.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"


AosDataJoinOnLine::AosDataJoinOnLine()
:
AosDataJoin(),
mEndDay(0)
{
}


AosDataJoinOnLine::AosDataJoinOnLine(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata),
mEndDay(0)
{
}


AosDataJoinOnLine::~AosDataJoinOnLine()
{
}


bool
AosDataJoinOnLine::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// The config is in the form:
	// 	<sdoc 
	// 		<AOSTAG_IIL_SCANNERS>
	// 			<AOSTAG_IILSCANNER .../>
	// 		</AOSTAG_IIL_SCANNERS>
	// 		<iilassembler .../>
	// 	</sdoc>
	AosTaskObjPtr task = mCtlr->getTask();
	aos_assert_r(task, false);

	mValue = def->getAttrU64("zky_value", 1);

	OmnString end_date = def->getAttrStr(AOSTAG_PROC_DATETIME);
	aos_assert_r(end_date != "", false);
	mEndDay = AosTimeFormat::strToEpochDay(end_date, AosTimeFormat::eYYYYMMDD);  
	aos_assert_r(mEndDay > 0, false);

	u16 key_head = AosCounterUtil::composeTimeEntryType(
		2, 1, AosStatType::toEnum("sum"), AosTimeGran::toEnum("day"));
	mKeyHead.assign((char *)&key_head, sizeof(u16));

	u16 key_head2 = AosCounterUtil::composeTimeEntryType(
		2, 1, AosStatType::toEnum("sum"), AosTimeGran::toEnum("mnt"));
	mKeyHead2.assign((char *)&key_head2, sizeof(u16));

	mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	aos_assert_r(mSep != "", false);
	AosConvertAsciiBinary(mSep);

	OmnString end_day_iilname = def->getAttrStr("end_day_iilname");
	aos_assert_r(end_day_iilname != "", false);
	mEndDayMap = AosIILEntryMapMgr::retrieveIILEntryMap(end_day_iilname, rdata);
	aos_assert_r(mEndDayMap, false);

	mEndDayMap->readlock();
	OmnScreen << "end day map size:" << mEndDayMap->size() << endl;
	mEndDayMap->unlock();

	OmnString type_iilname = def->getAttrStr("type_iilname");
	aos_assert_r(type_iilname != "", false);
	mTypeMap = AosIILEntryMapMgr::retrieveIILEntryMap(type_iilname, rdata);
	aos_assert_r(mTypeMap, false);

	mTypeMap->readlock();
	OmnScreen << "type map size:" << mTypeMap->size() << endl;
	mTypeMap->unlock();

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

	return true;
}
	

bool
AosDataJoinOnLine::run()
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
AosDataJoinOnLine::getNextKey(
		OmnString &key,
		u64 &value,
		const AosRundataPtr &rdata)
{
	key = "";
	value = 0;
	bool has_more;
	AosDataProcStatus::E status;

	while (1)
	{
		status = mIILScanner->moveNext(key, has_more, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk:
			 value = mIILScanner->getCrtValue();
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


u64
AosDataJoinOnLine::getEndDay(
		const OmnString &key,
		const AosRundataPtr &rdata)
{
	aos_assert_r(key != "", 0);
	
	u64 end_day = mEndDay;
	mEndDayMap->readlock();
	AosIILEntryMapItr itr = mEndDayMap->find(key);
	if (itr != mEndDayMap->end())
	{
		OmnString end_day_str = itr->second;
		end_day = atoi(end_day_str.data());
	}
	mEndDayMap->unlock();
	
	return end_day;
}


u64
AosDataJoinOnLine::getType(
		const OmnString &key,
		const AosRundataPtr &rdata)
{
	aos_assert_r(key != "", 0);
	
	u64 type = 0;
	mTypeMap->readlock();
	AosIILEntryMapItr itr = mTypeMap->find(key);
	if (itr != mTypeMap->end())
	{
		OmnString type_str = itr->second;
		type = atoi(type_str.data());
	}
	mTypeMap->unlock();
	
	return type;
}


bool
AosDataJoinOnLine::sendStart(const AosRundataPtr &rdata)
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
AosDataJoinOnLine::sendFinish(const AosRundataPtr &rdata)
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
AosDataJoinOnLine::runJoin(const AosRundataPtr &rdata)
{
	bool rslt = mIILScanner->start(rdata);
	aos_assert_r(rslt, false);
	
	mDayMap.clear();
	mMonthMap.clear();

	OmnString key;
	u64 start_day, end_day, type;
	while (1)
	{
		rslt = getNextKey(key, start_day, rdata);
		aos_assert_r(rslt, false);

		if (key == "")
		{
			break;
		}

		end_day = getEndDay(key, rdata);
		type = getType(key, rdata);
	
		rslt = appendOnLineEntry(type, start_day, end_day, rdata);
		aos_assert_r(rslt, false);	
	}

	OmnString rslt_key;
	u64 rslt_value;
	map<OmnString, u64>::iterator itr = mDayMap.begin();
	while (itr != mDayMap.end())
	{
		rslt_key = itr->first;
		rslt_value = itr->second;

		AosValueRslt value_rslt;
		value_rslt.setKeyValue(rslt_key.data(), rslt_key.length(), true, rslt_value);
		rslt = mIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);

		itr++;
	}

	itr = mMonthMap.begin();
	while (itr != mMonthMap.end())
	{
		rslt_key = itr->first;
		rslt_value = itr->second;

		AosValueRslt value_rslt;
		value_rslt.setKeyValue(rslt_key.data(), rslt_key.length(), true, rslt_value);
		rslt = mIILAsm2->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);

		itr++;
	}

	return true;
}

	
bool
AosDataJoinOnLine::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	return mIILScanner->setQueryContext(context, rdata);
}


int
AosDataJoinOnLine::getProgress() 
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
AosDataJoinOnLine::appendOnLineEntry(
		const u64 &type,
		const u64 &start_day,
		const u64 &end_day,
		const AosRundataPtr &rdata)
{
	OmnString str = mKeyHead;
	str << type << mSep; 
	int len = str.length();
	
	map<OmnString, u64>::iterator itr;
	OmnString day_str; 

	for (u64 i=start_day; i<end_day; i++)
	{
		if (i < 10) {
			day_str = "0000";
		} else if (i < 100) {
			day_str = "000";
		} else if (i < 1000) {
			day_str = "00";
		} else if (i < 10000) {
			day_str = "0";
		} else {
			day_str = "";
		}
		day_str << i;

		str.setLength(len);
		str << day_str;
	
		itr = mDayMap.find(str);
		if (itr == mDayMap.end())
		{
			mDayMap[str] = mValue;
		}
		else
		{
			(itr->second)++;
		}
	}

	str = mKeyHead2;
	str << type << mSep;
	len = str.length();

	OmnString month_str;
	u64 start_month = start_day * AOSTIMEFORMAT_SECONDS_PRE_DAY;
	start_month = AosTimeUtil::getEpochMonth(start_month);
	u64 end_month = end_day * AOSTIMEFORMAT_SECONDS_PRE_DAY;
	end_month = AosTimeUtil::getEpochMonth(end_month);

	for (u64 i=start_month; i<end_month; i++)
	{
		if (i < 10) {
			month_str = "00";
		} else if (i < 100) {
			month_str = "0";
		} else {
			month_str = "";
		}
		month_str << i;

		str.setLength(len);
		str << month_str;
	
		itr = mMonthMap.find(str);
		if (itr == mMonthMap.end())
		{
			mMonthMap[str] = mValue;
		}
		else
		{
			(itr->second)++;
		}
	}

	return true;
}


void
AosDataJoinOnLine::clear()
{
	mCtlr = 0;
	mIILScanner->clear();
	mIILScanner = 0;
	mIILAsm = 0;
	mIILAsm2 = 0;
}


AosDataJoinPtr
AosDataJoinOnLine::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinOnLine * join = OmnNew AosDataJoinOnLine(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinOnLine::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	return true;
}

