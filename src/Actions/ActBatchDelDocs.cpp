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
// 05/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActBatchDelDocs.h"

#include "IILUtil/IILId.h"
#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"
#include "API/AosApi.h"



AosActBatchDelDocs::AosActBatchDelDocs(const bool flag)
:
AosSdocAction(AOSACTTYPE_BATCHDELDOCS, AosActionType::eBatchDelDocs, flag),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosActBatchDelDocs::AosActBatchDelDocs(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_BATCHDELDOCS, AosActionType::eBatchDelDocs, false),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosActBatchDelDocs::~AosActBatchDelDocs()
{
}


AosActionObjPtr
AosActBatchDelDocs::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActBatchDelDocs(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosActBatchDelDocs::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return true;
}


bool 
AosActBatchDelDocs::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	aos_assert_rr(sdoc, rdata, false);
	if (!config(sdoc, rdata))
	{
		AosSetErrorU(rdata, "config error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mSizeId = sdoc->getAttrU64(AOSTAG_SIZEID, 0);

	mFlag = sdoc->getAttrBool("flag", false);
	
	AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_ACTIONS); 
	if (actions)
	{
		AosSdocAction::runActions(actions, rdata);	
	}

	OmnString mode = sdoc->getAttrStr(AOSTAG_RUNNING_MODE);
	if (mode == "thread")
	{
		// It should run it through a thread.
		if (mIsTemplate)
		{
			// The action is a template. It cannot run in the thread mode.
			AosSetErrorU(rdata, "cannot_run_thread_mode");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
		if (mThread)
		{
			// Should never happen
			AosSetErrorU(rdata, "internal_error");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		OmnThreadedObjPtr thisptr(this, false);
		mThread = OmnNew OmnThread(thisptr, "deleteiil", 0, false, true, __FILE__, __LINE__);
		mThread->start();
	}

	// It is not the thread mode. Just run it.
	return deleteDocs(true, rdata);	
}


bool	
AosActBatchDelDocs::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	mEvent = eNoEvent;
	aos_assert_r(mRundata, false);
	deleteDocs(false, mRundata);
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mEvent == eNoEvent)
		{
			mCondVar->wait(mLock);
		}

		switch (mEvent)
		{
		case eFinished:
			 state = OmnThrdStatus::eExit;
			 mLock->unlock();
			 return true;

		case eNoEvent:
			 break;

		default:
			 OmnAlarm << "Unrecognized event: " << mEvent << enderr;
			 mLock->unlock();
			 return false;
		}
		mEvent = eNoEvent;
		mLock->unlock();
	}

	return true;
}


bool
AosActBatchDelDocs::deleteDocs(const bool is_sync, const AosRundataPtr &rdata)
{
	// If it is sync, it will not return until it finishes the operation. 
	// Otherwise, it issues a request to delete the IIL and returns.
	//OmnNotImplementedYet;

	bool rslt = false; //AosDocClientObj::getDocClient()->deleteBatchDocs(rdata, mSizeId);
	aos_assert_rr(rslt, rdata, false);

	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	rslt = AosRemoveDataRecordBySizeId(siteid, mSizeId, mFlag, rdata);


	/*
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_an);
	query_context->setU64Value(0);
	query_context->setReverse(false);
	query_context->setBlockSize(100000);
	while(1)
	{
		AosQueryRsltObjPtr query_rslt = AosQueryRslt::getQueryRslt();
		query_rslt->setWithValues(true);
		bool rslt = AosQueryColumn(AOSIILID_SIZEIDS, query_rslt, 0, query_context, false, mRundata);
		aos_assert_r(rslt, false);
		while (1)
		{
			u64 docid;
			u64 key;
			bool finished;
			aos_assert_r(query_rslt->nextU64DocidValue(docid, key, finished, mRundata), false);
			if (finished) break;
OmnScreen << "delete sizeid: " << key << " format docid: " << docid << endl;
			aos_assert_r(AosDocClientObj::getDocClient()->deleteBatchDocs(rdata, (u32)key, true), false);
		}
		if (query_context->finished())
		{
			break;
		}
	}
	*/
	return true;
}


bool	
AosActBatchDelDocs::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool    
AosActBatchDelDocs::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


