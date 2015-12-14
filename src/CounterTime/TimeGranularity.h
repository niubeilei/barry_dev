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
#ifndef Omn_COunterTime_TimeGranularity_h
#define Omn_COunterTime_TimeGranularity_h

#include "Util/File.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "CounterTime/Ptrs.h"
#include "CounterTime/CounterTimeInfo.h"
#include "CounterTime/TimeCond.h"


class AosTimeGranularity : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eYearSize = 2*sizeof(u64),
		eYearMonthSize = (2+12)*sizeof(u64),
		eYearMonthDaySize = (2+12+365)*sizeof(u64),
		
		eYearMonthDayLeapSize = (2+12+366)*sizeof(u64),
	};

protected:
	TimeFormat		mTimeFormat;

public:
	AosTimeGranularity(
					const TimeFormat formatType, 
					const bool regflag); 
	~AosTimeGranularity() {}

	virtual bool procCounter(
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
					const AosRundataPtr &rdata) = 0;

	// virtual bool retrieveCounter(
	// 				const AosCounterRecordPtr &record,
	// 				const AosCounterTimeInfo &time,
	// 				const AosDocFileMgrPtr &file,
	// 				const AosRundataPtr &rdata) = 0;

	virtual bool retrieveCounterSet(
					const u32 seqno,
					const u64 offset,
					const u32 size,
					AosXmlTagPtr &record,
					const AosTimeCond &timeCond,
					const AosDocFileMgrPtr &file,
					const AosRundataPtr &rdata) = 0;

	virtual bool retrieveCountersByQry(
					const u32 seqno,
					const u64 offset,
					const u32 size,
					const OmnString &cname,
					const AosTimeCond &timeCond,
					const AosDocFileMgrPtr &file,
					const AosRundataPtr &rdata) = 0;

	static TimeFormat toEnum(const OmnString &type);
	static AosTimeGranularityPtr getProc(const OmnString &type);
	static AosTimeGranularityPtr getProc(const TimeFormat type);
protected:
	bool 	setEmptyContents(const AosRundataPtr &rdata);

private:
	bool 		registerProc(const AosTimeGranularityPtr &timePtr); 
};
#endif
#endif
