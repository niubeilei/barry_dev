//////////////////////////////////////////////////////////////////////////
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
// 01/01/2013: Created by Chen Ding
//////////////////////////////////////////////////////////////////////////
#if 0
#include "LogUtil/LogEntryCtnr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "LogUtil/LogEntry.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"



AosLogEntryCtnr::AosLogEntryCtnr()
{
}


AosLogEntryCtnr::~AosLogEntryCtnr()
{
}


bool 
AosLogEntryCtnr::appendEntry(const AosLogEntryPtr &entry)
{
	mEntries.push_back(entry);
	return true;
}


bool
AosLogEntryCtnr::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mEntries.size() > 0, rdata, false);
	for (u32 i=0; i<mEntries.size(); i++)
	{
		mEntries[i]->serializeTo(buff, rdata);
	}
	return true;
}


AosIILAssemblerPtr 
AosLogEntryCtnr::getContainerAttrAssm(const char *name)
{
	aos_assert_r(name, 0);
	mapitr_t itr = mContainerAttr.find(name);
	if (itr == mContainerAttr.end()) return 0;
	return itr->second;
}


AosIILAssemblerPtr 
AosLogEntryCtnr::getContainerKeywordAssm(const char *name)
{
	aos_assert_r(name, 0);
	mapitr_t itr = mContainerKeyword.find(name);
	if (itr == mContainerAttr.end()) return 0;
	return itr->second;
}


AosIILAssemblerPtr 
AosLogEntryCtnr::getContainerAttrKeywordAssm(const char *name)
{
	aos_assert_r(name, 0);
	mapitr_t itr = mContainerAttrKeyword.find(name);
	if (itr == mContainerAttr.end()) return 0;
	return itr->second;
}


AosIILAssemblerPtr 
AosLogEntryCtnr::getGlobalAttrAssm(const char *name)
{
	aos_assert_r(name, 0);
	mapitr_t itr = mGlobalAttr.find(name);
	if (itr == mContainerAttr.end()) return 0;
	return itr->second;
}


AosIILAssemblerPtr 
AosLogEntryCtnr::getGlobalKeywordAssm(const char *name)
{
	aos_assert_r(name, 0);
	mapitr_t itr = mGlobalKeyword.find(name);
	if (itr == mContainerAttr.end()) return 0;
	return itr->second;
}


AosIILAssemblerPtr 
AosLogEntryCtnr::getGlobalAttrKeywordAssm(const char *name)
{
	aos_assert_r(name, 0);
	mapitr_t itr = mGlobalAttrKeyword.find(name);
	if (itr == mContainerAttr.end()) return 0;
	return itr->second;
}

#endif
