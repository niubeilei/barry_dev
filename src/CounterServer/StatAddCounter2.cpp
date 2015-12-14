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
// 12/24/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "CounterServer/StatAddCounter2.h"

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
#include "Util/StrSplit.h"

#if 0

AosStatAddCounter2::AosStatAddCounter2(
		const u64 &iilid, 
		const AosOrder::E order, 
		const AosRundataPtr &rdata)
:
AosStatProc(iilid, AosCounterOperation::eAddCounter)
{
//	AosCompareFunPtr cmp = OmnNew AosFunStrU641(eMaxSize, false);
//	mValues = OmnNew AosBuffArray(cmp, true, false, 0);
	mValues = OmnNew AosBuffArray(eMaxSize);
}



bool 
AosStatAddCounter2::proc(
		const OmnString &counter_id,
		const AosBuffPtr &buff, 
		AosVirCtnrSvr* vserver, 
		const AosRundataPtr &rdata)
{
	// Example:
	// 		cname:cn.js.sz 		counter_id : cool;  2012/01/01 00:00:00
	// 		level|append_bit|stattype|time
	// 		3|1|sum|ntmcn.js.sz
	// 		2|1|sum|ntmcn.js
	// 		1|1|sum|ntmcn
	// 		0|1|sum|ntm
	//
	//		4|1|sum|daycn.js.15672.sz
	//		3|1|sum|daycn.15672.js
	//		2|1|sum|day15672.cn
	//		1|1|sum|day15672
	
	// The format is:
	// 		cname			OmnString
	// 		cvalue			int64_t
	// 		unitime			u64
	// 		stattype		u8
	OmnString cname = buff->getOmnStr("");
	int64_t value = buff->getI64(0);
	u64 unitime = buff->getU64(0);
	AosTimeGran::E time_gran = (AosTimeGran::E)buff->getU8(0);
	u16 entryType = buff->getU16(0);

	aos_assert_rr(cname != "", rdata, false);
	aos_assert_rr(mValues, rdata, false);

	mLock->lock();
	AosCounterName counter_name;
	counter_name.setName2(cname);

	OmnString container, member;
	int level = 0;
	u64 vv = value;

	bool need_proc_time = unitime > 0;//AosTime::isValidTime(unitime);

	OmnString key;
	bool rslt = true;
	u16 entry_type = 0;
	bool finished = false;
	vector<OmnString> str;
	AosStrSplit::splitStrByChar(cname.data(), AOS_COUNTER_SEGMENT_SEP2, str, 100, finished);  
	level = str.size();

	entry_type = AosCounterUtil::modifyEntryType(entryType, level, 1, AosTimeGran::eNoTime);
	key = AosCounterUtil::composeTerm2(entry_type, cname, 0);
	mValueRslt.reset();
	mValueRslt.setStr(key);
	//mValueRslt.setDocid(vv);
	rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
	if(smShowLog) OmnScreen << "key(level|append_bit|stattype|timetype): " << level <<"|1|ntm " << key << endl;
	aos_assert_rl(rslt, mLock, false);

	while (counter_name.nextName2(container, member, level))
	{
		if (member == "" && container != "")
		{
			if (need_proc_time)
			{
				// level + 1 (timeTerm)
				entry_type = AosCounterUtil::modifyEntryType(entryType, level + 1, 1, time_gran);
				key = AosCounterUtil::composeTerm2(entry_type, "", unitime);
				key = AosCounterUtil::composeStrTerm2(key, container);
				mValueRslt.reset();
				//mValueRslt.setCharStr1(key.data(), key.length(), true);
				//mValueRslt.setDocid(vv);
				rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
				if(smShowLog) OmnScreen << "key(level|append_bit|stattype|timetype): " << level + 1 <<"|1|day "<< key << endl;
				aos_assert_rl(rslt, mLock, false);
			}
			break;
		}
		if (container == "" || member == "")
		{
			OmnAlarm << "NULL" << enderr;
			break;
		}

		entry_type = AosCounterUtil::modifyEntryType(entryType, level, 1, AosTimeGran::eNoTime);
		key = AosCounterUtil::composeTerm2(entry_type, container, 0);
		mValueRslt.reset();
		//mValueRslt.setCharStr1(key.data(), key.length(), true);
		//mValueRslt.setDocid(vv);
		rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
		if(smShowLog) OmnScreen << "key(level|append_bit|stattype|timetype): " << level <<"|1|ntm " << key << endl;
		aos_assert_rl(rslt, mLock, false);

		if (need_proc_time)
		{
			// level + 2 (timeTerm + member)
			//  level + 2|0|sum|time_gran container.unitime.member
			entry_type = AosCounterUtil::modifyEntryType(entryType, level + 2, 1, time_gran);
			key = AosCounterUtil::composeTerm2(entry_type, container, unitime);
			key = AosCounterUtil::composeStrTerm2(key, member);
			mValueRslt.reset();
			//mValueRslt.setCharStr1(key.data(), key.length(), true);
			//mValueRslt.setDocid(vv);
			rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
			if(smShowLog) OmnScreen << "key(level|append_bit|stattype|timetype): " << level + 2 <<"|1|day "<< key << endl;
			aos_assert_rl(rslt, mLock, false);
		}
	}

	// Add the root level entries
	entry_type = AosCounterUtil::modifyEntryType(entryType, 0, 1, AosTimeGran::eNoTime);
	key = AosCounterUtil::composeTerm2(entry_type, AOS_ROOT_COUNTER_KEY, 0);
	mValueRslt.reset();
	//mValueRslt.setCharStr1(key.data(), key.length(), true);
	//mValueRslt.setDocid(vv);
	rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
	if(smShowLog) OmnScreen << "key(level|append_bit|stattype|timetype): 0|1|ntm " << key << endl;
	aos_assert_rl(rslt, mLock, false);
	if (need_proc_time)
	{
		entry_type = AosCounterUtil::modifyEntryType(entryType, 1, 1, time_gran);
		key = AosCounterUtil::composeTerm2(entry_type, "", unitime);
		mValueRslt.reset();
		//mValueRslt.setCharStr1(key.data(), key.length(), true);
		//mValueRslt.setDocid(vv);
		rslt = mValues->appendEntry(mValueRslt, rdata.getPtr());
		if(smShowLog) OmnScreen << "key(level|append_bit|stattype|timetype): 1|1|day "<< key << endl;
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
		}
		mValues->clear();
	}
	
	mLock->unlock();
	return true;
}
#endif
