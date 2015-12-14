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
#include "DataJoin/DataJoinCpnTownCode.h"


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


AosDataJoinCpnTownCode::AosDataJoinCpnTownCode()
:
AosDataJoin(),
mHaveStatisticHead(true),
mCrtCDRsNum(0),
mCrtTownCode(0)
{
}


AosDataJoinCpnTownCode::AosDataJoinCpnTownCode(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata),
mHaveStatisticHead(true),
mCrtCDRsNum(0),
mCrtTownCode(0)
{
}


AosDataJoinCpnTownCode::~AosDataJoinCpnTownCode()
{
}


bool
AosDataJoinCpnTownCode::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	
	mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	aos_assert_r(mSep != "", false);
	AosConvertAsciiBinary(mSep);
	
	mKeyHead = AosCounterUtil::composeTimeEntryType(
		1, 0, AosStatType::toEnum("sum"), AosTimeGran::toEnum("ntm"));

	OmnString cpn_citycode_iilname = def->getAttrStr("cpn_citycode_iilname");
	aos_assert_r(cpn_citycode_iilname != "", false);
	mCpnCityCodeMap = AosIILEntryMapMgr::retrieveIILEntryMap(cpn_citycode_iilname, rdata);
	aos_assert_r(mCpnCityCodeMap, false);

	mCpnCityCodeMap->lock();
	OmnScreen << "cpn city map size:" << mCpnCityCodeMap->size() << endl;
	mCpnCityCodeMap->unlock();

	OmnString upn_citycode_iilname = def->getAttrStr("upn_citycode_iilname");
	aos_assert_r(upn_citycode_iilname != "", false);
	mUpnCityCodeMap = AosIILEntryMapMgr::retrieveIILEntryMap(upn_citycode_iilname, rdata);
	aos_assert_r(mUpnCityCodeMap, false);

	mUpnCityCodeMap->lock();
	OmnScreen << "upn city map size:" << mUpnCityCodeMap->size() << endl;
	mUpnCityCodeMap->unlock();

	OmnString upn_towncode_iilname = def->getAttrStr("upn_towncode_iilname");
	aos_assert_r(upn_towncode_iilname != "", false);
	mUpnTownCodeMap = AosIILEntryMapMgr::retrieveIILEntryMap(upn_towncode_iilname, rdata);
	aos_assert_r(mUpnTownCodeMap, false);

	mUpnTownCodeMap->lock();
	OmnScreen << "upn town map size:" << mUpnTownCodeMap->size() << endl;
	mUpnTownCodeMap->unlock();

	// Retrieve IIL Scanners
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	aos_assert_r(scanners, false);
	
	AosXmlTagPtr scanner = scanners->getFirstChild(AOSTAG_IIL_SCANNER);
	aos_assert_r(scanner, false);
	
	AosIILScannerListenerPtr thisptr(this, false);	
	mIILScanner = AosIILScannerObj::createIILScannerStatic(
		thisptr, 0, scanner, rdata);
	aos_assert_r(mIILScanner, false);

	mIILAssembler = mCtlr->getIILAssembler();
	aos_assert_r(mIILAssembler, false);

	return true;
}


