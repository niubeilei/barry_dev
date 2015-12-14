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
//
// Modification History:
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CounterServer/StatAddCounter.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterServer/VirCtnrSvr.h"
#include "CounterUtil/CounterOperations.h"
#include "CounterUtil/CounterName.h"
#include "CounterUtil/CounterUtil.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/StrU64Array.h"
#include "Util/U64U64Array.h"
#include "UtilTime/TimeInfo.h"

#if 0

AosStatAddCounter::AosStatAddCounter(
		const u64 &iilid, 
		const AosOrder::E order, 
		const AosRundataPtr &rdata)
:
AosStatProc(iilid, AosCounterOperation::eAddCounter)
{
	//AosCompareFunPtr cmp = OmnNew AosFunStrU641(eMaxSize, false);
	//mValues = OmnNew AosStrU64Array(cmp, false);
	//mValues = OmnNew AosBuffArray(cmp, true, false, 0);
	mValues = OmnNew AosBuffArray(eMaxSize);
}



bool 
AosStatAddCounter::proc(
		const OmnString &counter_id,
		const AosBuffPtr &buff, 
		AosVirCtnrSvr* vserver, 
		const AosRundataPtr &rdata)
{
	// where 'N' should be AosTimeGran::eNoTimeFlag and 'S' is the
	// statType. Both are 'char'.
	// where 'T' should be AosTimeGran::eTimeFlag and 'S' is the
	// statType. Both are 'char'.
	// Example:
	// 		cname:cool.cn.js.sz 		counter_id : cool;  2012/01/01 00:00:00
	// 		Nf|$_|cool|$$|cn|$$|js|$_|sz
	// 		Nf|$_|cool|$$|cn|$_|js
	// 		Nf|$_|cool|$$|cn
	// 		Nf|$_|cool
	// 		Nf|$_|__zkyroot
	//
	// 		Tf|$_|cool|$$|cn|$$|js|$$|sz|$_|2923023476719616
	// 		Tf|$_|cool|$$|cn|$$|js|$_|2923023476719616
	// 		Tf|$_|cool|$$|cn|$_|2923023476719616
	// 		Tf|$_|cool|$_|2923023476719616
	// 		Tf|$_|__zkyroot|$_|2923023476719616
	//
	// 		Tf|$_|cool|$$|cn|$$|js|$$|2923023476719616|$_|sz
	// 		Tf|$_|cool|$$|cn|$$|2923023476719616|$_|js
	// 		Tf|$_|cool|$$|2923023476719616|$_|cn
	// 		Tf|$_|2923023476719616|$_|cool
	//
	// entry_name:NS|$_|cool|$$|cn|$$|js  member: sz; ...
	
	// The format is:
	// 		cname			OmnString
	// 		cvalue			int64_t
	// 		unitime			u64
	// 		stattype		u8
	OmnString cname = buff->getOmnStr("");
	int64_t value = buff->getI64(0);
	u64 unitime = buff->getU64(0);
	AosStatType::E statType = (AosStatType::E)buff->getU8(0);

	aos_assert_rr(cname != "", rdata, false);
	aos_assert_rr(mValues, rdata, false);

	mLock->lock();
	AosCounterName counter_name(cname);
	OmnString container, member;
	u64 vv = value;//AosCounterUtil::convertValueToU64(value);

	bool need_proc_time = AosTime::isValidTime(unitime);

	OmnString key;
	bool rslt = false;
	if (need_proc_time)
	{
		key = AosCounterUtil::composeTimeCounterName(cname, statType, unitime);
		mValueRslt.reset();
		mValueRslt.setCharStr1(key.data(), key.length(), true);
		mValueRslt.setDocid(vv);
		rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
		if (smShowLog) OmnScreen << "key: " << key << endl;
		aos_assert_rl(rslt, mLock, false);
	}
	
	while (counter_name.nextName(container, member))
	{
		if (member == "" && container != "")
		{
			key = AosCounterUtil::composeCounterKey(container, statType);
			mValueRslt.reset();
			mValueRslt.setCharStr1(key.data(), key.length(), true);
			mValueRslt.setDocid(vv);
			rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
			//rslt = mValues->appendEntry(key, vv, rdata);
			if(smShowLog) OmnScreen << "key: " << key << endl;
			aos_assert_rl(rslt, mLock, false);

			if (need_proc_time)
			{
				key = AosCounterUtil::composeTimeCounterName("", statType, unitime, container);
				mValueRslt.reset();
				mValueRslt.setCharStr1(key.data(), key.length(), true);
				mValueRslt.setDocid(vv);
				rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
				//rslt = mValues->appendEntry(key, vv, rdata);
				if (smShowLog) OmnScreen << "key: " << key << endl;
				aos_assert_rl(rslt, mLock, false);
			}
			break;
		}
		if (container == "" || member == "")
		{
			OmnAlarm << "NULL" << enderr;
			break;
		}
		key = AosCounterUtil::composeCounterName(container, statType, member);
		mValueRslt.reset();
		mValueRslt.setCharStr1(key.data(), key.length(), true);
		mValueRslt.setDocid(vv);
		rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
		//rslt = mValues->appendEntry(key, vv, rdata);
		if (smShowLog) OmnScreen << "key: " << key << endl;
		aos_assert_rl(rslt, mLock, false);

		if (need_proc_time)
		{
			key = AosCounterUtil::composeTimeCounterName(container, statType, unitime);
			mValueRslt.reset();
			mValueRslt.setCharStr1(key.data(), key.length(), true);
			mValueRslt.setDocid(vv);
			rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
			//rslt = mValues->appendEntry(key, vv, rdata);
			if (smShowLog) OmnScreen << "key: " << key << endl;
			aos_assert_rl(rslt, mLock, false);

			key = AosCounterUtil::composeTimeCounterName(container, statType, unitime, member);
			mValueRslt.reset();
			mValueRslt.setCharStr1(key.data(), key.length(), true);
			mValueRslt.setDocid(vv);
			rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
			//rslt = mValues->appendEntry(key, vv, rdata);
			if (smShowLog) OmnScreen << "key: " << key << endl;
			aos_assert_rl(rslt, mLock, false);
		}
	}

	// Add the root level entries
	key = AosCounterUtil::composeRootCounterName(statType);
	mValueRslt.reset();
	mValueRslt.setCharStr1(key.data(), key.length(), true);
	mValueRslt.setDocid(vv);
	rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
	//rslt = mValues->appendEntry(key, vv, rdata);
	if (smShowLog) OmnScreen << "key: " << key << endl;
	aos_assert_rl(rslt, mLock, false);
	if (need_proc_time)
	{
		key = AosCounterUtil::composeRootTimeCounterName(
				statType, unitime);
		mValueRslt.reset();
		mValueRslt.setCharStr1(key.data(), key.length(), true);
		mValueRslt.setDocid(vv);
		rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
		//rslt = mValues->appendEntry(key, vv, rdata);
		if (smShowLog) OmnScreen << "key: " << key << endl;
		aos_assert_rl(rslt, mLock, false);
	}

	if (mValues->getNumEntries() > mMaxDocs)
	{
		// Need to flush the data to IIL.
		// OmnNotImplementedYet;
		//mValues->sort();
		OmnString name;
		u64 vvv;
		bool unfinished = true;
		mValues->resetRecordLoop();
		while (unfinished)
		{
			//mValues->nextValue(name, vvv, finished);
			unfinished = mValues->nextValue(mValueRslt, true);
			if (!unfinished) 
			{
				mValues->clear();
				mLock->unlock();
				return true;
			}
			if (smShowLog) OmnScreen << "mIILID: " << mIILID << " name: " << name << " vvv: " << vvv << endl; 
			u64 docid;
			int datalen;
			name = OmnString(mValueRslt.getCharStr(datalen));
			vvv = mValueRslt.getDocid();
			u64 init_value = AosCounterUtil::eLargeValue;
			AosIncrementKeyedValue(mIILID, name, docid, vvv, init_value, true, rdata);
			//AosIncrementKeyedValue(mIILID, name, docid, vvv, vvv, true, rdata);
		}
		mValues->clear();
	}
	mLock->unlock();
	return true;
}
#endif
