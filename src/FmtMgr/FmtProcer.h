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
// 06/17/2013	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_FmtMgr_FmtProcer_h
#define AOS_FmtMgr_FmtProcer_h

#include "FmtMgr/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "StorageMgrUtil/FileInfo.h"
#include "SEUtil/SeConfig.h"
#include "TransUtil/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"

#include <deque>
#include <map>
using namespace std;


class AosFmtProcer: public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eProcThrdId,
	};

private:
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	OmnCondVarPtr	mStopCondVar;
	AosFmtFilePtr	mFmtFile;
	bool			mStoped;
	
	//u64			mFinishedFmtId;
	OmnThreadPtr	mProcThrd;
	deque<u32> 		mProcQueue;
	int				mCrtAddedReq;
	map<u64, u32> 	mProcedFileSeqs;
	
	bool			mShowLog;

public:
	AosFmtProcer(
		const AosFmtFilePtr &fmt_file,
		const bool show_log);
	~AosFmtProcer();
	
	// ThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };

	bool 	start();
	bool 	stop();

	bool 	addReq(const u32 seq);
	bool	recvFinishedFmtId(const u64 fmt_id);

private:
	bool 	procThrdFunc(
				OmnThrdStatus::E &state,
				const OmnThreadPtr &thread);

	int 	getNextProcSeq();

	bool 	procEachFmtFile(const u32 file_seq);
	bool 	procFmt(const AosFmtPtr &fmt);
	bool 	procCreateFile(
				const AosRundataPtr &rdata, 
				const AosFmtPtr &fmt);
	bool 	procModifyFile(
				const AosRundataPtr &rdata, 
				const AosFmtPtr &fmt);
	bool 	procDeleteFile(
				const AosRundataPtr &rdata, 
				const AosFmtPtr &fmt);
	bool 	procTransId(
				const AosRundataPtr &rdata, 
				const AosFmtPtr &fmt);
	
};
#endif
