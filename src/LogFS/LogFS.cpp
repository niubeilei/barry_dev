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
#include "LogFS/LogFS.h"


AosLogFS::AosLogFS()
{
}


AosLogFS::~AosLogFS()
{
}


bool
AosLogFS::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	return true;
}


bool
AosLogFS::syncChanges(
		AosRundata *rdata, 
		const char *data, 
		const int len)
{
	// This function syncs the changes 'data' to all the sync backups. 
	// After that, it adds the FMT to the queue for async backups 
	// and inter data center backups.
	AosFmtPtr fmt = OmnNew AosFmt(eModifyFile, mSeqno, mOffset, data, len);
	mReplicMgr->syncFmt(rdata, fmt);
	return true;
}


bool
AosLogFS::createNextLogFile(AosRundata *rdata)
{
	OmnString fname = AosGetBaseDataDir();
	aos_assert_rr(fname != "", rdata, false);

	// The file name is:
	// "base_dir/Cubes/Cube<ddd>/LogFS/<logfs_docid>"
	mSeqno++;
	fname << "/Cubes/Cube" << mCubeId << "/LogFS/" << mLogFSDocid
		<< "/" << mPrefix << "_" << mSeqno;
	mActiveFile = OmnNew OmnFile(fname, OmnFile::eCreate);
	aos_assert_rr(mActiveFile, rdata, false);
	return true;
}


bool
AosLogFS::addStat(
		AosRundata *rdata, 
		FileOpr operation, 
		const int size)
{

