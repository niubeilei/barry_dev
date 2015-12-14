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
#ifndef AOS_IILClientWrap_IILProcMgr_h
#define AOS_IILClientWrap_IILProcMgr_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"

#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/CondVar.h"
#include "Util/String.h"
#include "aosUtil/Types.h"
#include "IILClient/Ptrs.h"


#include <map>
using namespace std;

OmnDefineSingletonClass(AosIILProcMgrSingleton,
						AosIILProcMgr,
						AosIILProcMgrSelf,
						OmnSingletonObjId::eIILProcMgr,
						"IILProcMgr");


class AosIILProcMgr : virtual public OmnRCObject, virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eNumProcs = 10,
		eMemLen = 10000000, //10M
		eStartFlag = 0xa4bfd315,
		eEndFlag   = 0xa4bfd316,
		eMaxTrans = 10,
		eDftBatchFreq = 600,	// 10 minutes
		eDftNumTransPerBatch = 1000
	};

	OmnString			mDirName;
	OmnString			mArchivename;
	OmnString			mFileName;
	OmnFilePtr  		mTempFile;
	u32					mTransid;
	bool				mSaveFlag;

	OmnMutexPtr         mLock;
	OmnMutexPtr         mQueueLock;
	OmnMutexPtr         mProcLock;
	OmnCondVarPtr       mCondVar;
	OmnCondVarPtr       mCondVar2;
	OmnCondVarPtr       mCondVar3;
	OmnThreadPtr		mThread;

	AosIILProcPtr 		mMasterProc;
	AosIILProcPtr 		mProcs[eNumProcs];
	map<u32, bool> 		mTransMap;
	AosIILLogPtr		mLog;

	typedef pair<u32, u32> pairType;
	map<u64, pairType> mHead;
	u32					mBatchFreq;
	int					mNumTrans;
	u32					mLastSaveSec;
	int					mNumTransPerBatch;
	bool				mNeedToSwitch;

public:
	AosIILProcMgr();
	~AosIILProcMgr();


	//singleton
	static AosIILProcMgr*    getSelf();
	virtual bool        start(){return true;}
	virtual bool        stop();
	virtual OmnString   getSysObjName() const {return "AosIILProcMgr";}
	virtual OmnRslt     config(const OmnXmlParserPtr &def){return true;}
	virtual OmnSingletonObjId::E  getSysObjId() const
	{
		return OmnSingletonObjId::eIILProcMgr;
	}

	// OmnThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	
	bool	start(const AosXmlTagPtr &def);
	bool 	appendLog(const u64 &iilid, AosBuff &buff);
	bool 	saveLog();
	bool	creTempFile();
	bool	changeStats(int i);
	bool	chackStats();
	u32		getTransid(){return mTransid;}
	bool	signalSave(int i);
	bool	signalTrans();
	bool    timerThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool	docTransStarted(const u32 tid, const bool flag);
	bool	docTransFinished(const u32 tid);

};
#endif
