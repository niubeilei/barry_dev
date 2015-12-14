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
// 05/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataBlob/DataBlob.h"

#include "API/AosApiG.h"
#include "DataRecord/DataRecord.h"
#include "DataBlob/BlobRecord.h"
#include "DataBlob/BlobVariable.h"
#include "DataBlob/FullHandlerType.h"
#include "DataBlob/FullHandlerActions.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataBlobObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/DataRecord.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/ThreadPool.h"


static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("datablob", __FILE__, __LINE__);


AosDataBlob::AosDataBlob(
		const OmnString &name, 
		const AosDataBlobType::E type)
:
mType(type),
mIsSorted(false),
mIsStable(false),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	// Ken Lee, 2013/01/09
	// Should we need thread ?
	//aos_assert(startThrds());
}


AosDataBlob::AosDataBlob(const AosDataBlob &rhs)
:
mType(rhs.mType),
mIsSorted(false),
mIsStable(rhs.mIsStable),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mFullActionsBeforeSort(rhs.mFullActionsBeforeSort),
mFullActionsAfterSort(rhs.mFullActionsAfterSort),
mAppendActions(rhs.mAppendActions),
mInvalidActions(rhs.mInvalidActions),
mRunFullActionsInBackground(rhs.mRunFullActionsInBackground),
mNeedToSortOnFull(rhs.mNeedToSortOnFull),
mFullHandler(rhs.mFullHandler),
mMaxNumRecords(rhs.mMaxNumRecords),
mMaxMemSize(rhs.mMaxMemSize),
mPageSize(rhs.mPageSize)
{
}


AosDataBlob::~AosDataBlob()
{
}