bool
AosDataJoinCpnTownCode::run()
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
	bool rslt = runJoin(rdata.getPtrNoLock());
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
AosDataJoinCpnTownCode::runJoin(AosRundata *rdata)
{
	bool rslt = mIILScanner->start(rdata);
	aos_assert_r(rslt, false);

	u64 value;
	bool finish = false;
	OmnString cpn, upn;

	while (!finish)
	{
		rslt = getNextKey(cpn, upn, value, finish, rdata);
		if (finish) break;
		aos_assert_r(rslt, false);

		rslt = procNextKey(cpn, upn, value, rdata);
		aos_assert_r(rslt, false);
	}

	rslt = procEnd(rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataJoinCpnTownCode::getNextKey(
		OmnString &cpn,
		OmnString &upn,
		u64 &value,
		bool &finish,
		AosRundata *rdata)
{
	cpn = "";
	upn = "";
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

			 cpn = str[0];
			 upn = str[1];
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
AosDataJoinCpnTownCode::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	return mIILScanner->setQueryContext(context, rdata);
}


int
AosDataJoinCpnTownCode::getProgress() 
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
	

int
AosDataJoinCpnTownCode::getCpnCityCode(
		const OmnString &cpn,
		AosRundata *rdata)
{
	if (!AosIsCellPhoneNum(cpn))
	{
		OmnAlarm << "cpn:" << cpn << enderr;
		return -1;
	}

	int city_code = -1;
	//mCpnCityCodeMap->lock();

	OmnString prefix = cpn;
	prefix.setLength(7);	
	AosIILEntryMapItr itr = mCpnCityCodeMap->find(prefix);
	if (itr != mCpnCityCodeMap->end())
	{
		OmnString city_code_str = itr->second;
		city_code = atoi(city_code_str.data());
	}
	else
	{
		prefix = cpn;
		prefix.setLength(8);
		itr = mCpnCityCodeMap->find(prefix);
		if (itr != mCpnCityCodeMap->end())
		{
			OmnString city_code_str = itr->second;
			city_code = atoi(city_code_str.data());
		}
	}

	//mCpnCityCodeMap->unlock();
	return city_code;
}


int
AosDataJoinCpnTownCode::getUpnCityCode(
		const OmnString &upn,
		AosRundata *rdata)
{
	aos_assert_r(upn != "", -1);

	int city_code = -1;
	//mUpnCityCodeMap->lock();
	AosIILEntryMapItr itr = mUpnCityCodeMap->find(upn);
	if (itr != mUpnCityCodeMap->end())
	{
		OmnString city_code_str = itr->second;
		city_code = atoi(city_code_str.data());
	}
	//mUpnCityCodeMap->unlock();

	return city_code;
}


int
AosDataJoinCpnTownCode::getUpnTownCode(
		const OmnString &upn,
		AosRundata *rdata)
{
	aos_assert_r(upn != "", -1);
	
	int town_code = -1;
	//mUpnTownCodeMap->lock();
	AosIILEntryMapItr itr = mUpnTownCodeMap->find(upn);
	if (itr != mUpnTownCodeMap->end())
	{
		OmnString town_code_str = itr->second;
		town_code = atoi(town_code_str.data());
	}
	//mUpnTownCodeMap->unlock();
	
	return town_code;
}


bool
AosDataJoinCpnTownCode::appendEntry(
		const OmnString &cpn,
		const u64 &town_code,
		AosRundata *rdata_raw)
{
	OmnString str((char *)&mKeyHead, sizeof(u16));
	str << cpn;
	
	AosValueRslt value_rslt;
	value_rslt.setKeyValue(str.data(), str.length(), true, town_code);
	return mIILAssembler->appendEntry(value_rslt, rdata_raw);
}

	
bool
AosDataJoinCpnTownCode::procEnd(AosRundata *rdata)
{
	return appendEntry(mCrtCpn, mCrtTownCode, rdata);
}


bool
AosDataJoinCpnTownCode::procNextKey(
		const OmnString &cpn,
		const OmnString &upn,
		const u64 &value,
		AosRundata *rdata)
{
	int cpn_citycode = getCpnCityCode(cpn, rdata);
	int upn_citycode = getUpnCityCode(upn, rdata);
	if (cpn_citycode < 0 || upn_citycode < 0 || cpn_citycode != upn_citycode)
	{
		return true;
	}

	if (cpn != mCrtCpn || mCrtCpn == "")
	{
		int town_code = getUpnTownCode(upn, rdata);
		if (town_code < 0)
		{
			return true;
		}

		if (mCrtCpn != "")
		{
			appendEntry(mCrtCpn, mCrtTownCode, rdata);
		}
	
		mCrtCpn = cpn;
		mCrtCDRsNum = value;
		mCrtTownCode = (u64)town_code;
		return true;
	}
	else
	{
		if(value > mCrtCDRsNum)
		{
			int town_code = getUpnTownCode(upn, rdata);
			if (town_code < 0)
			{
				return true;
			}
			
			mCrtCDRsNum = value;
			mCrtTownCode = (u64)town_code;
		}
	}

	return true;
}


void
AosDataJoinCpnTownCode::clear()
{
	mCtlr = 0;
	mIILScanner->clear();
	mIILScanner = 0;
	mIILAssembler = 0;
	mCpnCityCodeMap = 0;
	mUpnCityCodeMap = 0;
	mUpnTownCodeMap = 0;
}


AosDataJoinPtr
AosDataJoinCpnTownCode::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinCpnTownCode * join = OmnNew AosDataJoinCpnTownCode(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinCpnTownCode::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	OmnString sep = def->getAttrStr(AOSTAG_SEPARATOR);
	aos_assert_r(sep != "", false);
	
	OmnString cpn_citycode_iilname = def->getAttrStr("cpn_citycode_iilname");
	aos_assert_r(cpn_citycode_iilname != "", false);

	OmnString upn_citycode_iilname = def->getAttrStr("upn_citycode_iilname");
	aos_assert_r(upn_citycode_iilname != "", false);

	OmnString upn_towncode_iilname = def->getAttrStr("upn_towncode_iilname");
	aos_assert_r(upn_towncode_iilname != "", false);

	// Retrieve IIL Scanners
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	aos_assert_r(scanners, false);
	
	AosXmlTagPtr scanner = scanners->getFirstChild(AOSTAG_IIL_SCANNER);
	aos_assert_r(scanner, false);
	
	AosIILScannerObjPtr iilscanner = AosIILScannerObj::createIILScannerStatic(
		0, 0, scanner, rdata);
	aos_assert_r(iilscanner, false);

	return true;
}

