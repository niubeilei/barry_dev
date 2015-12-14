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
// 06/15/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/SeConfigMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "SiteMgr/SyncServer.h"


OmnSingletonImpl(AosSeConfigMgrSingleton,
	AosSeConfigMgr,
	AosSeConfigMgrSelf,
	"AosSeConfigMgr");

static OmnString sgDftHomeContainer = "zky_desktop";
static OmnString sgNoOwnerGrpFlag = "-";
static OmnString sgNoOwnerRoleFlag = "-";


AosSeConfigMgr::AosSeConfigMgr()
{
}


AosSeConfigMgr::~AosSeConfigMgr()
{
}


bool
AosSeConfigMgr::start()
{
	return true;
}


bool
AosSeConfigMgr::config(const AosXmlTagPtr &def)
{
	// Configure the SeConfigMgr through the application configuration 
	// object 'OmnApp::getAppConfig()'
	aos_assert_r(def, false);	
	mDftHomeCtnr = def->getAttrStr(AOSCONFIG_DFT_HOMECTNR, sgDftHomeContainer);
	mNoOwnerGrpFlag = def->getAttrStr(AOSCONFIG_NoOwnerGrpFlag, sgNoOwnerGrpFlag);
	mNoOwnerRoleFlag = def->getAttrStr(AOSCONFIG_NoOwnerRoleFlag, sgNoOwnerRoleFlag);
	return true;
}


bool
AosSeConfigMgr::stop()
{
	return true;
}

