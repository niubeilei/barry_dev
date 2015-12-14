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
// 09/27/2010 	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtilServer/UserDocMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosUserDocMgrSingleton,
                 AosUserDocMgr,
                 AosUserDocMgrSelf,
                "AosUserDocMgr");



AosUserDocMgr::AosUserDocMgr()
{
}


AosUserDocMgr::~AosUserDocMgr()
{
}


bool      	
AosUserDocMgr::start()
{
	return true;
}


bool        
AosUserDocMgr::stop()
{
	return true;
}


bool    
AosUserDocMgr::signal(const int threadLogicId)
{
	return true;
}


bool    
AosUserDocMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosUserDocMgr::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		OmnSleep(100);
	}

	return true;
}


AosXmlTagPtr
AosUserDocMgr::getUserDoc(const u64 &docid, const AosRundataPtr &rdata)
{
	return AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
}


AosXmlTagPtr
AosUserDocMgr::getUserDoc(
		const u32 siteid, 
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{
	return AosDocClientObj::getDocClient()->getDocByCloudid(cid, rdata);
}

