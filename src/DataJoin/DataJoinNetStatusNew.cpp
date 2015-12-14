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
#include "DataJoin/DataJoinNetStatusNew.h"


#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterUtil/CounterUtil.h"
#include "DataJoin/DataJoinCtlr.h"
#include "DataRecord/DataRecord.h"
#include "DataAssembler/DataAssembler.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Util/OmnNew.h"
#include "UtilTime/TimeFormat.h"
#include "XmlUtil/XmlTag.h"


static u64 sgDayInterval = 90;
static OmnString sgAllProvince = "99";


AosDataJoinNetStatusNew::AosDataJoinNetStatusNew()
:
AosDataJoin(),
mCrtDay(0)
{
}


AosDataJoinNetStatusNew::AosDataJoinNetStatusNew(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata),
mCrtDay(0)
{
}


AosDataJoinNetStatusNew::~AosDataJoinNetStatusNew()
{
}


bool
AosDataJoinNetStatusNew::config(
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

	OmnString proc_start_day = task->getTaskENV(AOSTAG_PROC_START_DAY, rdata);
	aos_assert_r(proc_start_day != "", false);
	
	mFirstDay = AosTimeFormat::strToEpochDay(proc_start_day, AosTimeFormat::eYYYYMMDD);  
	aos_assert_r(mFirstDay > 0, false);

	mCrtDay = mFirstDay;

	OmnString proc_end_day = task->getTaskENV(AOSTAG_PROC_END_DAY, rdata);
	aos_assert_r(proc_end_day != "", false);

	mLastDay = AosTimeFormat::strToEpochDay(proc_end_day, AosTimeFormat::eYYYYMMDD);
	aos_assert_r(mLastDay > 0, false);
	aos_assert_r(mLastDay >= mFirstDay, false);
		
	mFieldSep = def->getAttrStr(AOSTAG_FIELD_SEP);
	aos_assert_r(mFieldSep != "", false);
	AosConvertAsciiBinary(mFieldSep);
	
	mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	aos_assert_r(mSep != "", false);
	AosConvertAsciiBinary(mSep);
	
	mNeedCity = def->getAttrBool("need_city", true);

	if (mNeedCity)
	{
		u16 key_head = AosCounterUtil::composeTimeEntryType(
			3, 1, AosStatType::toEnum("sum"), AosTimeGran::toEnum("day"));
		mKeyHead.assign((char *)&key_head, sizeof(u16));
	}
	else
	{
		u16 key_head = AosCounterUtil::composeTimeEntryType(
			2, 0, AosStatType::toEnum("sum"), AosTimeGran::toEnum("day"));
		mKeyHead.assign((char *)&key_head, sizeof(u16));
	}

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

	mInNetStIILAsm = createIILAsm(def->getFirstChild(AOSTAG_INNET_ST_IILASM), task, rdata);
	aos_assert_r(mInNetStIILAsm, false);

	mOutNetIILAsm = createIILAsm(def->getFirstChild(AOSTAG_OUTNET_IILASM), task, rdata);
	aos_assert_r(mOutNetIILAsm, false);

	mOutNetStIILAsm = createIILAsm(def->getFirstChild(AOSTAG_OUTNET_ST_IILASM), task, rdata);
	aos_assert_r(mOutNetStIILAsm, false);

	return true;
}
	

