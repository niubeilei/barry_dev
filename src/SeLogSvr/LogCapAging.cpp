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
// This is management the Log
// Description:
//  <logs>
// 		<logname aging_type="xxx" max_fliesize="xxx" max_num_files="xxx"/>
// 	</logs>
//
// Modification History:
// 06/23/2011 Create by Tom 
////////////////////////////////////////////////////////////////////////////
#include "SeLogSvr/LogCapAging.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/File.h"
#include "ValueSel/ValueRslt.h"
#include "Rundata/Ptrs.h"
#include "Util/StrSplit.h"
#include "SeLogSvr/SeLog.h"
#include "SeLogUtil/LogId.h"


AosLogCapAging::AosLogCapAging(
		const u32 maxfilesize,
		const int maxfiles,
		const AosRundataPtr &rdata)
:
AosLogAging(eCapacityAging),
mMaxFileSize(maxfilesize),
mMaxNumFiles(maxfiles)
{
	u64 max_size = AosLogId::getMaxLogFileSize();
	if (mMaxFileSize > max_size) mMaxFileSize = max_size;
}


AosLogCapAging::AosLogCapAging(AosBuff &buff)
:
AosLogAging(eCapacityAging)
{
	mMaxFileSize = buff.getU32(0);
	mMaxNumFiles = buff.getInt(-1);
	aos_assert(mMaxFileSize > 0);
	aos_assert(mMaxNumFiles >= 0);
	u64 max_size = AosLogId::getMaxLogFileSize();
	if (mMaxFileSize > max_size) mMaxFileSize = max_size;
}


AosLogCapAging::AosLogCapAging(
		const AosXmlTagPtr &config, 
		const AosRundataPtr &rdata)
:
AosLogAging(eCapacityAging),
mMaxFileSize(eDftFileSize),
mMaxNumFiles(eDftMaxNumFiles)
{
	if (config)
	{
		mMaxFileSize = config->getAttrU64("max_filesize", eDftFileSize);
		mMaxNumFiles = config->getAttrInt("max_files", eDftMaxNumFiles);
	}
	u64 max_size = AosLogId::getMaxLogFileSize();
	if (mMaxFileSize > max_size) mMaxFileSize = max_size;
}


AosLogCapAging::~AosLogCapAging()
{
}


bool 
AosLogCapAging::isFull(
		const u32 writePos, 
		const AosSeLogPtr &selog,
		const AosRundataPtr &rdata)
{ 
	// This function checks whether the log is full. The log is full
	// if and only if it has more t
	if (writePos < mMaxFileSize) return false;
	
	aos_assert_rr(selog, rdata, false);
	int num_files = selog->getNumFiles();
	if (num_files >= mMaxNumFiles)
	{
		// There are too many files. Need to remove the oldest file.
		selog->removeOldestFile(rdata);
	}
	return true;
}


AosLogAgingPtr
AosLogCapAging::clone()
{
	AosRundataPtr rdata = OmnApp::getRundata();
	AosLogCapAgingPtr aging = OmnNew AosLogCapAging(mMaxFileSize, mMaxNumFiles, rdata);
	return aging.getPtr();
}
