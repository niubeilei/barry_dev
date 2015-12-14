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
//	This class is used to manage a group of files, used to store 
//	either IILs or Documents. There are a number of files in the
//  group, each with a unique sequence number. Each file can 
//  store up to a given amount of data. 
//	
//	All documents are stored in 'mTransFilename' + seqno. There is 
//	a Document Index, that is stored in files 'mTransFilename' + 
//		'Idx_' + seqno
//
// Modification History:
// 12/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILClientWrap_IILLogFile_h
#define AOS_IILClientWrap_IILLogFile_h

#include "aosUtil/Types.h"
#include "IILClient/Ptrs.h"
#include "IILClient/IILWrapUtil.h"
#include "IILUtil/IILLogType.h"
#include "IILUtil/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/File.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"

#include <map>
#include <hash_map>
using namespace std;

typedef map<u64, pair<u32, u32> > aos_update_map_t; 

class AosIILLogFile : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eLogStatusBlockOffset = 10,

		eEntryHeadSize = 12,
		eLogStatusBlockSize = 12,
		eHeaderEntrySize = 20,		// u64 + u32 + u64

		eNumEntriesPerBatch = 10000,
		eMemorySize = 100000,
		eHeadPoison = 0xab12c312,
		eTailPoison = 0x12c354ac,
		eEndLogSize = 16,
	
		eStatusActive = 0x42643642,
		
		eDocStartLog = 15,  //An doc
		eIILLog = 16,
		eDocEndLog = 17,
		
		eLogNum = 10,
		eBodySize = 10000,
		eMemSize = 5000,
		eStatusOffset = 10,
		eHeadContentsStart = 100,
		eBodyContentsStart = 100
	};

private:
	OmnMutexPtr     	mAddLogLock;
	OmnMutexPtr     	mProcLogLock;
	OmnMutexPtr     	mLock;
	OmnString 			mDirname;
	OmnString			mFname;
	OmnFilePtr          mLogFile;
	u32					mTail;
	char *				mMemory;
	u32					mCrtFilePos;
	u32					mMemoryIdx;
	OmnFilePtr          mHeadFile;
	AosIILLogBuffPtr	mLogBuff;

public:
	AosIILLogFile(const OmnString &dirname,
				  const OmnString &fname,
				  const bool readonly);
	~AosIILLogFile();

	bool		saveToFile();
	bool 		processLogs(const AosIILProcPtr &proc, const bool is_current);
	bool 		rebuildMap(
					const AosIILProcPtr &iilproc,
					hash_map<u64, u32, AosU64Hash1> &update_map, 
					u32 &next_transid,
					const bool is_current_log);
	u32			getCrtPos(){return mCrtFilePos;}
	bool 		isReadyToProcess();
	bool isFileGood() const
	{
		if (!mLogFile) return false;
		return mLogFile->isGood();
	}
	bool addEntry(const u32 previous, u32 &tail, AosBuff &buff);
	bool processOneIIL(
			const AosIILProcPtr &proc, 
			const u64 &iilid, 
			const u32 tail, 
			const bool is_current);

private:
	OmnFilePtr  openFile();
	bool setStatus(const bool status);

	bool flushMemory();
	OmnString getCurrentFname()
	{
		OmnString fname = mDirname;
		fname << "/current";
		return fname;
	}
	
	OmnString getCrtIILUpdateLogFname()
	{
		OmnString fname = mDirname;
		fname << "/crt_update_log";
		return fname;
	}

	OmnString getFinishedFname()
	{
		OmnString fname = mDirname;
		fname << "/finished";
		return fname;
	}

	OmnString getFinishedIILUpdateLogFname()
	{
		OmnString fname = mDirname;
		fname << "/fsh_update_log";
		return fname;
	}

	OmnFilePtr openFile(const OmnString &fname)
	{
		OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eReadWrite);
		aos_assert_r(ff, ff);
		if (!ff->isGood())
		{
			ff = OmnNew OmnFile(fname, OmnFile::eCreate);
			aos_assert_r(ff->isGood(), 0);
		}
		return ff;
	}

	void getLogType(u32 &size, u32 &log_type)
	{
		log_type = (size >> 24);
		size &= 0x00ffffff;
	}


public:
	bool addDocStartLog(const u32 trans_id);
	bool addDocEndLog(const u32 trans_id);
	bool mergeContents(const AosIILLogFilePtr &rhs);
	bool removeWorkingFile();
	bool closeFile()
	{
		mLock->lock();
		aos_assert_rl(mLogFile, mLock, false);
		mLogFile->closeFile();
		mLogFile = 0;
		mLock->unlock();
		return true;
	}
};
#endif
