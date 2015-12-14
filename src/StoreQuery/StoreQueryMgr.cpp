////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StoreQuery/StoreQueryMgr.h"
#include "StoreQuery/StoreQuery.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/OmnNew.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include <API/AosApiR.h>
#include <API/AosApiC.h>
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/CodeConvertion.h"

#include <iostream>
#include <algorithm>


OmnSingletonImpl(AosStoreQueryMgrSingleton, AosStoreQueryMgr, AosStoreQueryMgrSelf, "AosStoreQueryMgr");


AosStoreQueryMgr::AosStoreQueryMgr()
:
mQueryLock(OmnNew OmnMutex()),
mQueryCondVar(OmnNew OmnCondVar())
{
}

AosStoreQueryMgr::~AosStoreQueryMgr()
{
}

bool 
AosStoreQueryMgr::start()
{
    OmnThreadedObjPtr thisPtr(this, false); 
	if(!mQueryThrd)
	{
		mQueryThrd = OmnNew OmnThread(thisPtr, "StoreQueryThread", eQueryThrd, true, true, __FILE__, __LINE__);
	}
	mQueryThrd->start();
	return true;
}


bool 
AosStoreQueryMgr::stop()
{
	return true;
}

bool
AosStoreQueryMgr::signal(const int threadLogicId)
{
	mQueryLock->lock();
	mQueryCondVar->signal();
	mQueryLock->unlock();
	return true;
}

/*
bool    
AosStoreQueryMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return mThreadStatus;
}
*/

bool
AosStoreQueryMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mQueryLock->lock();
		
		if (mQueries.size() <= 0)
		{
			mQueryCondVar->wait(mQueryLock);
			mThreadStatus = true;
			mQueryLock->unlock();
			continue;
		}

		AosStoreQueryPtr query = mQueries.front();
		mQueries.pop();
		mQueryLock->unlock();
		procQuery(query);
	}
	return true;
}

bool 
AosStoreQueryMgr::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	return true;
}

bool 
AosStoreQueryMgr::procQuery(const AosStoreQueryPtr &query)
{
	query->runQuery();
	return true;
}

bool    
AosStoreQueryMgr::addQuery(const AosXmlTagPtr &querynode, const AosRundataPtr &rdata) 
{
	AosStoreQueryPtr query = OmnNew AosStoreQuery(querynode, rdata);
	aos_assert_r(query, false);
	mQueryLock->lock();
	mQueries.push(query);
	mQueryCondVar->signal();
	mQueryLock->unlock();
	OmnString rslts = "<Contents zky_result=\"wait\">";
	AosXmlTagPtr querydoc =  query->getQueryResultDoc(rdata);
	aos_assert_r(querydoc, false);
	rslts << querydoc->toString() << "</Contents>";
	rdata->setResults(rslts);
	return true;
}
