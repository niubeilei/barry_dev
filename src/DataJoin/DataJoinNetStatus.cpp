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
#if 0
#include "DataJoin/DataJoinNetStatus.h"


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


static u64 sgDayInterval = 90;
static bool sgAddOnNetFlag = false;
static OmnString sgAllProvince = "99";


AosDataJoinNetStatus::AosDataJoinNetStatus()
:
AosDataJoin(),
mProcAll(false),
mEpochDay(0),
mKeyFieldIdx(-1),
mCityFieldIdx(-1),
mCarrierFieldIdx(-1),
mDayFieldIdx(-1),
mIsFirstKey(true),
mNeedMove(true),
mCrtDay(0)
{
}


AosDataJoinNetStatus::AosDataJoinNetStatus(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata),
mProcAll(false),
mEpochDay(0),
mKeyFieldIdx(-1),
mCityFieldIdx(-1),
mCarrierFieldIdx(-1),
mDayFieldIdx(-1),
mIsFirstKey(true),
mNeedMove(true),
mCrtDay(0)
{
}


AosDataJoinNetStatus::~AosDataJoinNetStatus()
{
}


bool
AosDataJoinNetStatus::config(
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

	mProcAll = def->getAttrBool(AOSTAG_PROCALL, false);
	
	OmnString proc_datetime = task->getTaskENV(AOSTAG_PROC_DATETIME, rdata);
	aos_assert_r(proc_datetime != "", false);
	
	mEpochDay = AosTimeFormat::strToEpochDay(proc_datetime, AosTimeFormat::eYYYYMMDD);  
	aos_assert_r(mEpochDay > 0, false);

	mKeyFieldIdx = def->getAttrInt(AOSTAG_KEYFIELDIDX, -1);
	aos_assert_r(mKeyFieldIdx >= 0, false);

	mCityFieldIdx = def->getAttrInt(AOSTAG_CITY_FIELD_IDX, -1);
	aos_assert_r(mCityFieldIdx >= 0, false);

	mCarrierFieldIdx = def->getAttrInt(AOSTAG_CARRIER_FIELD_IDX, -1);
	aos_assert_r(mCarrierFieldIdx >= 0, false);

	mDayFieldIdx = def->getAttrInt(AOSTAG_EPOCH_DAY_IDX, -1);
	aos_assert_r(mDayFieldIdx >= 0, false);

	mFieldSep = def->getAttrStr(AOSTAG_FIELD_SEP);
	aos_assert_r(mFieldSep != "", false);
	AosConvertAsciiBinary(mFieldSep);
	
	mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	aos_assert_r(mSep != "", false);
	AosConvertAsciiBinary(mSep);

	mKeyHead = AosCounterUtil::composeTimeEntryType(
		2, 0, AosStatType::toEnum("sum"), AosTimeGran::toEnum("day"));

	mCityHead = AosCounterUtil::composeTimeEntryType(
		3, 1, AosStatType::toEnum("sum"), AosTimeGran::toEnum("day"));

	// Retrieve IIL Scanners
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	aos_assert_r(scanners, false);
	
	AosXmlTagPtr scanner = scanners->getFirstChild(AOSTAG_IIL_SCANNER);
	aos_assert_r(scanner, false);
	
	AosIILScannerListenerPtr thisptr(this, false);	
	mIILScanner = AosIILScannerObj::createIILScannerStatic(thisptr, 0, scanner, rdata);
	aos_assert_r(mIILScanner, false);

	mInNetIILAsm = createIILAsm(def->getFirstChild(AOSTAG_INNET_IILASM), task, rdata);
	aos_assert_r(mInNetIILAsm, false);

	mOnNetIILAsm = createIILAsm(def->getFirstChild(AOSTAG_ONNET_IILASM), task, rdata);
	aos_assert_r(mOnNetIILAsm, false);

	mOutNetIILAsm = createIILAsm(def->getFirstChild(AOSTAG_OUTNET_IILASM), task, rdata);
	aos_assert_r(mOutNetIILAsm, false);

	mInNetStIILAsm = createIILAsm(def->getFirstChild(AOSTAG_INNET_ST_IILASM), task, rdata);
	aos_assert_r(mInNetStIILAsm, false);

	mOnNetStIILAsm = createIILAsm(def->getFirstChild(AOSTAG_ONNET_ST_IILASM), task, rdata);
	aos_assert_r(mOnNetStIILAsm, false);

	mOutNetStIILAsm = createIILAsm(def->getFirstChild(AOSTAG_OUTNET_ST_IILASM), task, rdata);
	aos_assert_r(mOutNetStIILAsm, false);

	if(!mProcAll)
	{
		AosXmlTagPtr iil_asb = def->getFirstChild(AOSTAG_ONNET_IILASM);
		aos_assert_r(iil_asb, false);

		OmnString dataColId = iil_asb->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
		aos_assert_r(dataColId != "", false);

		AosXmlTagPtr dataColTag = task->getDataColTag(dataColId, rdata);
		aos_assert_r(dataColTag, false);

		AosXmlTagPtr iil_tag = dataColTag->getFirstChild();
		aos_assert_r(iil_tag, false);
		
		OmnString iilname = iil_tag->getAttrStr(AOSTAG_IILNAME);
		aos_assert_r(iilname != "", false);

		OmnString cfg_str;
		cfg_str	<< "<iilscanner zky_iilname=\"" << iilname << "\" zky_matchtype=\"prefix\" zky_fieldsep=\"$\" "
				<< "zky_match_field_idx=\"1\" zky_ignorematcherr=\"false\" pagesize=\"100000\">"
				<< "<zky_valuecond zky_value_type=\"str\" zky_opr=\"prefix\">"
				<< "<zky_value1>" << mKeyHead << (mEpochDay - 1) << mSep << "</zky_value1>"
				<< "</zky_valuecond>"
				<< "<zky_selectors />"
				<< "</iilscanner>";
		
		AosXmlTagPtr cfg_xml = AosXmlParser::parse(cfg_str AosMemoryCheckerArgs);
		aos_assert_r(cfg_xml, false);

		mOnNetIILScanner = AosIILScannerObj::createIILScannerStatic(
			thisptr, 0, cfg_xml, rdata);
		aos_assert_r(mOnNetIILScanner, false);
	}

	return true;
}
	

