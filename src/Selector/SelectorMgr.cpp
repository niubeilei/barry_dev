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
// 2013/03/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Selector/SelectorMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SingletonClass/SingletonImpl.cpp"


OmnSingletonImpl(AosSelectorMgrSingleton,
                 AosSelectorMgr,
                 AosSelectorMgrSelf,
                "AosSelectorMgr");


AosSelectorMgr::AosSelectorMgr()
:
mLock(OmnNew OmnMutex()),
mCOndVar(OmnNew OmnCondVar())
{
}


AosSelectorMgr::~AosSelectorMgr()
{
}


bool
AosSelectorMgr::start()
{
	return true;
}


bool
AosSelectorMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosSelectorMgr::stop()
{
    return true;
}


bool
AosSelectorMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	Entry entry;
    while (state == OmnThrdStatus::eActive)
    {
		mLock->lock();
		if (mRequests.size() == 0)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		entry = mRequests.front();
		mRequests.pop();
		mLock->unlock();
		run(entry);
	}
	return true;
}


bool 
AosSelectorMgr::signal(const int threadLogicId)
{
	return true;
}


bool 
AosSelectorMgr::checkThread(OmnString &errmsg, const int tid) const
{
}

	
bool 
AosSelectorMgr::run(
		const AosRundataPtr &rdata, 
		const OmnString &query,
		const AosCappletPtr &capplet)
{
	aos_assert_rr(rdata, capplet, false);
	const char *data = query.data();
	int len = query.length();
	if (len < ePrefixLength)
	{
		capplet->cappletFailed(rdata, "invalid_query", query);
		return false;
	}

	Entry entry(rdata, query, capplet);
	mLock->lock();
	mRequests.push(entry);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosSelectorMgr::run(
		const AosRundataPtr &rdata, 
		const OmnString &query,
		const AosCappletPtr &capplet)
{
	const char *data = query.data();
	const int len = query.length();
	if (len < ePrefixLen)
	{
		capplet->failed(rdata, "invalid_query", query);
		return true;
	}

	switch (data[0])
	{
	case eDocid:
		 return procDocid(rdata, query, capplet);

	case eObjid:
		 return procObjid(rdata, query, capplet);

	case eContainer:
		 return procContainer(rdata, query, capplet);

	default:
		 OmnAlarm << "Query type not recognized: " << query << enderr;
		 capplet->cappletFailed(rdata, "unrecognized_type", query);
		 break;
	}

	return true;
}


bool
AosSelectorMgr::procDocid(
	const AosRundataPtr &rdata, 
	const OmnString &query, 
	const AosCappletPtr &capplet)
{
	const char *data = query.data();
	u64 docid = AosStr2Docid(&data[ePrefixLen], query.length()-ePrefixLen);
	AosDocClientCallerPtr caller = OmnNew AosDocClientCaller(rdata, query, capplet);
	bool rslt = AosGetDocByDocid(rdata, docid, caller);
	if (!rslt)
	{
		capplet->cappletFailed(rdata, rdata->getErrmsg(), query);
	}
	return true;
}


bool AosSelectorMgr::procObjid(
	const AosRundataPtr &rdata, 
	const OmnString &query, 
	const AosCappletPtr &capplet)
{
	const char *data = query.data();
	OmnString objid(&data[ePrefixLen]);
	AosDocClientCallerPtr caller = OmnNew AosDocClientCaller(rdata, query, capplet);
	bool rslt = AosGetDocByObjid(rdata, objid, caller);
	if (!rslt)
	{
		proc->failed(rdata, rdata->getErrmsg(), query);
	}
	return true;
}


bool
AosSelectorMgr::procContainer(
	const AosRundataPtr &rdata, 
	const OmnString &query, 
	const AosSelectorProcPtr &proc)
{
	const char *data = query.data();
	OmnString ctnr_objid(&data[ePrefixLen]);
	AosDocClientCallerPtr caller = OmnNew AosDocClientCaller(rdata, query, proc);
	bool rslt = AosGetDocByContainer(rdata, ctnr_objid, callback);
	if (!rslt)
	{
		proc->failed(rdata, rdata->getErrmsg(), query);
	}
	return true;
}

