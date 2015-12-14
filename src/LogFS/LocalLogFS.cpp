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
// Modification History:
// 2014/09/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LogFS/LocalLogFS.h"


AosLocalLogFS::AosLocalLogFS()
{
}


AosLocalLogFS::~AosLocalLogFS()
{
}


bool
AosLocalLogFS::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	return true;
}


bool
AosLocalLogFS::appendData(
		AosRundata *rdata, 
		const char *data, 
		const int len, 
		u64 &pos)
{
	// This function appends 'data' to the current log file.
	// After that, it syncs the changes to all the sync backups. 
	pos = (mSeqno << 32) + mCrtSize;

	bool rslt = mActiveFile1->append(data, len, true);
	aos_assert_rr(rslt, rdata, false);

	bool rslt = mActiveFile2->append(data, len, true);
	aos_assert_rr(rslt, rdata, false);

	mCrtSize += len;
	if (fileTooBig()) createNextLogFile(rdata);

	return true;
}


bool
AosLocalLogFS::createNextLogFile(AosRundata *rdata)
{
	OmnString fname1, fname2;
	AosGetPartitions(fname1, fname2);
	aos_assert_rr(fname1 != "", rdata, false);
	aos_assert_rr(fname2 != "", rdata, false);

	// The file name is:
	// "base_dir/Cubes/Cube<ddd>/LogFS/<logfs_docid>"
	mSeqno++;
	fname1 << "/Cubes/Cube" << mCubeId << "/LogFS/" << mLogFSDocid
		<< "/" << mPrefix << "_" << mSeqno;
	mActiveFile1 = OmnNew OmnFile(fname1, OmnFile::eCreate);
	aos_assert_rr(mActiveFile1, rdata, false);

	fname2 << "/Cubes/Cube" << mCubeId << "/LogFS/" << mLogFSDocid
		<< "/" << mPrefix << "_" << mSeqno;
	mActiveFile2 = OmnNew OmnFile(fname2, OmnFile::eCreate);
	aos_assert_rr(mActiveFile2, rdata, false);

	return true;
}