bool
AosDataJoinNetStatus::run()
{
	// This task joins two or more IILs to create a new table. The
	// IILs are specified in mIILScanners. 
	// 1. Issue a request to copy the data for all the IILs in mIILScanners.
	// 2. It waits until all responses are received.
	// 3. Once all responses are received, it starts the join process.
	//
	// Join Process:
	// 1. Every IIL has a cursor, which sets to the first entry.
	// 2. Starting from left, it retrieves the current 'Join Key', 
	//    passes it on to the next IIL with a 'Record To be Created'.
	//    If the next IIL accepts the value, it sets its value to 
	//    the record, and passes the 'Join Key' to the next IIL, 
	//    if any. 
	//
	//    If any IIL rejects the 'Join Key', the record is dropped, 
	//    and the control goes back to the first IIL. It moves the cursor
	//    to the next one, if any, and repeats the above process.
	//
	//    If all IILs accept the 'Join Key', the record is created and 
	//    inserted into the table.
	//
	// 3. During the above process, if any IIL consumed all its data but
	//    there are still remaining data to process, it will pause the process,
	//    fetch the data, and then resume the process.
	//
	// This join will creates at most one record for each 'Join Key' 
	// in the first IIL.

	// 1. Issue a request to retrieve data for every IIL in 'mIIL'. 
	aos_assert_r(mStatus == eIdle, false);
	mStartTime = OmnGetSecond();
	OmnScreen << "join iil start , time:" << mStartTime << endl;
	
	AosRundataPtr rdata = mRundata;
	mLock->lock();
	mStatus = eActive;
	bool rslt = sendStart(rdata);
	if(rslt)
	{
		rslt = runJoin(rdata);
		sendFinish(rdata);
	}
	mStatus = eFinished;
	mLock->unlock();
	mFinished = true;
	mSuccess = rslt;
	
	mEndTime = OmnGetSecond();
	OmnScreen << "join iil finished, time:" << mEndTime
		<< ", spend:" << (mEndTime - mStartTime) << endl;
	
	AosDataJoinPtr thisptr(this, true);
	mCtlr->joinFinished(thisptr, rdata);

	clear();
	return true;
}


