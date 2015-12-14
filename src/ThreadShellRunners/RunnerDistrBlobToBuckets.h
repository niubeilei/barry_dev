////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ThreadShellRunners_RunnerDistrBlobToBuckets_h
#define Aos_ThreadShellRunners_RunnerDistrBlobToBuckets_h

#include "DataBlob/Ptrs.h"
#include "DataSource/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/ThreadShellMgr.h"
#include "Thread/ThrdShellProc.h"
#include "Util/RCObjImp.h"

class AosDistrBlobToBucketsRunner : virtual public OmnThrdShellProc
{
	OmnDefineRCObject;

private:
	const AosDataBlobObjPtr		mBlob;
	const AosDataSourceObjPtr	mDataSource;
	const AosRundataPtr			mRundata;

public:
	AosDistrBlobToBucketsRunner(
		const AosDataBlobObjPtr &blob,
		const AosDataSourceObjPtr &data_source,
		const AosRundataPtr &rdata)
	:
	OmnThrdShellProc(""),
	mBlob(blob),
	mDataSource(data_source),
	mRundata(rdata)
	{
	}

	virtual bool        run();
	virtual bool		procFinished();

	static bool run(const AosDataBlobObjPtr &blob,
					const AosDataSourceObjPtr &data_source,
					const AosRundataPtr &rdata);

private:
	bool appendToFile(const u64 &start_pos, 
					const u64 &end_pos, 
					const u32 file_idx, 
					const AosRundataPtr &rdata);
};
#endif

