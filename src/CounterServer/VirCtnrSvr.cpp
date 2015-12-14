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
// Counters are identified through Counter IDs, which are assigned by users.
// Each counter has an IIL, called Counter IIL, that manages all its values. 
// Counter IILs are String IILs. Its keys are in the form:
// 		TS_<counter-container>_<counter-member>
// where 'T' is the time flag and 'S' is the statistics flag. To fully distribute
// counters, a Counter IIL is split over N virtuals. The distribution is 
// determined by:
// 		TS_<counter-container>
//
// A counter is defined in the form:
// 		111.222.333.444...
// It defines the following counters:
// 		TS_222.333.444.555_111
// 		TS_333.444.555_222
// 		TS_444.555_333
// 		TS_555_444
// where 'S' is the statistics type (one char long) and 'T' is the time flag.
//
// If it is timed counter, the time is defined as;
// 		uuu.sss.nnn.hhh.ddd.mmm.yyy
// where:
// 		uuu: for milliseconds
// 		sss: for seconds
// 		nnn: for minute
// 		hhh: for hours
// 		ddd: for days
// 		mmm: for months
// 		yyy: for years
// All are optional.
//
// Example:
// Given a counter: 111.222.333.444 and time: hhh.ddd.mmm.yyy, it will 
// create the following counters:
// 		
// 		hhh.ddd.mmm.yyy.111.222.333.444
// 		hhh.ddd.mmm.yyy.222.333.444
// 		hhh.ddd.mmm.yyy.333.444
// 		hhh.ddd.mmm.yyy.444
// 		111.222.333.444
// Or:
// 		TC_222.333.444_111
// 		TC_333.444_222
// 		TC_444_333
// 		TC_111.222.333.444_time
// 		TC_222.333.444_time
// 		TC_333.444_time
// 		TC_444_time
//
// Time Based Queries
// Given a counter 'cname', its time based queries are done by:
// 		[start_time, end_time, time.cname, stattype]
//
// Time Aggregator
// Time Aggregators are the algorithms that combine multiple queried values 
// into one. For instance, if we want to query on days, values belonging 
// to the same day are aggregated into one value. 
//
// Given a counter [member, counter, value, type], 'counter' is a 'container', 
// and 'member' is a member in the container. 
//
// StatTypes
// 		sum: 	the sum of the counted values
// 		count: 	the count of the counted values
// 		value:	collect the values
// 		value_ct:	counts the occurences of values.
//
// Derived Stats:
// 		average: 		calculated from 'sum' and 'count'
// 		max:			calculated from 'value'.
// 		min:			calculated from 'value'
// 		standard error: calculated from 'value'.
//
// Time Granularity
// The following time granularity is supported:
// 		eYearly
// 		eMonthly
// 		eWeekly
// 		eDayly
// 		eHourly
// 		eMinutely
// 		eSecondly
//
// If it is eMonthly, it means when doing the statistics, it will ignore 
// days, hours, minutes, seconds, and milliseconds. Or in other word, 
// it is the sum of all the values for given days. As an example: 
// 		[2012-04-02-04:59:33:24]
// we want to do the following time-based statistics:
// 		eDaily
// 		eHourly
// 		eMinutely
// 		eSecondly
// This means that four time-based values for each counter are needed:
// 		[Daily,  	2012-04-02-00:00:00]
// 		[Hourly, 	2012-04-02-04:00:00]
// 		[Minutely, 	2012-04-02-04:59:00]
// 		[Secondly, 	2012-04-02-04:59:24]
// The first parameter tells the time granularity and the second parameter
// tells the actual value.
//
// Caching
// The class maintains a cache whose key is 'member' + 'container' + 'type'. 
// Its value is a u64.
//
// Distribution
// Counters are identified by strings:
// 		aaa.bbb.ccc.ddd
// One way to distribute counters are to calculate the hash key of a counter, 
// and then distribute based on the hash key. For a given counter "aaa.bbb.ccc.ddd"
// and a counter type 'type', the values for the counter is managed through 
// an IIL. The IIL is identified by: "bbb.ccc.ddd.type", and the member key in 
// the IIL is 'aaa'. As far as this counter is concerned, we only care about
// where the IIL is. 
// 
// Modification History:
// 05/19/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CounterServer/VirCtnrSvr.h"

#include "API/AosApi.h"
#include "CounterServer/StatProc.h"
#include "CounterUtil/CounterTrans.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/CounterName.h"
#include "CounterUtil/CounterOperations.h"
#include "DbQuery/Query.h"
#include "IILUtil/IILId.h"
#include "IILUtil/IILUtil.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "ValueSel/ValueSel.h"
#include "Util/UtUtil.h"
#include "Util/Opr.h"
#include "Util/File.h"
#include "Util/StrU64Array.h"
#include "Util/Buff.h"
#include "UtilTime/TimeInfo.h"

OmnMutexPtr 			AosVirCtnrSvr::smLock = OmnNew OmnMutex();
//OmnThreadPtr			AosVirCtnrSvr::smThread;
AosVirCtnrSvr::CtMap_t	AosVirCtnrSvr::smCounterMap;