bool
AosDataJoinNetStatus::sendStart(const AosRundataPtr &rdata)
{
	aos_assert_r(mInNetIILAsm, false);
	aos_assert_r(mOnNetIILAsm, false);
	aos_assert_r(mOutNetIILAsm, false);
	aos_assert_r(mInNetStIILAsm, false);
	aos_assert_r(mOnNetStIILAsm, false);
	aos_assert_r(mOutNetStIILAsm, false);
	
	bool rslt = mInNetIILAsm->sendStart(rdata);
	aos_assert_r(rslt, false);

	rslt = mOnNetIILAsm->sendStart(rdata);
	aos_assert_r(rslt, false);

	rslt = mOutNetIILAsm->sendStart(rdata);
	aos_assert_r(rslt, false);
	
	rslt = mInNetStIILAsm->sendStart(rdata);
	aos_assert_r(rslt, false);

	rslt = mOnNetStIILAsm->sendStart(rdata);
	aos_assert_r(rslt, false);

	rslt = mOutNetStIILAsm->sendStart(rdata);
	aos_assert_r(rslt, false);
	
	return true;
}

	
bool
AosDataJoinNetStatus::sendFinish(const AosRundataPtr &rdata)
{
	aos_assert_r(mInNetIILAsm, false);
	aos_assert_r(mOnNetIILAsm, false);
	aos_assert_r(mOutNetIILAsm, false);
	aos_assert_r(mInNetStIILAsm, false);
	aos_assert_r(mOnNetStIILAsm, false);
	aos_assert_r(mOutNetStIILAsm, false);
	
	bool rslt = mInNetIILAsm->sendFinish(rdata);
	aos_assert_r(rslt, false);

	rslt = mOnNetIILAsm->sendFinish(rdata);
	aos_assert_r(rslt, false);

	rslt = mOutNetIILAsm->sendFinish(rdata);
	aos_assert_r(rslt, false);
	
	rslt = mInNetStIILAsm->sendFinish(rdata);
	aos_assert_r(rslt, false);

	rslt = mOnNetStIILAsm->sendFinish(rdata);
	aos_assert_r(rslt, false);

	rslt = mOutNetStIILAsm->sendFinish(rdata);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosDataJoinNetStatus::runJoin(const AosRundataPtr &rdata)
{
	bool rslt = mIILScanner->start(rdata);
	aos_assert_r(rslt, false);

	if(!mProcAll)
	{
		rslt = mOnNetIILScanner->start(rdata);
		aos_assert_r(rslt, false);
	}

	switch (mStatus)
	{
	case eIdle: mStatus = eActive;
	case eActive: break;
	case eFinished: return true;
	default: return false;
	}

	AosDataProcStatus::E status;
	OmnString key;
	bool has_more;
	while(1)
	{
		status = mIILScanner->moveNext(key, has_more, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk:
			 rslt = procNewKey(key, rdata);
			 aos_assert_r(rslt, false);
			 break;
		
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

	rslt = procEnd(rdata);
	aos_assert_r(rslt, false);

	return true;
}

	
bool
AosDataJoinNetStatus::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	return mIILScanner->setQueryContext(context, rdata);
}


int
AosDataJoinNetStatus::getProgress() 
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
AosDataJoinNetStatus::appendInNetEntry(
		const OmnString &key,
		const OmnString &city,
		const OmnString &carrier_id,
		const u64 &day,
		const AosRundataPtr &rdata)
{
	OmnString str((char *)&mKeyHead, sizeof(u16));
	str << day << mSep << key;
	AosValueRslt value_rslt;
	value_rslt.setKeyValue(str.data(), str.length(), true, 1);
	bool rslt = mInNetIILAsm->appendEntry(value_rslt, rdata);
	aos_assert_r(rslt, false);

	str.assign((char *)&mCityHead, sizeof(u16));
	str << sgAllProvince << mSep << day << mSep << carrier_id;
	value_rslt.setKeyValue(str.data(), str.length(), true, 1);
	rslt = mInNetStIILAsm->appendEntry(value_rslt, rdata);
	aos_assert_r(rslt, false);

	str.assign((char *)&mCityHead, sizeof(u16));
	str << city << mSep << day << mSep << carrier_id;
	value_rslt.setKeyValue(str.data(), str.length(), true, 1);
	rslt = mInNetStIILAsm->appendEntry(value_rslt, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataJoinNetStatus::appendOnNetEntry(
		const OmnString &key,
		const OmnString &city,
		const OmnString &carrier_id,
		const u64 &day,
		const AosRundataPtr &rdata)
{
	if (!sgAddOnNetFlag) return true;

	OmnString str((char *)&mKeyHead, sizeof(u16));
	str << day << mSep << key;
	AosValueRslt value_rslt;
	value_rslt.setKeyValue(str.data(), str.length(), true, 1);
	bool rslt = mOnNetIILAsm->appendEntry(value_rslt, rdata);
	aos_assert_r(rslt, false);

	str.assign((char *)&mCityHead, sizeof(u16));
	str << sgAllProvince << mSep << day << mSep << carrier_id;
	value_rslt.setKeyValue(str.data(), str.length(), true, 1);
	rslt = mOnNetStIILAsm->appendEntry(value_rslt, rdata);
	aos_assert_r(rslt, false);

	str.assign((char *)&mCityHead, sizeof(u16));
	str << city << mSep << day << mSep << carrier_id;
	value_rslt.setKeyValue(str.data(), str.length(), true, 1);
	rslt = mOnNetStIILAsm->appendEntry(value_rslt, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataJoinNetStatus::appendOutNetEntry(
		const OmnString &key,
		const OmnString &city,
		const OmnString &carrier_id,
		const u64 &day,
		const AosRundataPtr &rdata)
{
	if (day > mEpochDay)
	{
		OmnAlarm << "time is error, mEpochDay:" << mEpochDay << ", day:" << day << enderr;
		return true;
	}

	OmnString str((char *)&mKeyHead, sizeof(u16));
	str << day << mSep << key;
	AosValueRslt value_rslt;
	value_rslt.setKeyValue(str.data(), str.length(), true, 1);
	bool rslt = mOutNetIILAsm->appendEntry(value_rslt, rdata);
	aos_assert_r(rslt, false);

	str.assign((char *)&mCityHead, sizeof(u16));
	str << sgAllProvince << mSep << day << mSep << carrier_id;
	value_rslt.setKeyValue(str.data(), str.length(), true, 1);
	rslt = mOutNetStIILAsm->appendEntry(value_rslt, rdata);
	aos_assert_r(rslt, false);

	str.assign((char *)&mCityHead, sizeof(u16));
	str << city << mSep << day << mSep << carrier_id;
	value_rslt.setKeyValue(str.data(), str.length(), true, 1);
	rslt = mOutNetStIILAsm->appendEntry(value_rslt, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataJoinNetStatus::procNewKey(
		const OmnString &key,
		const AosRundataPtr &rdata)
{
	aos_assert_r(key != "", false);
	
	const char * key_data = 0;
	int key_len = -1;
	bool rslt = AosGetField(key_data, key_len, key.data(),
		key.length(), mKeyFieldIdx, mFieldSep, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(key_data && key_len > 0, false);
	OmnString kk(key_data, key_len);

	const char * city_data = 0;
	int city_len = -1;
	rslt = AosGetField(city_data, city_len, key.data(),
		key.length(), mCityFieldIdx, mFieldSep, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(city_data && city_len > 0, false);
	OmnString city(city_data, city_len);

	const char * carrier_data = 0;
	int carrier_len = -1;
	rslt = AosGetField(carrier_data, carrier_len, key.data(),
		key.length(), mCarrierFieldIdx, mFieldSep, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(carrier_data && carrier_len > 0, false);
	OmnString carrier_id(carrier_data, carrier_len);

	const char * day_data = 0;
	int day_len = -1;
	rslt = AosGetField(day_data, day_len, key.data(),
		key.length(), mDayFieldIdx, mFieldSep, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(day_data && day_len > 0, false);
	OmnString day(day_data, day_len);
	u64 dd = atoll(day.data());

	if (dd > mEpochDay) return true;

	if (mProcAll)
	{
		return procKeyAll(kk, city, carrier_id, dd, rdata);
	}
	return procKeyDay(kk, city, carrier_id, dd, rdata);
}


bool
AosDataJoinNetStatus::procKeyAll(
		const OmnString &key,
		const OmnString &city,
		const OmnString &carrier_id,
		const u64 &day,
		const AosRundataPtr &rdata)
{
	bool rslt;
	if (mIsFirstKey)
	{
		mIsFirstKey = false;
		mCrtKey = key;
		mCrtCity = city;
		mCrtCarrierId = carrier_id;
		mCrtDay = day;
		
		rslt = appendInNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
		aos_assert_r(rslt, false);
	
		if (sgAddOnNetFlag)
		{
			rslt = appendOnNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
			aos_assert_r(rslt, false);
		}
		return true;
	}
	
	if (key != mCrtKey)
	{
		if ((mEpochDay - mCrtDay) >= sgDayInterval)
		{
			if (sgAddOnNetFlag)
			{
				for(u64 i = 1; i < sgDayInterval; i++)
				{
					rslt = appendOnNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay + i, rdata);
					aos_assert_r(rslt, false);
				}
			}	

			rslt = appendOutNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay + sgDayInterval, rdata);
			aos_assert_r(rslt, false);
		}
		else
		{
			if (sgAddOnNetFlag)
			{
				for(u64 i = 1; i <= mEpochDay - mCrtDay; i++)
				{
					rslt = appendOnNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay + i, rdata);
					aos_assert_r(rslt, false);
				}
			}
		}

		mCrtKey = key;
		mCrtCity = city;
		mCrtCarrierId = carrier_id;
		mCrtDay = day;
			
		rslt = appendInNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
		aos_assert_r(rslt, false);
	
		if (sgAddOnNetFlag)
		{
			rslt = appendOnNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
			aos_assert_r(rslt, false);
		}

		return true;
	}

	if ((day - mCrtDay) > sgDayInterval)
	{
		if (sgAddOnNetFlag)
		{
			for(u64 i = 1; i < sgDayInterval; i++)
			{
				rslt = appendOnNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay + i, rdata);
				aos_assert_r(rslt, false);
			}
		}
		
		rslt = appendOutNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay + sgDayInterval, rdata);
		aos_assert_r(rslt, false);
		
		rslt = appendInNetEntry(key, city, carrier_id, day, rdata);
		aos_assert_r(rslt, false);
	}
	else
	{
		if (sgAddOnNetFlag)
		{
			for(u64 i = 1; i < day - mCrtDay; i++)
			{
				rslt = appendOnNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay + i, rdata);
				aos_assert_r(rslt, false);
			}
		}
	}
	
	mCrtKey = key;
	mCrtCity = city;
	mCrtCarrierId = carrier_id;
	mCrtDay = day;

	if (sgAddOnNetFlag)
	{
		rslt = appendOnNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
		aos_assert_r(rslt, false);
	}

	return true;	
}

	
bool
AosDataJoinNetStatus::procNextOnNetKey(
		const OmnString &key,
		const bool procToEnd,
		const AosRundataPtr &rdata)
{
	AosDataProcStatus::E status;
	const char * vv;
	int len;
	bool rslt;
	while(1)
	{
		if(mNeedMove)
		{
			status = mOnNetIILScanner->moveNext(vv, len, -1, rdata);
		}
		else
		{
			status = AosDataProcStatus::eOk;
		}
		mNeedMove = true;
		switch (status)
		{
		case AosDataProcStatus::eOk:
			 {
				 OmnString key_2(vv, len);
			 	 if(key_2 < key)
			 	 {
					if (sgAddOnNetFlag)
					{
						rslt = appendOnNetEntry(key_2, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
					 	aos_assert_r(rslt, false);
					}
				 }
				 else if(key_2 > key)
				 {
					if(procToEnd)
					{
						if (sgAddOnNetFlag)
						{
							rslt = appendOnNetEntry(key_2, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
							aos_assert_r(rslt, false);
						}
					}
					else
					{
						mNeedMove = false;
					}
					return true;
				 }
			 }
			 break;
		
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
	
	return true;
}


bool
AosDataJoinNetStatus::procKeyDay(
		const OmnString &key,
		const OmnString &city,
		const OmnString &carrier_id,
		const u64 &day,
		const AosRundataPtr &rdata)
{
	bool rslt;
	if (mIsFirstKey)
	{
		rslt = procNextOnNetKey(mCrtKey, false, rdata);
		aos_assert_r(rslt, false);
		
		mIsFirstKey = false;
		mCrtKey = key;
		mCrtCity = city;
		mCrtCarrierId = carrier_id;
		mCrtDay = day;

		if (day == mEpochDay)
		{
			rslt = appendInNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
			aos_assert_r(rslt, false);
	
			if (sgAddOnNetFlag)
			{
				rslt = appendOnNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
				aos_assert_r(rslt, false);
			}
		}
		return true;
	}
	
	if (key != mCrtKey)
	{
		rslt = procNextOnNetKey(mCrtKey, false, rdata);
		aos_assert_r(rslt, false);
		
		if ((mEpochDay - mCrtDay) == sgDayInterval)
		{
			rslt = appendOutNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
			aos_assert_r(rslt, false);
		}

		mCrtKey = key;
		mCrtCity = city;
		mCrtCarrierId = carrier_id;
		mCrtDay = day;

		if (day == mEpochDay)
		{
			rslt = appendInNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
			aos_assert_r(rslt, false);
	
			if (sgAddOnNetFlag)
			{
				rslt = appendOnNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
				aos_assert_r(rslt, false);
			}
		}
		return true;
	}

	if ((day - mCrtDay) > sgDayInterval && day == mEpochDay)
	{
		rslt = appendOutNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay + sgDayInterval, rdata);
		aos_assert_r(rslt, false);
		
		rslt = appendInNetEntry(key, city, carrier_id, day, rdata);
		aos_assert_r(rslt, false);
	}
	
	mCrtKey = key;
	mCrtCity = city;
	mCrtCarrierId = carrier_id;
	mCrtDay = day;
	return true;	
}


bool
AosDataJoinNetStatus::procEnd(const AosRundataPtr &rdata)
{
	bool rslt;
	if(mProcAll)
	{
		if ((mEpochDay - mCrtDay) >= sgDayInterval)
		{
			if (sgAddOnNetFlag)
			{
				for(u64 i = 1; i < sgDayInterval; i++)
				{
					rslt = appendOnNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay + i, rdata);
					aos_assert_r(rslt, false);
				}
			}
			
			rslt = appendOutNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay + sgDayInterval, rdata);
			aos_assert_r(rslt, false);
		}
		else
		{
			if (sgAddOnNetFlag)
			{
				for(u64 i = 1; i <= mEpochDay - mCrtDay; i++)
				{
					rslt = appendOnNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay + i, rdata);
					aos_assert_r(rslt, false);
				}
			}
		}
		
		return true;
	}

	if ((mEpochDay - mCrtDay) == sgDayInterval)
	{
		rslt = appendOutNetEntry(mCrtKey, mCrtCity, mCrtCarrierId, mCrtDay, rdata);
		aos_assert_r(rslt, false);
	}
	
	rslt = procNextOnNetKey(mCrtKey, true, rdata);
	aos_assert_r(rslt, false);

	return true;
}

	
void
AosDataJoinNetStatus::clear()
{
	mCtlr = 0;
	mIILScanner->clear();
	mIILScanner = 0;
	mInNetIILAsm = 0;
	mOnNetIILAsm = 0;
	mOutNetIILAsm = 0;
	mOnNetIILScanner = 0;
}


AosDataJoinPtr
AosDataJoinNetStatus::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinNetStatus * join = OmnNew AosDataJoinNetStatus(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinNetStatus::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	OmnString proc_datetime = task->getTaskENV(AOSTAG_PROC_DATETIME, rdata);
	aos_assert_r(proc_datetime != "", false);
	
	u64 epochDay = AosTimeFormat::strToEpochDay(proc_datetime, AosTimeFormat::eYYYYMMDD);  
	aos_assert_r(epochDay > 0, false);

	int keyFieldIdx = def->getAttrInt(AOSTAG_KEYFIELDIDX, -1);
	aos_assert_r(keyFieldIdx >= 0, false);
	
	int cityFieldIdx = def->getAttrInt(AOSTAG_CITY_FIELD_IDX, -1);
	aos_assert_r(cityFieldIdx >= 0, false);

	int carrierFieldIdx = def->getAttrInt(AOSTAG_CARRIER_FIELD_IDX, -1);
	aos_assert_r(carrierFieldIdx >= 0, false);

	int dayFieldIdx = def->getAttrInt(AOSTAG_EPOCH_DAY_IDX, -1);
	aos_assert_r(dayFieldIdx >= 0, false);
	
	OmnString fieldSep = def->getAttrStr(AOSTAG_FIELD_SEP);
	aos_assert_r(fieldSep != "", false);

	OmnString sep = def->getAttrStr(AOSTAG_SEPARATOR);
	aos_assert_r(sep != "", false);

	// Retrieve IIL Scanners
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	aos_assert_r(scanners, false);
	
	AosXmlTagPtr scanner = scanners->getFirstChild(AOSTAG_IIL_SCANNER);
	aos_assert_r(scanner, false);
	
	AosIILScannerObjPtr iilscanner = AosIILScannerObj::createIILScannerStatic(
		0, 0, scanner, rdata);
	aos_assert_r(iilscanner, false);
	
	AosDataAssemblerObjPtr assm = createIILAsm(def->getFirstChild(AOSTAG_INNET_IILASM), task, rdata);
	aos_assert_r(assm, false);

	assm = createIILAsm(def->getFirstChild(AOSTAG_ONNET_IILASM), task, rdata);
	aos_assert_r(assm, false);

	assm = createIILAsm(def->getFirstChild(AOSTAG_OUTNET_IILASM), task, rdata);
	aos_assert_r(assm, false);

	assm = createIILAsm(def->getFirstChild(AOSTAG_INNET_ST_IILASM), task, rdata);
	aos_assert_r(assm, false);

	assm = createIILAsm(def->getFirstChild(AOSTAG_ONNET_ST_IILASM), task, rdata);
	aos_assert_r(assm, false);

	assm = createIILAsm(def->getFirstChild(AOSTAG_OUTNET_ST_IILASM), task, rdata);
	aos_assert_r(assm, false);

	return true;
}

#endif