bool 
AosDataBlob::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	aos_assert_r(conf, false);
	mConfig = conf;

	mMaxNumRecords = conf->getAttrInt64(AOSTAG_MAX_RECORDS, -1);
	mMaxMemSize = conf->getAttrInt64(AOSTAG_MAX_SIZE, eMaxMemSize);
	
	mPageSize = conf->getAttrInt64(AOSTAG_PAGESIZE, -1);

	mRunInThrdShell = conf->getAttrBool(AOSTAG_RUN_INSHELL, false);
	mIsStable = conf->getAttrBool(AOSTAG_IS_STABLE, false);
	mRunFullActionsInBackground = conf->getAttrBool(AOSTAG_RFACTS_IN_BGD, true);
	mNeedToSortOnFull = conf->getAttrBool(AOSTAG_SORT_FULL, true);

	//  Init the mFinishActions.
	AosXmlTagPtr full_actions_before_sort = conf->getFirstChild("full_actions_before_sort");
	mFullActionsBeforeSort.clear();
	if (full_actions_before_sort)
	{
		AosXmlTagPtr act = full_actions_before_sort->getFirstChild();
		while (act)
		{
			// Chen Ding, 2013/06/29
			// AosActionObjPtr actobj = AosActionObj::createAction(act, rdata);
			AosActionObjPtr actobj = AosActionObj::getAction(act, rdata);
//			AosActionObjPtr actobj = OmnNew AosActTest(false);
			aos_assert_rr(actobj, rdata, false);
			mFullActionsBeforeSort.push_back(actobj);
			act = full_actions_before_sort->getNextChild();
		}
	}
	
	//  Init the mFinishActions.
	AosXmlTagPtr full_actions_after_sort = conf->getFirstChild("full_actions_after_sort");
	mFullActionsAfterSort.clear();
	if (full_actions_after_sort)
	{
		AosXmlTagPtr act = full_actions_after_sort->getFirstChild();
		while (act)
		{
			// Chen Ding, 2013/06/29
			// AosActionObjPtr actobj = AosActionObj::createAction(act, rdata);
			AosActionObjPtr actobj = AosActionObj::getAction(act, rdata);
//			AosActionObjPtr actobj = OmnNew AosActTest(false);
			aos_assert_rr(actobj, rdata, false);
			mFullActionsAfterSort.push_back(actobj);
			act = full_actions_after_sort->getNextChild();
		}
	}
	
	//  Init the mFinishActions.
	AosXmlTagPtr start_actions = conf->getFirstChild("start_actions");
	mStartActions.clear();
	if (start_actions)
	{
		AosXmlTagPtr act = start_actions->getFirstChild();
		while (act)
		{
			// Chen Ding, 2013/06/29
			// AosActionObjPtr actobj = AosActionObj::createAction(act, rdata);
			AosActionObjPtr actobj = AosActionObj::getAction(act, rdata);
//			AosActionObjPtr actobj = OmnNew AosActTest(false);
			aos_assert_rr(actobj, rdata, false);
			mStartActions.push_back(actobj);
			act = start_actions->getNextChild();
		}
	}
	
	//  Init the mFinishActions.
	AosXmlTagPtr finish_actions_before_sort = conf->getFirstChild("finish_actions_before_sort");
	mFinishActionsBeforeSort.clear();
	if (finish_actions_before_sort)
	{
		AosXmlTagPtr act = finish_actions_before_sort->getFirstChild();
		while (act)
		{
			// Chen Ding, 2013/06/29
			// AosActionObjPtr actobj = AosActionObj::createAction(act, rdata);
			AosActionObjPtr actobj = AosActionObj::getAction(act, rdata);
//			AosActionObjPtr actobj = OmnNew AosActTest(false);
			aos_assert_rr(actobj, rdata, false);
			mFinishActionsBeforeSort.push_back(actobj);
			act = finish_actions_before_sort->getNextChild();
		}
	}
	
	AosXmlTagPtr finish_actions_after_sort = conf->getFirstChild("finish_actions_after_sort");
	mFinishActionsAfterSort.clear();
	if (finish_actions_after_sort)
	{
		AosXmlTagPtr act = finish_actions_after_sort->getFirstChild();
		while (act)
		{
			// Chen Ding, 2013/06/29
			// AosActionObjPtr actobj = AosActionObj::createAction(act, rdata);
			AosActionObjPtr actobj = AosActionObj::getAction(act, rdata);
//			AosActionObjPtr actobj = OmnNew AosActTest(false);
			aos_assert_rr(actobj, rdata, false);
			mFinishActionsAfterSort.push_back(actobj);
			act = finish_actions_after_sort->getNextChild();
		}
	}
	//  Init the mAppendActions.
	AosXmlTagPtr append = conf->getFirstChild("append_actions");
	mAppendActions.clear();
	if (append)
	{
		AosXmlTagPtr act = append->getFirstChild();
		while (act)
		{
			// Chen Ding, 2013/06/29
			// AosActionObjPtr actobj = AosActionObj::createAction(act, rdata);
			AosActionObjPtr actobj = AosActionObj::getAction(act, rdata);
//			AosActionObjPtr actobj = OmnNew AosActTest(false);
			aos_assert_rr(actobj, rdata, false);
			mAppendActions.push_back(actobj);
			act = append->getNextChild();
		}
	}
	
	AosXmlTagPtr handler = conf->getFirstChild("handler");
	aos_assert_r(handler, false);
	mFullHandler = getFullHandler(handler, rdata);
	aos_assert_rr(mFullHandler, rdata, false);
	
	AosXmlTagPtr tag = conf->getFirstChild("record_def"); 
	if (tag)
	{
	    mDataRecord = AosDataRecordObj::createDataRecordStatic(tag, 0, rdata.getPtrNoLock() AosMemoryCheckerArgs);
	}
	
	mInvalidActions.clear();
	return true;
}


bool
AosDataBlob::startThrds()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "DataBlob", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}


bool
AosDataBlob::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mPendingReqs.empty())
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		AosRunActionsReq req = mPendingReqs.front();
		mPendingReqs.pop();
		mLock->unlock();
		processReq(req);
	}
	
	return true;
}


bool    
AosDataBlob::signal(const int threadLogicId)
{
	return true;
}


