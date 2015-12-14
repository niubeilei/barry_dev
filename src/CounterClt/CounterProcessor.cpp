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
// A counter is defined in the form:
// 		111.222.333.444...
// It defines the following counters:
// 		111 + 222.333.444.555
// 		222 + 333.444.555
// 		333 + 444.555
// 		444 + 555
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
// 		hhh.ddd.mmm.yyy.111.222.333.444
// 		hhh.ddd.mmm.yyy.222.333.444
// 		hhh.ddd.mmm.yyy.333.444
// 		hhh.ddd.mmm.yyy.444
// 		111.222.333.444
// Or:
// 		[time, time.111.222.333.444]
// 		[time, time.222.333.444]
// 		[time, time.333.444]
// 		[time, time.444]
// 		[111, 222.333.444]
// 		[222, 333.444]
// 		[333, 444]
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
//
// 
// Modification History:
// 05/19/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "CounterServer/CounterProcessor.h"


AosCounterProcessor::AosCounterProcessor()
{
}


AosCounterProcessor::~AosCounterProcessor()
{
	
}

bool
AosCounterProcessor::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosCounterProcessor::start(const AosRundataPtr &rdata)
{
	return true;
}


/*
bool
AosCounterProcessor::updateCounter(
        const OmnString &cname,
        const int64_t &cvalue,
        const AosCounterTimeInfo &time,
        const OmnString &timetype,
		const vector<AosStatType::E> statTypes,
        const AosRundataPtr &rdata)
{
	for (u32 i=0; i<statTypes.size(); i++)
	{
		if (!AosStatType::isOrigType(statTypes[i]))
		{
			OmnAlarm << "Stat is invalid: " << statTypes[i] << enderr;
			continue;
		}

		updateCounter(cname, cvalue, time, timetype, statTypes[i], rdata);
	}
	return true;
}
*/


bool
AosCounterProcessor::processCounter(
        const OmnString &cname,
        const int64_t &cvalue,
        const AosCounterTimeInfo &time,
        const OmnString &timetype,
		const AosStatType::E statType,
        const AosRundataPtr &rdata)
{
	bool rslt = isValidCounterName(cname, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "invalid_counter_name") << ": " << cname;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (!AosStatType::isOrigType(statType))
	{
		AosSetError(rdata, "invalid_stat_type") << ": " << statType;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosCounterTime::E time_gran = AosCounterTime::toEnum(timetype);
	u64 unitime = time.getUniTime(time_gran);
	AosCounterName name(cname);
	OmnString counterName, member;

	while ((counterName = name.nextName(member)) != "")
	{
		OmnString key = createCounterKey(cname, statType);
		assembler->addCounter(key, member, unitime, cvalue, rdata);
	}

	aos_assert_rr(member != "", rdata, false);
	OmnString key = createCounterKey(AOSCOUNTER_ROOT_NAME, statType);
	assembler->addCounter(key, member, unitime, cvalue, rdata);
	return true;
}
#endif