AosVirCtnrSvr::AosVirCtnrSvr(const u32 id, const AosXmlTagPtr &conf)
{
	config(conf);
}


AosVirCtnrSvr::~AosVirCtnrSvr()
{
	
}

bool
AosVirCtnrSvr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosVirCtnrSvr::start(const AosRundataPtr &rdata)
{
	//OmnThreadedObjPtr thisptr(this, false);
	//mThread = OmnNew OmnThread(thisptr, "countermgr", 0, true, true, __FILE__, __LINE__);
	//mThread->start();
	return true;
}


bool
AosVirCtnrSvr::stop()
{
	return true;
}


u64
AosVirCtnrSvr::getIILIDByCounterId(
		const OmnString &counter_id, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(counter_id != "", rdata, 0);
	smLock->lock();

	// Check whether it is in the cache.
	CtMapItr_t itr = smCounterMap.find(counter_id);
	if (itr != smCounterMap.end())
	{
		u64 iilid = itr->second->getIILID();
		smLock->unlock();
		return iilid;
	}
	smLock->unlock();

	// It is not in the cache.
	return AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, counter_id, 0, rdata);
}


AosStatProcPtr
AosVirCtnrSvr::retrieveCounterProcSafe(
		const OmnString &counter_id, 
		const AosCounterOperation::E &opr,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(counter_id != "", rdata, 0);
	smLock->lock();
	CtMapItr_t itr = smCounterMap.find(counter_id);
	AosStatProcPtr proc = 0;
	if (itr == smCounterMap.end())
	{
		OmnString kk = counter_id;
		u64 iilid = AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, kk, 0, rdata);
		if (iilid == 0) 
		{
			// It is not created yet. Create it now.
			iilid = AosCreateIIL(counter_id, eAosIILType_Str, true, rdata);
			aos_assert_rl(iilid, smLock, 0);
			u64 tmp_iilid = AOSIILID_COUNTER_IILIDS;
			AosSetStrValueDocUnique(tmp_iilid, false, kk, iilid, false, rdata);
			aos_assert_rl(tmp_iilid == AOSIILID_COUNTER_IILIDS, smLock, 0);
		}

		AosOrder::E order = AosIILUtil::getIILOrder(counter_id);
		proc = AosStatProc::createStatProc(iilid, order, opr, rdata);
		smCounterMap[counter_id] = proc;
	}
	else
	{
		proc = itr->second;
	}
	smLock->unlock();
	return proc;
}


bool	
AosVirCtnrSvr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
	{
		OmnSleepSec(1000);
	}
	return true;
}


bool	
AosVirCtnrSvr::signal(const int threadLogicId)
{
	return true;
}


bool    
AosVirCtnrSvr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosVirCtnrSvr::setEmptyContents(const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return false;
}


//bool
//AosVirCtnrSvr::proc(
//		const AosBigTransPtr &trans,
//		const AosBuffPtr &buff, 
//		const AosRundataPtr &rdata)
bool
AosVirCtnrSvr::proc(
		const AosCounterTransPtr &trans,
		const AosRundataPtr &rdata)
{
	// It retrieves a transaction that may contain multiple requests. 
	// The transaction format is:
	// 		counter_id		OmnString
	// 		operation		u8
	// 		contents		(depending on the operation)

	// Ketty 2013/03/28
	/*
	int guard = AosCounterUtil::eMaxTransPerBuffTrans;
	while (guard-- && buff->getCrtIdx() < buff->dataLen())
	{
		// Retrieve the counterProc
		OmnString counter_id = buff->getOmnStr("");
		if (counter_id == "")
		{
			AosSetErrorU(rdata, "counter_counter_id_empty");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		u8 ss = buff->getU8(AosCounterOperation::eInvalid);
		AosCounterOperation::E operation = (AosCounterOperation::E)ss;
		if (!AosCounterOperation::isValid(operation))
		{
			AosSetErrorU(rdata, "unrecognized_counter_operation") << ": " << operation;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		AosStatProcPtr counter = retrieveCounterProcSafe(counter_id, operation, rdata);
		if (!counter)
		{
			AosSetErrorU(rdata, "failed_retr_counter") << ": " << counter_id;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		counter->proc(counter_id, buff, this, rdata);
	}
	aos_assert_rr(guard, rdata, false);
	*/
	OmnString counter_id = trans->getCounterId();
	AosCounterOperation::E operation = trans->getOpr();
		
	if (counter_id == "")
	{
		AosSetErrorU(rdata, "counter_counter_id_empty");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	if (!AosCounterOperation::isValid(operation))
	{
		AosSetErrorU(rdata, "unrecognized_counter_operation") << ": " << operation;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
		
	AosStatProcPtr counter = retrieveCounterProcSafe(counter_id, operation, rdata);
	if (!counter)
	{
		AosSetErrorU(rdata, "failed_retr_counter") << ": " << counter_id;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosBuffPtr cont = trans->getCont();
	counter->proc(counter_id, cont, this, rdata);
	return true;
}


void
AosVirCtnrSvr::setShowLog(bool b)
{
	AosStatProc::setShowLog(b);
}
