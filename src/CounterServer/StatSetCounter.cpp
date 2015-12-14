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
#include "CounterServer/StatSetCounter.h"

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


AosStatSetCounter::AosStatSetCounter(
		const u64 &iilid,
		const AosRundataPtr &rdata)
:
AosStatProc(iilid, AosCounterOperation::eSetCounter)
{
	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin); 
}


bool 
AosStatSetCounter::proc(
		const OmnString &counter_id,
		const AosBuffPtr &buff, 
		AosVirCtnrSvr* vserver, 
		const AosRundataPtr &rdata)
{
	//TSK0227-Statistics2  5.Set Counters
	// where 'N' should be AosTimeGran::eNoTimeFlag and 'S' is the
	// statType. Both are 'char'.
	// where 'T' should be AosTimeGran::eTimeFlag and 'S' is the
	// statType. Both are 'char'.
	// Example:
	// 
	//  Example:
	//      cname:cool.cn.js.sz         counter_id : cool;  2012/01/01 00:00:00
	//      type 1:
	//      Nf|$_|cool|$$|cn|$$|js|$_|sz								child
	//      Nf|$_|cool|$$|cn|$_|js										parents
	//      Nf|$_|cool|$$|cn											parents
	//      Nf|$_|cool													parents
	//      Nf|$_|__zkyroot												parents
	// 
	// 		type 2:
	//      Tf|$_|cool|$$|cn|$$|js|$$|sz|$_|2923023476719616			child
	//      Tf|$_|cool|$$|cn|$$|js|$_|2923023476719616					parents
	//		Tf|$_|cool|$$|cn|$_|2923023476719616						parents
	//      Tf|$_|cool|$_|2923023476719616								parents
	//      Tf|$_|__zkyroot|$_|2923023476719616							parents
	//
	//		type 3:
	//		Tf|$_|cool|$$|cn|$$|js|$$|2923023476719616|$_|sz 			child
	//		Tf|$_|cool|$$|cn|$$|2923023476719616|$_|js 					parents 
	//		Tf|$_|cool|$$|2923023476719616|$_|cn					 	parents	
	//		Tf|$_|2923023476719616|$_|cool								parents 
	//
	// entry_name:NS|$_|cool|$$|cn|$$|js  member: sz; ...
	
	// This function sets a counter. That is, given an entry:
	// 		[entry-key, value]
	// it checks whether the entry already exists. If yes, it does 
	// nothing. Otherwise, it creates the entry. If it has parents, 
	// all their parents are incremented by one.
	
	mLock->lock();
	OmnString cname = buff->getOmnStr("");
	int64_t value = buff->getI64(0);
	u64 unitime = buff->getU64(0);
	AosStatType::E statType = (AosStatType::E)buff->getU8(0);
	aos_assert_rl(cname != "", mLock, false);
	aos_assert_rl(mBuff, mLock, false);

OmnScreen << "SET cname : " << cname << " value: " << value << endl; 
	mBuff->setOmnStr(cname);
	mBuff->setI64(value);
	mBuff->setU64(unitime);
	mBuff->setU8((u8)statType);

	if (mBuff->dataLen() > 0 && (u32)mBuff->dataLen() > mMaxDocs)
	{
		mBuff->reset();
		OmnString cname;
		bool type1_added, type2_added, type3_added;
		while (mBuff->dataLen()>0 && mBuff->getCrtIdx() < mBuff->dataLen())
		{
			cname = mBuff->getOmnStr("");
			int64_t vv = mBuff->getI64(0);
			unitime = mBuff->getU64(0);
			statType = (AosStatType::E)mBuff->getU8(0);

			//cname: CounterName|$$|Field1|$$|Field2|$$|Field3
			AosCounterName counter_name(cname);
			OmnString container, member;
			bool need_proc_time = AosTime::isValidTime(unitime);
			OmnString key;
			if (need_proc_time)
			{
				//Type2: Tf|$_|CounterName|$$|Field1|$$|Field2|$$|Field3|$_|unitime
				key = AosCounterUtil::composeTimeCounterName(cname, statType, unitime);
				OmnScreen << "key: " << key  << " vv:" << vv<< endl;
				setValue(key, vv, type2_added, rdata);
				OmnScreen << "type2_added :" <<(type2_added ? "true": "false") << endl;
			}

			if (counter_name.nextName(container, member))
			{
				if (container == "")
				{
					OmnAlarm << "NULL" << enderr;
					break;
				}

				if (member == "")
				{
					key = AosCounterUtil::composeCounterKey(container, statType);
					OmnScreen << "key: " << key << " vv:" << vv << endl;
					setValue(key, vv, type1_added, rdata);
					OmnScreen << "type1_added :" << (type1_added ? "true": "false") << endl;

					if (need_proc_time)
					{
						key = AosCounterUtil::composeTimeCounterName("", statType, unitime, container);
						OmnScreen << "key: " << key << " vv:" << vv<< endl;
						setValue(key, vv, type3_added, rdata);
						OmnScreen << "type3_added :" << (type3_added ? "true": "false") << endl;
					}
				}
				else
				{
					//Type1: Nf|$_|CounterName|$$|Field1|$$|Field2|$$|$_|Field3
					key = AosCounterUtil::composeCounterName(container, statType, member);
					OmnScreen << "key: " << key << " vv:" << vv << endl;
					setValue(key, vv, type1_added, rdata);
					OmnScreen << "type1_added :" << (type1_added ? "true": "false") << endl;

					if (need_proc_time)
					{
						if(type2_added)
						{
							//Type2: Tf|$_|CounterName|$$|Field1|$$|Field2|$$|$_|unitime
							key = AosCounterUtil::composeTimeCounterName(container, statType, unitime);
							OmnScreen << "key: " << key  << " type2  vv:" << vv << endl;
							setValue(key, vv, rdata);
						}

						//Type3: Tf|$_|CounterName|$$|Field1|$$|Field2|$$|unitime|$_|Field3
						key = AosCounterUtil::composeTimeCounterName(container, statType, unitime, member);
						OmnScreen << "key: " << key << " vv:" << vv<< endl;
						setValue(key, vv, type3_added, rdata);
						OmnScreen << "type3_added :" << (type3_added ? "true": "false") << endl;
					}
				}
			}

			while (counter_name.nextName(container, member))
			{
				if (member == "" && container != "")
				{
					if (type1_added)
					{
						//Type1: Nf|$_|CounterName
						key = AosCounterUtil::composeCounterKey(container, statType);
						OmnScreen << "key: " << key << " type1 vv:" << vv<< endl;
						setValue(key, vv, rdata);
					}

					if (need_proc_time && type3_added)
					{
						//Type3: Tf|$_|unitime|$$|CounterName
						key = AosCounterUtil::composeTimeCounterName("", statType, unitime, container);
						OmnScreen << "key: " << key <<" type3 vv:" << vv<< endl;
						setValue(key, vv, rdata);
					}
					break;
				}
				if (container == "" || member == "")
				{
					OmnAlarm << "NULL" << enderr;
					break;
				}

				if (type1_added)
				{
					//Type1: Nf|$_|CounterName...|$_|member
					key = AosCounterUtil::composeCounterName(container, statType, member);
					OmnScreen << "key: " << key  << " type1  vv:" << vv << endl;
					setValue(key, vv, rdata);
				}

				if (need_proc_time && type2_added)
				{
					//Type2: Tf|$_|CounterName ...|$_|unitime
					key = AosCounterUtil::composeTimeCounterName(container, statType, unitime);
					OmnScreen << "key: " << key << " type2  vv:" << vv<< endl;
					setValue(key, vv, rdata);
				}

				if (need_proc_time && type3_added)
				{
					//Type3: Tf|$_|CounterName ...|$$|unitime|$_|member
					key = AosCounterUtil::composeTimeCounterName(container, statType, unitime, member);
					OmnScreen << "key: " << key << " type3  vv:" << vv << endl;
					setValue(key, vv, rdata);
				}

			}

			// Add the root level entries
			if (type1_added)
			{
				// Type1: Nf|$_|__zkyroot
				key = AosCounterUtil::composeRootCounterName(statType);
				OmnScreen << "key: " << key << " type1  vv:" << vv<< endl;
				setValue(key, vv, rdata);
			}
			if (need_proc_time && type2_added)
			{
				//Type2: Tf|$_|__zkyroot|$_|unitime
				key = AosCounterUtil::composeRootTimeCounterName(
						statType, unitime);
				OmnScreen << "key: " << key <<" type2  vv:" << vv<< endl;
				setValue(key, vv, rdata);
			}
		}
		mBuff->reset();
		mBuff->setDataLen(0);
	}
	mLock->unlock();
	return true;
}

bool
AosStatSetCounter::setValue(
			const OmnString &name,
			const int64_t &vvv, 
			const AosRundataPtr &rdata)
{
	u64 init_value = AosCounterUtil::eLargeValue;
	u64 docid;
	AosIncrementKeyedValue(mIILID, name, docid, vvv, init_value, true, rdata);
	return true;
}

bool
AosStatSetCounter::setValue(
		const OmnString &name,
		const int64_t &vvv,
		bool &added,
		const AosRundataPtr &rdata)
{
	u64 init_value = AosCounterUtil::eLargeValue;
	u64 docid;
	AosIncrementKeyedValue(mIILID, name, docid, vvv, init_value, true, rdata);
	u64 vv = AosCounterUtil::convertValueToU64(vvv);
	if ((docid + vvv) == vv)
	{
		added = true;
		return true;
	}
	added = false;
	return true;
}
