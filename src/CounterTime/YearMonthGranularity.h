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
// 06/07/2011	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_COunterTime_YearMonthGranularity_h
#define Omn_COunterTime_YearMonthGranularity_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Rundata/Rundata.h"
#include "CounterTime/TimeGranularity.h"
#include "CounterUtil/CounterRecord.h"


class AosYearMonthGranularity : public AosTimeGranularity 
{
public:
	enum
	{
		eYearMonthRecordSize = sizeof(u32)+sizeof(int64_t)*13,
		eMaxYearMonthRecordsSize = 1000000
	};

	struct YearMonthRecord
	{
		u32 year;
		int64_t value;

		int64_t month[12];
	};

private:

public:
	AosYearMonthGranularity(
			const AosTimeGranularity::TimeFormat formatType, 
			const bool regflag);
	~AosYearMonthGranularity() {}

	bool procCounter(
			u32 &seqno,
			u64 &offset,
			u32 &size,
			u64 &nameIILID,
			u64 &valueIILID,
			const AosCounterTimeInfo &time,
			const OmnString &iilKey,
			const u64 &prevCounterId,
			const int64_t &value, 
			const AosDocFileMgrPtr &file,
			const AosRundataPtr &rdata);

	bool retrieveCounter(
            const AosCounterRecord &record,
	        const AosCounterTimeInfo &time,
	        const AosDocFileMgrPtr &file,
	        const AosRundataPtr &rdata);

	bool retrieveCounterSet(
            const u32 seqno,
            const u64 offset,
            const u32 size,
            AosXmlTagPtr &record,
            const AosTimeCond &timeCond,
            const AosDocFileMgrPtr &file,
            const AosRundataPtr &rdata);

	bool retrieveCountersByQry(
            const u32 seqno,
            const u64 offset,
            const u32 size,
			const OmnString &cname,
            const AosTimeCond &timeCond,
            const AosDocFileMgrPtr &file,
            const AosRundataPtr &rdata);

	int	  findOrCreateRecord(
			YearMonthRecord *records, 
			const AosCounterTimeInfo &time,
			const int64_t value,
			const int size,
			bool &insertFlag);
	
private:
	bool			init();
	bool			assembleYearResp(
								const u32 &fromYearPos,
								const OmnString &cname,
								const YearMonthRecord *records,
								const AosTimeCond &timeCond,
								const AosRundataPtr &rdata);

	bool			assembleYearMonthResp(
								const u32 &fromYearPos,
								const OmnString &cname,
								const YearMonthRecord *records,
								const AosTimeCond &timeCond,
								const AosRundataPtr &rdata);
};
#endif
#endif
