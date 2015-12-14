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
#include "ThreadShellRunners/RunnerDistrBlobToBuckets.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataSourceObj.h"
#include "SEInterfaces/DataBlobObj.h"
#include "ThreadShellRunners/Ptrs.h"
#include "Util/File.h"


bool
AosDistrBlobToBucketsRunner::run(
		const AosDataBlobObjPtr &blob,
		const AosDataSourceObjPtr &data_source,
		const AosRundataPtr &rdata)
{
	AosDistrBlobToBucketsRunnerPtr runner = OmnNew AosDistrBlobToBucketsRunner(
			blob, data_source, rdata);
	return OmnThreadShellMgr::getSelf()->proc(runner);
}


bool
AosDistrBlobToBucketsRunner::run()
{
	// This function distributes 'mBlob' into the buckets based on 
	// the distribution map. 
	aos_assert_r(mRundata, false);
	aos_assert_rr(mBlob, mRundata, false);
	if (!mBlob->isSorted()) mBlob->sort();

	vector<OmnString> &distr_map = mDataSource->getDistrMap();
	aos_assert_rr(distr_map.size() > 0, mRundata, false);
	if (mBlob->size() < 1)
	{
		return true;
	}

	if (distr_map.size() == 1)
	{
		return appendToFile(0, mBlob->size()-1, 0, mRundata);
	}

	int64_t start_pos = 0;
	int64_t end_pos = -1;
	for (u32 i=0; i<distr_map.size(); i++)
	{
		OmnString key = distr_map[i];
		end_pos = mBlob->findFirstEntry(start_pos, key, mRundata);
		if (end_pos >= start_pos)
		{
			bool rslt = appendToFile(start_pos, end_pos, i, mRundata);
			aos_assert_r(rslt, false);
			start_pos = end_pos+1;
		}
	}

	aos_assert_rr(start_pos >= 0, mRundata, false);
	if (start_pos < mBlob->size())
	{
		return appendToFile(start_pos, mBlob->size(), distr_map.size(), mRundata);
	}

	return true;
}


bool		
AosDistrBlobToBucketsRunner::procFinished()
{
	aos_assert_r(mRundata, false);
	if (mDataSource) 
	{
		AosDistrBlobToBucketsRunnerPtr thisptr(this, false);
		mDataSource->procFinished(thisptr);
	}
	return true;
}


bool
AosDistrBlobToBucketsRunner::appendToFile(
		const u64 &start_pos,
		const u64 &end_pos, 
		const u32 file_idx, 
		const AosRundataPtr &rdata)
{
	// This function appends the contents to the 'file_idx'-th file. 
	aos_assert_r(mRundata, false);
	aos_assert_rr(mDataSource, mRundata, false);
	OmnFilePtr file = mDataSource->getFile(file_idx, mRundata);
	aos_assert_rr(file, mRundata, false);
	int64_t mem_size = 0;
	const char *mem = mBlob->getMemory(start_pos, end_pos, mem_size, mRundata);
	if (!mem)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		return false;
	}

	return file->append(mem, mem_size, false);
}

