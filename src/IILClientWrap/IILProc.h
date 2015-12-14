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
#ifndef AOS_IILClientWrap_IILProc_h
#define AOS_IILClientWrap_IILProc_h

#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "IILClient/Ptrs.h"
#include "IILClient/IILWrapUtil.h"
#include "IILMgr/Ptrs.h"
#include "IILUtil/IILLogType.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/String.h"
#include "Util/File.h"

#include <queue>
#include <list>
#include <map>
#include <hash_map>
using namespace std;

class AosIILProc : virtual public OmnThreadedObj
{
	OmnDefineRCObject;
	
private:
	enum Status
	{
		eStatusIdle,
		eStatusSwitched,
		eStatusProcessingFinished
	};

	enum
	{
		eHeadPoison = 0xffdab12c,
		eTailPoison = 0xaabbcc22,
		
		eHeadPosionOffset = 10,
		eStatusOffset = 14,
		eChangeIdOffset = 18,
		eTailPosionOffset = 22,
		eHeadSize = 16,
		eContentStart = 50,
		eUpdateLogEntrySize = 20,
		eIILWorkingHeadPos = 10,
		eWorkingContentsStart = 100,
		eIILWorkingHeadSize = 28,
		eIILWorkingContentsStart = 500,

		eWorkingFinishedOnly = 1325,
		eWorkingCurrent = 1264,
		eUpdateLogStart = 100
	};

	OmnMutexPtr			mLock;
	OmnCondVarPtr   	mCondVar;
	OmnThreadPtr		mThread;
	u32 				mSeqno;
	OmnString 			mDirname;
	OmnString			mArchivename;
	AosIILProcMapType  *mCurrentMap;
	AosIILProcMapType  *mFinishedMap;
	AosIILLogFilePtr	mFinishedLogs;
	AosIILLogFilePtr	mCurrentLogs;
	u32					mUpdatePos;
	bool				mProcessFinishedLogs;
	OmnFilePtr			mFinishedUpdateLogFile;
	OmnFilePtr			mCurrentUpdateLogFile;
	OmnFilePtr			mWorkingFile;
	u32					mProcessedTransid;
	AosIILProcMapType::iterator mFinishedItr;
	AosIILProcMapType::iterator mCurrentItr;
	Status				mStatus;

public:
	AosIILProc(
		const OmnString &dir, 
		const OmnString &archivename, 
		const u32 seqno);
	~AosIILProc();

	// OmnThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool 	appendLog(const u64 &iilid, AosBuff &buff);
	bool	switchLogs();
	bool	addMapEntry(
				const u64 &iilid, 
				const u32 crt_pos,
				const bool is_current);
	bool	docTransStarted(const u32 transid);
	bool	docTransFinished(const u32 transid);
	bool	isIILModified(const u64 &iilid);
	u64 	getNextMapEntry(u32 &tail, const bool is_current);
	bool	removeFinishedMapEntry(const u64 &iilid);
	bool 	readUpdateLogEntry(map<u64, u32>&themap, const OmnString &fname);
	bool 	resetMapLoop(const bool is_current);
	bool 	saveIIL(const AosIILPtr &iil, const u64 &tail, const bool is_current);

	OmnFilePtr resetWorkingFile()
	{
		mWorkingFile = OmnNew OmnFile(getIILWorkingFname(), OmnFile::eCreate);
		aos_assert_r(mWorkingFile->isGood(), 0);
		return mWorkingFile;
	}

private:
	OmnString 			readInitFilename();
	OmnString 			createFilename();
	AosIILLogFilePtr	getFirstBatch(std::list<OmnString> &fnames);
	bool 				readFilenames(std::list<OmnString> &fnames);
	bool 				restore(u32 &trans_id);
	bool 				cleanCurrentFile(u32 &trans_id);
	bool 				rebuildMap();
	OmnString			getUpdateLogFname();
	bool 				readUpdateLogEntry(map<u64, pair<u32, u32> >&themap, const OmnString &fname);
	bool				addEntry(const u64 &iilid, AosBuff &buff);

	OmnString getCurrentFname()
	{
		OmnString fname = mDirname;
		fname << "/current";
		return fname;
	}

	OmnString getFinishedFname()
	{
		OmnString fname = mDirname;
		fname << "/finished";
		return fname;
	}

	OmnString getFinishedUpdateLogFname()
	{
		OmnString fname = mDirname;
		fname << "/fsh_update_log";
		return fname;
	}

	OmnString getCrtUpdateLogFname()
	{
		OmnString fname = mDirname;
		fname << "/crt_update_log";
		return fname;
	}

	OmnString getIILWorkingFname()
	{
		OmnString fname = mDirname;
		fname << "/iil_temp";
		return fname;
	}

	bool	removeIILWorkingFile();
	bool	addFinishedUpdateLogEntry(const u64 &iilid, const u32 tail);
	bool	openUpdateLogFile();
	bool	updateMap(const u64 &iilid, const u32 tail, const bool isFinishedLog);
	bool	flushMemory();
	bool 	processIILWorkingFile();
	bool 	initFinishedLogFile();
	bool 	openFinishedUpdateLogFile();
	bool 	recoverIIL();
	bool 	recoverIIL(const OmnFilePtr &file);
	bool 	doesUpdateLogExist(
					const OmnString &fname, 
					const u64 &the_iilid, 
					const u32 the_tail);
	bool	archiveFinishedLogs();
	bool 	renameLogFile();
	bool 	processOneIIL(const u64 &iilid);
	bool 	openWorkingFile();
	bool 	openFinishedLog();
	bool 	readUpdateLogEntry(AosIILProcMapType &themap, const OmnString &fname);
};
#endif
