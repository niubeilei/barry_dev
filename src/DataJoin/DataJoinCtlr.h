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
// 05/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataJoin_DataJoinCtlr_h
#define Aos_DataJoin_DataJoinCtlr_h 

#include "Conds/Condition.h"
#include "Conds/Ptrs.h"
#include "DataAssembler/Ptrs.h"
#include "DataAssembler/DataAssembler.h"
#include "DataJoin/DataJoinListener.h"
#include "DataJoin/Ptrs.h"
#include "SEInterfaces/IILScannerObj.h"
#include "SEInterfaces/IILScannerListener.h"
#include "SEInterfaces/DataProcObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "Groupby/GroupbyType.h"
#include "Groupby/GroupbyProc.h"
#include "Groupby/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;

class AosDataJoinCtlr : virtual public OmnRCObject
{
	OmnDefineRCObject;

	u32							mStartTime;
	u32							mEndTime;
	int							mProgress;
	int							mPrevProgress;
	bool						mIsFinished;
	bool						mNeedSplit;
	OmnString					mProcKey;
	
	OmnMutexPtr					mLock;
	OmnMutexPtr					mCondLock;
	OmnCondVarPtr				mCondVar;
	AosTaskObjPtr				mTask;
	AosDataJoinListenerPtr		mListener;
	vector<AosDataJoinPtr>		mJoins;
	
	AosDataAssemblerObjPtr		mIILAssembler;
	AosDataAssemblerObjPtr		mDocAssembler;
	AosDataRecordObjPtr			mIILRecord;
	AosDataRecordObjPtr			mDocRecord;
	vector<AosConditionObjPtr>	mFilters;
	vector<AosDataProcObjPtr>	mDataProcs;
	map<OmnString, AosDataAssemblerObjPtr>	mIILAssemblers;

public:
	AosDataJoinCtlr(
		const AosXmlTagPtr &def,
		const AosDataJoinListenerPtr &listener,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata);
	~AosDataJoinCtlr();
	
	AosTaskObjPtr		getTask() const {return mTask;}
	AosDataAssemblerObjPtr getIILAssembler() const {return mIILAssembler;}
	AosDataAssemblerObjPtr getDocAssembler() const {return mDocAssembler;}
	vector<AosConditionObjPtr> & getFilters(){return mFilters;}
	
	AosDataRecordObjPtr	cloneIILRecord(const AosRundataPtr &rdata) const;
	AosDataRecordObjPtr	cloneDocRecord(const AosRundataPtr &rdata) const;
	vector<AosDataProcObjPtr> & getDataProcs() {return mDataProcs;}
	
	virtual bool 	start(const AosRundataPtr &rdata);
	virtual bool 	start(
						const AosTaskObjPtr &task, 
						const AosTaskDataObjPtr &task_data, 
						const AosRundataPtr &rdata);
	virtual bool	finish(
						const bool all_success,
						const OmnString &status_records,
						const AosRundataPtr &rdata);
	virtual bool	joinFinished(
						const AosDataJoinPtr &join,
						const AosRundataPtr &rdata);

	static AosDataJoinCtlrPtr createDataJoinCtlr(
						const AosXmlTagPtr &def,
						const AosDataJoinListenerPtr &listener,
						const AosTaskObjPtr &task,
						const AosRundataPtr &rdata);
	static bool		checkConfig(
						const AosXmlTagPtr &def,
						const AosTaskObjPtr &task,
						const AosRundataPtr &rdata);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	bool			sendStart(const AosRundataPtr &rdata);
	bool			sendFinish(const AosRundataPtr &rdata);
	bool			updateTaskProgress(const AosRundataPtr &rdata);
	bool			addThreadShellProc(
						const OmnThrdShellProcPtr &runner,
						const AosRundataPtr &rdata);
};
#endif