bool
AosDataJoinNetStatusNew::run()
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
	bool rslt = sendStart(rdata.getPtrNoLock());
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
AosDataJoinNetStatusNew::sendStart(const AosRundataPtr &rdata)
{
	aos_assert_r(mInNetIILAsm, false);
	aos_assert_r(mInNetStIILAsm, false);
	aos_assert_r(mOutNetIILAsm, false);
	aos_assert_r(mOutNetStIILAsm, false);
	
	bool rslt = mInNetIILAsm->sendStart(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	rslt = mInNetStIILAsm->sendStart(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	rslt = mOutNetIILAsm->sendStart(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);
	
	rslt = mOutNetStIILAsm->sendStart(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);
	
	return true;
}

	
bool
AosDataJoinNetStatusNew::sendFinish(const AosRundataPtr &rdata)
{
	aos_assert_r(mInNetIILAsm, false);
	aos_assert_r(mInNetStIILAsm, false);
	aos_assert_r(mOutNetIILAsm, false);
	aos_assert_r(mOutNetStIILAsm, false);
	
	bool rslt = mInNetIILAsm->sendFinish(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	rslt = mInNetStIILAsm->sendFinish(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	rslt = mOutNetIILAsm->sendFinish(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);
	
	rslt = mOutNetStIILAsm->sendFinish(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosDataJoinNetStatusNew::getNextKey(
		OmnString &key,
		u64 &day,
		bool &finish,
		const AosRundataPtr &rdata)
{
	key = "";
	day = 0;
	finish = false;

	bool f = false;
	bool has_more = true;
	vector<OmnString> str;
	AosDataProcStatus::E status;

	while(1)
	{
		status = mIILScanner->moveNext(key, has_more, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk:
			 str.clear();
			 AosStrSplit::splitStrBySubstr(key.data(),
				mFieldSep.data(), str, 100, f);

			 if (mNeedCity)
			 {
			 	aos_assert_r(str.size() == 4, false);

			 	//if (!AosIsCellPhoneNum(str[1])) break;

			 	day = str[0].toU64();
			 	key = "";
			 	key << str[1] << mFieldSep << str[2] << mFieldSep << str[3];
			 }
			 else
			 {
			 	aos_assert_r(str.size() == 2, false);

			 	//if (!AosIsCellPhoneNum(str[1])) break;

			 	day = str[0].toU64();
			 	key = str[1];
			 }
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
AosDataJoinNetStatusNew::createMap(
		const u64 &crt_day,
		map<OmnString, int> &crt_map,
		const AosRundataPtr &rdata)
{
	crt_map.clear();

	AosQueryContextObjPtr context = mIILScanner->getQueryContext();
	aos_assert_r(context, false);

	AosQueryContextObjPtr con = context->clone();
	aos_assert_r(con, false);

	OmnString value1, value2;
	value1 << (crt_day + 1) << mFieldSep;
	value2 << (crt_day + sgDayInterval) << mFieldSep;
	con->setOpr(eAosOpr_range_ge_lt);
	con->setStrValue(value1);
	con->setStrValue2(value2);
	con->setFinished(false);
	con->setTotalDocInRslt(0);
	con->setCrtDocid(0);
	con->setCrtValue("");
	con->resetIILIndex2();

	bool rslt = mIILScanner->reloadData(con, rdata);
	aos_assert_r(rslt, false);

	u64 day;
	u64 dd = 0;
	OmnString key;
	bool finish = false;
	map<OmnString, int>::iterator itr;

	while (!finish)
	{
		rslt = getNextKey(key, day, finish, rdata);
		if (finish) break;
		if (!rslt || day < crt_day + 1 || day >= crt_day + sgDayInterval)
		{
			OmnAlarm << "error" << enderr;
			continue;
		}
		if (day != dd)
		{
			OmnScreen << "map crt day:" << day << endl;
			dd = day;
		}

		itr = crt_map.find(key);
		if (itr != crt_map.end())
		{
			itr->second++;
		}
		else
		{
			crt_map[key] = 1;
		}
	}

	return true;
}


bool
AosDataJoinNetStatusNew::createSet(
		const u64 &crt_day,
		set<OmnString> &crt_set,
		const AosRundataPtr &rdata)
{
	crt_set.clear();

	AosQueryContextObjPtr context = mIILScanner->getQueryContext();
	aos_assert_r(context, false);

	AosQueryContextObjPtr con = context->clone();
	aos_assert_r(con, false);

	OmnString value;
	value << crt_day << mFieldSep;
	con->setOpr(eAosOpr_prefix);
	con->setStrValue(value);
	con->setFinished(false);
	con->setTotalDocInRslt(0);
	con->setCrtDocid(0);
	con->setCrtValue("");
	con->resetIILIndex2();

	bool rslt = mIILScanner->reloadData(con, rdata);
	aos_assert_r(rslt, false);

	u64 day;
	OmnString key;
	bool finish = false;

	while (!finish)
	{
		rslt = getNextKey(key, day, finish, rdata);
		if (finish) break;
		if (!rslt || day != crt_day)
		{
			OmnAlarm << "error" << enderr;
			continue;
		}

		crt_set.insert(key);
	}

	return true;
}


bool
AosDataJoinNetStatusNew::addSet(
		map<OmnString, int> &crt_map,
		set<OmnString> &crt_set,
		const AosRundataPtr &rdata)
{
	OmnString key;
	set<OmnString>::iterator set_itr;
	map<OmnString, int>::iterator map_itr;
	for (set_itr = crt_set.begin(); set_itr != crt_set.end(); set_itr++)
	{
		key = *set_itr;

		map_itr = crt_map.find(key);
		if (map_itr != crt_map.end())
		{
			map_itr->second++;
		}
		else
		{
			crt_map[key] = 1;
		}
	}
	
	return true;
}


bool
AosDataJoinNetStatusNew::removeSet(
		map<OmnString, int> &crt_map,
		set<OmnString> &crt_set,
		const AosRundataPtr &rdata)
{
	OmnString key;
	set<OmnString>::iterator set_itr;
	map<OmnString, int>::iterator map_itr;
	for (set_itr = crt_set.begin(); set_itr != crt_set.end(); set_itr++)
	{
		key = *set_itr;

		map_itr = crt_map.find(key);
		if (map_itr == crt_map.end())
		{
			OmnAlarm << "missing find key:" << key << ", crt_day" << mCrtDay << enderr;
			return false;
		}
		
		map_itr->second--;
		if (map_itr->second <= 0)
		{
			crt_map.erase(map_itr);
		}
	}
	
	return true;

}


bool
AosDataJoinNetStatusNew::runJoin(const AosRundataPtr &rdata)
{
	bool rslt = mIILScanner->start(rdata);
	aos_assert_r(rslt, false);

	OmnScreen << "mCrtDay:" << mCrtDay << endl;

	OmnString key;
	map<OmnString, int> crt_map;
	map<OmnString, int>::iterator map_itr;
	set<OmnString> begin_set, end_set;
	set<OmnString>::iterator set_itr;

	OmnScreen << "create begin set start:" << (mCrtDay - sgDayInterval) << endl;
	rslt = createSet(mCrtDay - sgDayInterval, begin_set, rdata);
	aos_assert_r(rslt, false);
	OmnScreen << "create begin set success:" << (mCrtDay - sgDayInterval) << ",set_size:" << (begin_set.size()) << endl;

	OmnScreen << "create crt map start:" << (mCrtDay - sgDayInterval) << endl;
	rslt = createMap(mCrtDay - sgDayInterval, crt_map, rdata);
	aos_assert_r(rslt, false);
	OmnScreen << "create crt map success:" << (mCrtDay - sgDayInterval) << ",map_size:" << (crt_map.size()) << endl;

	OmnScreen << "create end set start:" << mCrtDay << endl;
	rslt = createSet(mCrtDay, end_set, rdata);
	aos_assert_r(rslt, false);
	OmnScreen << "create end set success:" << mCrtDay << ",set_size:" << (end_set.size()) << endl;

	while (mCrtDay <= mLastDay)
	{
		for (set_itr = begin_set.begin(); set_itr != begin_set.end(); set_itr++)
		{
			key = *set_itr;
			if (crt_map.count(key) <= 0 && end_set.count(key) <= 0)
			{
				rslt = appendOutNetEntry(key, mCrtDay, rdata);
				aos_assert_r(rslt, false);
			}
		}

		for (set_itr = end_set.begin(); set_itr != end_set.end(); set_itr++)
		{
			key = *set_itr;
			if (crt_map.count(key) <= 0 && begin_set.count(key) <= 0)
			{
				rslt = appendInNetEntry(key, mCrtDay, rdata);
				aos_assert_r(rslt, false);
			}
		}

		mCrtDay++;
		if (mCrtDay > mLastDay)
		{
			break;
		}
		OmnScreen << "mCrtDay:" << mCrtDay << endl;

		OmnScreen << "create begin set start:" << (mCrtDay - sgDayInterval) << endl;
		rslt = createSet(mCrtDay - sgDayInterval, begin_set, rdata);
		aos_assert_r(rslt, false);
		OmnScreen << "create begin set success:" << (mCrtDay - sgDayInterval) << ",set_size:" << (begin_set.size()) << endl;

		OmnScreen << "remove begin set start" << endl;
		rslt = removeSet(crt_map, begin_set, rdata);
		aos_assert_r(rslt, false);
		OmnScreen << "remove begin set success" << endl;

		OmnScreen << "add end set start" << endl;
		rslt = addSet(crt_map, end_set, rdata);
		aos_assert_r(rslt, false);
		OmnScreen << "add end set success" << endl;

		OmnScreen << "create end set start:" << mCrtDay << endl;
		rslt = createSet(mCrtDay, end_set, rdata);
		aos_assert_r(rslt, false);
		OmnScreen << "create end set success:" << mCrtDay << ",set_size:" << (end_set.size()) << endl;
	}

	if (mNeedCity)
	{ 
		AosValueRslt value_rslt;
		map<OmnString, int>::iterator itr = mInNetStMap.begin();
		while (itr != mInNetStMap.end())
		{
			key = itr->first;
			value_rslt.setKeyValue(key.data(), key.length(), true, itr->second);
			rslt = mInNetStIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);
			itr++;
		}

		itr = mOutNetStMap.begin();
		while (itr != mOutNetStMap.end())
		{
			key = itr->first;
			value_rslt.setKeyValue(key.data(), key.length(), true, itr->second);
			rslt = mOutNetStIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);
			itr++;
		}
	}

	return true;
}

	
bool
AosDataJoinNetStatusNew::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	return mIILScanner->setQueryContext(context, rdata);
}


int
AosDataJoinNetStatusNew::getProgress() 
{
	if(mFinished)
	{
		mProgress = 100;
		return mProgress;
	}
	
	aos_assert_r(mIILScanner, 0);

	mProgress = (mCrtDay - mFirstDay) * 100 / (mLastDay - mFirstDay + 1);
	if (mProgress < 0) return 0;

	//OmnScreen << "join:[" << this << "], mProgress:" << mProgress;
	return mProgress;
}


bool
AosDataJoinNetStatusNew::appendInNetEntry(
		const OmnString &key,
		const u64 &day,
		const AosRundataPtr &rdata)
{
	bool finished = false;
	vector<OmnString> strs;
	AosStrSplit::splitStrBySubstr(key.data(),
		mFieldSep.data(), strs, 100, finished);

	bool rslt;
	OmnString str;
	AosValueRslt value_rslt;
	map<OmnString, int>::iterator itr;

	if (mNeedCity)
	{
		aos_assert_r(strs.size() == 3, false);

		OmnString phonenum = strs[0];
		OmnString city = strs[1];
		OmnString carrier_id = strs[2];

		str = mKeyHead;
		str << sgAllProvince << mSep << day << mSep << phonenum;
		value_rslt.setKeyValue(str.data(), str.length(), true, 1);
		rslt = mInNetIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);

		str = mKeyHead;
		str << city << mSep << day << mSep << phonenum;
		value_rslt.setKeyValue(str.data(), str.length(), true, 1);
		rslt = mInNetIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);

		str = mKeyHead;
		str << sgAllProvince << mSep << day << mSep << carrier_id;
		itr = mInNetStMap.find(str);
		if (itr == mInNetStMap.end())
		{
			mInNetStMap[str] = 1;		
		}
		else
		{
			itr->second += 1;
		}

		str = mKeyHead;
		str << city << mSep << day << mSep << carrier_id;
		itr = mInNetStMap.find(str);
		if (itr == mInNetStMap.end())
		{
			mInNetStMap[str] = 1;		
		}
		else
		{
			itr->second += 1;
		}
	}
	else
	{
		aos_assert_r(strs.size() == 1, false);
	
		OmnString phonenum = strs[0];

		str = mKeyHead;
		str << day << mSep << phonenum;
		value_rslt.setKeyValue(str.data(), str.length(), true, 1);
		rslt = mInNetIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosDataJoinNetStatusNew::appendOutNetEntry(
		const OmnString &key,
		const u64 &day,
		const AosRundataPtr &rdata)
{
	if (day > mLastDay)
	{
		OmnAlarm << "time is error, mEpochDay:" << mLastDay << ", day:" << day << enderr;
		return true;
	}

	bool finished = false;
	vector<OmnString> strs;
	AosStrSplit::splitStrBySubstr(key.data(),
		mFieldSep.data(), strs, 100, finished);

	bool rslt;
	OmnString str;
	AosValueRslt value_rslt;
	map<OmnString, int>::iterator itr;

	if (mNeedCity)
	{
		aos_assert_r(strs.size() == 3, false);

		OmnString phonenum = strs[0];
		OmnString city = strs[1];
		OmnString carrier_id = strs[2];

		str = mKeyHead;
		str << sgAllProvince << mSep << day << mSep << phonenum;
		value_rslt.setKeyValue(str.data(), str.length(), true, 1);
		rslt = mOutNetIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);

		str = mKeyHead;
		str << city << mSep << day << mSep << phonenum;
		value_rslt.setKeyValue(str.data(), str.length(), true, 1);
		rslt = mOutNetIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);

		str = mKeyHead;
		str << sgAllProvince << mSep << day << mSep << carrier_id;
		itr = mOutNetStMap.find(str);
		if (itr == mOutNetStMap.end())
		{
			mOutNetStMap[str] = 1;		
		}
		else
		{
			itr->second += 1;
		}

		str = mKeyHead;
		str << city << mSep << day << mSep << carrier_id;
		itr = mOutNetStMap.find(str);
		if (itr == mOutNetStMap.end())
		{
			mOutNetStMap[str] = 1;		
		}
		else
		{
			itr->second += 1;
		}
	}
	else
	{
		aos_assert_r(strs.size() == 1, false);
	
		OmnString phonenum = strs[0];

		str = mKeyHead;
		str << day << mSep << phonenum;
		value_rslt.setKeyValue(str.data(), str.length(), true, 1);
		rslt = mOutNetIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}
	return true;
}


void
AosDataJoinNetStatusNew::clear()
{
	mCtlr = 0;
	mIILScanner->clear();
	mIILScanner = 0;
	mInNetIILAsm = 0;
	mInNetStIILAsm = 0;
	mOutNetIILAsm = 0;
	mOutNetStIILAsm = 0;
}


AosDataJoinPtr
AosDataJoinNetStatusNew::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinNetStatusNew * join = OmnNew AosDataJoinNetStatusNew(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinNetStatusNew::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	OmnString proc_datetime = task->getTaskENV(AOSTAG_PROC_DATETIME, rdata);
	aos_assert_r(proc_datetime != "", false);
	
	u64 epochDay = AosTimeFormat::strToEpochDay(proc_datetime, AosTimeFormat::eYYYYMMDD);  
	aos_assert_r(epochDay > 0, false);

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
	
	AosDataAssemblerObjPtr assm;
	assm = createIILAsm(def->getFirstChild(AOSTAG_INNET_IILASM), task, rdata);
	aos_assert_r(assm, false);

	assm = createIILAsm(def->getFirstChild(AOSTAG_OUTNET_IILASM), task, rdata);
	aos_assert_r(assm, false);

	assm = createIILAsm(def->getFirstChild(AOSTAG_INNET_ST_IILASM), task, rdata);
	aos_assert_r(assm, false);

	assm = createIILAsm(def->getFirstChild(AOSTAG_OUTNET_ST_IILASM), task, rdata);
	aos_assert_r(assm, false);

	return true;
}

