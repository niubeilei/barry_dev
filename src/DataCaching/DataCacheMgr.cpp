////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Cacheering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Cacheering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2013/01/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataCaching/DataCacheMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SingletonClass/SingletonImpl.cpp"


OmnSingletonImpl(AosDataCacheMgrSingleton,
                 AosDataCacheMgr,
                 AosDataCacheMgrSelf,
                "AosDataCacheMgr");


AosDataCacheMgr::AosDataCacheMgr()
{
}


AosDataCacheMgr::~AosDataCacheMgr()
{
}


bool
AosDataCacheMgr::start()
{
	return true;
}


bool
AosDataCacheMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosDataCacheMgr::stop()
{
    return true;
}


