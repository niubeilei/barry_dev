////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 07/11/2012 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_DataScanner_FileReadRunner_h
#define AOS_DataScanner_FileReadRunner_h

#include "DataScanner/FileScanner.h"
#include "Thread/ThrdShellProc.h"
#include "Util/ReadFile.h"
#include "Util/File.h"

class AosFileReadRunner : public OmnThrdShellProc
{
	OmnDefineRCObject;

private:
	enum
	{
		eMinReadSize = 1000000,         // 1M
		eDftReadSize = 100000000        // 100M
	};
	AosDataScannerObjPtr	mScanner;
	OmnString 				mFileName;
	int						mPhysicalId;
	int64_t					mStartPos;	
	int64_t					mLength;
	int64_t					mFileLen;
	int64_t					mRecordLen;
	AosRundataPtr			mRundata;
	static u32              smReadSize;

public:
	//constructor
	AosFileReadRunner(
			const AosDataScannerObjPtr &scanner,
			const AosXmlTagPtr &def,
			const int64_t &start_pos,
			const int64_t &length,
			const int64_t &filelen,
			const AosRundataPtr &rdata);
	
	virtual bool run();
	virtual bool procFinished();

private :
	bool config(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata);

	bool recoverFile(
		const AosBuffPtr &buff,
		const u32 bytes_read,
		const int64_t &seekPos,
		const AosRundataPtr &rdata);
};
#endif
#endif
