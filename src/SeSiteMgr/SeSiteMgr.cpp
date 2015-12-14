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
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeSiteMgr/SeSiteMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "XmlUtil/XmlTag.h"



OmnSingletonImpl(AosSeSiteMgrSingleton,
                 AosSeSiteMgr,
                 AosSeSiteMgrSelf,
                "AosSeSiteMgr");

AosSeSiteMgr::AosSeSiteMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosSeSiteMgr::~AosSeSiteMgr()
{
}


bool
AosSeSiteMgr::start()
{
	return true;
}


bool
AosSeSiteMgr::config(const AosXmlTagPtr &config)
{
	mDftSite = AOSSITE_DEFAULT_SITEID;
	return true;
}


bool
AosSeSiteMgr::stop()
{
    return true;
}


bool
AosSeSiteMgr::resetSiteLoop()
{
	mSiteLoop = -1;
	return true;
}


OmnString 
AosSeSiteMgr::nextSite()
{
	OmnNotImplementedYet;
	return AOSSITE_DEFAULT_SITEID;
	// if (mSiteLoop == -1)
	// {
	// 	mSiteLoop++;
	// 	return "100";
	// }
}