bool    
AosDataBlob::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosDataBlob::processReq(const AosRunActionsReq &req)
{
	// This function will run 'actions_before_sort' and 'actions_after_sort' 
	// in the order.
	
	AosBuffArrayPtr data = req.data;
	vector<AosActionObjPtr> actions_before_sort = req.actions_before_sort;
	vector<AosActionObjPtr> actions_after_sort = req.actions_after_sort;
	bool run_in_thrds_shell  = req.run_in_thrds_shell;
	AosRundataPtr rdata = req.rdata;
	if (size() <= 0) return true;
	if (actions_before_sort.size() <= 0 && actions_after_sort.size() <= 0) return true;
	
	if (run_in_thrds_shell) return runActionsInMultiThreads(data, actions_before_sort, actions_after_sort, rdata);
	
	return runActionsInCurrentThread(data, actions_before_sort, actions_after_sort, rdata);
}


bool
AosDataBlob::runActionsInBackground(
		const AosBuffArrayPtr &data,
		const vector<AosActionObjPtr> &actions_before_sort, 
		const vector<AosActionObjPtr> &actions_after_sort, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	AosRunActionsReq req(mRunInThrdShell, data, actions_before_sort, actions_after_sort, rdata);
	mPendingReqs.push(req);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


AosDataBlobFullHandlerPtr	
AosDataBlob::getFullHandler(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(def, rdata, 0);
	OmnString typestr = def->getAttrStr(AOSTAG_TYPE);
	AosFullHandlerType::E type = AosFullHandlerType::toEnum(typestr);
	try
	{
		switch (type)
		{
		case AosFullHandlerType::eActions:
			 return OmnNew AosFullHandlerActions(def, rdata);

		default:
			 AosSetErrorU(rdata, "invalid_full_handler_type") << ": " << def->toString() << enderr;
			 return 0;
		}
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}



AosDataBlobObjPtr
AosDataBlob::createDataBlob(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(def, rdata, 0);
	OmnString typestr = def->getAttrStr(AOSTAG_TYPE);
	AosDataBlobType::E type = AosDataBlobType::toEnum(typestr);
	aos_assert_r(AosDataBlobType::isValid(type), 0);
	try
	{
		switch (type)
		{
		case AosDataBlobType::eRecord:
			 return OmnNew AosBlobRecord(def, rdata);

		default:
			 AosSetErrorU(rdata, "invalid_blob_type") << enderr;
			 return 0;
		}
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

AosDataBlob::ActionRunner::~ActionRunner()
{
}

AosDataBlob::ActionRunner::ActionRunner(
		const AosDataBlobObjPtr &blob,
		const int64_t start_idx,
		const int64_t num_entries, 
		const vector<AosActionObjPtr> &actions,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("DataBlob"),
mBlob(blob),
mStartIdx(start_idx),
mNumEntries(num_entries),
mActions(actions),
mDataRecord(record),
mRundata(rdata)
{
}


bool
AosDataBlob::ActionRunner::run()
{
	/*
	if (mActions.size() == 0) return true;

	AosDataRecordObjPtr record = mDataRecord->clone(mRundata AosMemoryCheckerArgs);
	if (!mBlob->firstRecordInRange(mStartIdx, record, mRundata)) return true;
	int64_t num_records = 0;
	while (record && (num_records++ < mNumEntries || mNumEntries == -1))
	{
		for (u32 i=0; i<mActions.size(); i++)
		{
			aos_assert_r(mActions[i]->run(record, mRundata), false); 		
		}
		if (!mBlob->nextRecordInRange(record, mRundata)) return true;
	}
	return true;	
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosDataBlob::ActionRunner::procFinished()
{
	return true;
}


bool
AosDataBlob::runActionsInMultiThreads(
		const AosBuffArrayPtr &data,
		const vector<AosActionObjPtr> &actions_before_sort, 
		const vector<AosActionObjPtr> &actions_after_sort, 
		const AosRundataPtr &rdata)
{
	if (actions_before_sort.size() > 0)
	{
		// Run all the 'actions_before_sort' 
		runActionsInMultiThreads(data, actions_before_sort, rdata);
	}

	if (mNeedToSortOnFull || actions_after_sort.size() > 0)
	{	
		// clock_t start = clock();
		sort();
		// clock_t end = clock();
	}

	return runActionsInMultiThreads(data, actions_after_sort, rdata);
}


bool
AosDataBlob::runActionsInMultiThreads(
		const AosBuffArrayPtr &data,
		const vector<AosActionObjPtr> &actions, 
		const AosRundataPtr &rdata)
{
	// This function runs 'actions' through thread shells.
	if (size() <= 0) return true;
	u64 num_pages = determineNumPages(size(), rdata);
	aos_assert_rr(num_pages > 0, rdata, false);

	if (actions.size() <= 0) return true;

	AosDataBlobObjPtr thisptr(this, false);
	vector<OmnThrdShellProcPtr> runners;
	for (u64 i=0; i<num_pages; i++)
	{
		OmnThrdShellProcPtr runner;
		if (i == num_pages - 1)
		{
			runner = OmnNew ActionRunner(
				thisptr, i * mPageSize, -1, actions, mDataRecord, rdata);
		}
		else
		{
			runner = OmnNew ActionRunner(
				thisptr, i * mPageSize, mPageSize, actions, mDataRecord, rdata);
		}
		runners.push_back(runner);
	}
	
	aos_assert_r(sgThreadPool, false);
	return sgThreadPool->procSync(runners);
}


bool
AosDataBlob::runActionsInCurrentThread(
		const AosBuffArrayPtr &data,
		const vector<AosActionObjPtr> &actions_before_sort, 
		const vector<AosActionObjPtr> &actions_after_sort, 
		const AosRundataPtr &rdata)
{
	if (size() <= 0) return true;
	if (actions_before_sort.size() > 0)
	{
		aos_assert_r(runActions(data, actions_before_sort, rdata), false);
	}

	if (mNeedToSortOnFull || actions_after_sort.size() > 0)
	{
		sort();
	}
	
	return runActions(data, actions_after_sort, rdata);
}


bool 
AosDataBlob::runStartActionsInCurrentThread(
			const AosDataBlobObjPtr &blob,
			const vector<AosActionObjPtr> &start_actions, 
			const AosRundataPtr &rdata)
{
	/*
	aos_assert_r(blob, false);
	if (start_actions.size() <= 0) return true;
	
	for (u32 i=0; i<start_actions.size(); i++)
	{
		aos_assert_r(start_actions[i]->run(blob, rdata), false);
	}

	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosDataBlob::runActions(
		const AosBuffArrayPtr &data,
		const vector<AosActionObjPtr> &actions, 
		const AosRundataPtr &rdata)
{
	/*
	if (actions.size() <= 0) return true;

	aos_assert_r(data, false);
	aos_assert_r(mDataRecord, false);
	
	AosDataRecordObjPtr record = mDataRecord->clone(rdata AosMemoryCheckerArgs);
	record->clear();
	while (data->nextValue(record))
	{
		for (u32 i=0; i<actions.size(); i++)
		{
			aos_assert_r(actions[i]->run(record, rdata), false);
			record->clear();
		}
	}
	
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


u64
AosDataBlob::determineNumPages(const u64 &size, const AosRundataPtr &rdata)
{
	if (size <= mPageSize) return 1;
	if (mPageSize <= 0) mPageSize = eDftPageSize;
	aos_assert_rr(mPageSize > 0, rdata, 0);

	return size / mPageSize;
}


bool
AosDataBlob::serializeFrom(
		const AosBuffPtr &buff,           
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosDataBlob::serializeTo(
			const AosBuffPtr &buff, 
			const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosDataBlobObjPtr
AosDataBlob::createDataBlob(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	aos_assert_r(record, 0);
	return OmnNew AosBlobRecord(record, rdata);
}
	

AosDataBlobObjPtr
AosDataBlob::createDataBlob(
		const OmnString &sep,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sep != "", 0);
	return OmnNew AosBlobVariable(sep, rdata); 
}

	
AosDataRecordObjPtr
AosDataBlob::cloneDataRecord() const
{
	OmnNotImplementedYet;
	return 0;
}

