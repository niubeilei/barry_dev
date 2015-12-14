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
// 2013/03/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILCache/DataPickerProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SingletonClass/SingletonImpl.cpp"


AosDataPickerProc::AosDataPickerProc(
		const AosRundataPtr &rdata, 
		const OmnString &query, 
		const AosDataPickerProcPtr &proc)
:
mRundata(rdata),
mQuery(query),
mProc(proc)
{
}


AosDataPickerProc::~AosDataPickerProc()
{
}


bool
AosDataPickerProc::start()
{
	return true;
}


bool
AosDataPickerProc::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosDataPickerProc::stop()
{
    return true;
}


bool
AosDataPickerProc::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
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
AosDataPickerProc::signal(const int threadLogicId)
{
	return true;
}


bool 
AosDataPickerProc::checkThread(OmnString &errmsg, const int tid) const
{
}

	
bool 
AosDataPickerProc::run(
		const AosRundataPtr &rdata, 
		const OmnString &query,
		const AosDQProcPtr &proc)
{
}


bool
AosDataPickerProc::run(
		const AosRundataPtr &rdata, 
		const OmnString &query,
		const AosDataPickerProcPtr &proc)
{
	const char *data = query.data();
	const int len = query.length();
	if (len < ePrefixLen)
	{
		proc->failed(rdata, "invalid_query", query);
		return true;
	}

	switch (data[0])
	{
	case eDocid:
		 return procDocid(rdata, query, proc);

	case eObjid:
		 return procObjid(rdata, query, proc);

	case eContainer:
		 return procContainer(rdata, query, proc);

	default:
		 OmnAlarm << "Query type not recognized: " << query << enderr;
		 proc->failed(rdata, "unrecognized_type", query);
		 break;
	}

	return true;
}


bool
AosDataPickerProc::procDocid(
	const AosRundataPtr &rdata, 
	const OmnString &query, 
	const AosDataPickerProcPtr &proc)
{
	const char *data = query.data();
	u64 docid = AosStr2Docid(&data[ePrefixLen], query.length()-ePrefixLen);
	AosDocClientCallerPtr caller = OmnNew AosDocClientCaller(rdata, query, proc);
	bool rslt = AosGetDocByDocid(rdata, docid, caller);
	if (!rslt)
	{
		proc->failed(rdata, rdata->getErrmsg(), query);
	}
	return true;
}


bool AosDataPickerProc::procObjid(
	const AosRundataPtr &rdata, 
	const OmnString &query, 
	const AosDataPickerProcPtr &proc)
{
	const char *data = query.data();
	OmnString objid(&data[ePrefixLen]);
	AosDocClientCallerPtr caller = OmnNew AosDocClientCaller(rdata, query, proc);
	bool rslt = AosGetDocByObjid(rdata, objid, caller);
	if (!rslt)
	{
		proc->failed(rdata, rdata->getErrmsg(), query);
	}
	return true;
}


bool
AosDataPickerProc::procContainer(
	const AosRundataPtr &rdata, 
	const OmnString &query, 
	const AosDataPickerProcPtr &proc)
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


