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
// 04/11/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HtmlModules/ElemIdMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "SearchEngine/DocServer.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "SiteMgr/SyncServer.h"

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <dirent.h>
#include <map>

//static std::map<const char*, u32, Aosltstr> sgCrtIds; 
static std::map<u32, u32> sgCrtIds; 

OmnSingletonImpl(AosElemIdMgrSingleton,
	AosElemIdMgr,
	AosElemIdMgrSelf,
	"AosElemIdMgr");


extern AosXmlTagPtr gAosAppConfig;

AosElemIdMgr::AosElemIdMgr()
:
mLock(OmnNew OmnMutex()),
mBatchSize(eDftBatchSize),
mNumIdRemaining(0)
{
}


AosElemIdMgr::~AosElemIdMgr()
{
}


bool
AosElemIdMgr::start()
{
	mLock->lock();
	if (gAosAppConfig)
	{
		mDirname = gAosAppConfig->getAttrStr(AOSCONFIG_DIRNAME);
		AosXmlTagPtr conf = gAosAppConfig->getFirstChild(AOSCONFIG_ELEMIDMGR);
		if (!conf) return true;

		mBatchSize = conf->getAttrInt(AOSCONFIG_BATCHSIZE, eDftBatchSize);
	}

	mLock->unlock();
	return true;
}

bool
AosElemIdMgr::config(const AosXmlTagPtr &def)
{
	return true;
}

bool
AosElemIdMgr::stop()
{
	return true;
}


OmnString	
AosElemIdMgr::getNewElemId(const u32 siteid)
{
	aos_assert_r(siteid != 0, "");

	mLock->lock();
	u32 crtid = sgCrtIds[siteid];
	if (crtid == 0)
	{
		// Not retrieved from the server yet. Retrieve it now.
		// Mark it as being retrieved
		sgCrtIds[siteid] = eIsRetrieving;
		mLock->unlock();

		AosXmlTagPtr doc;
		OmnString objid = AOSOBJIDPRE_CRT_HTMLID;
		objid << "_" << siteid;
		bool rslt = AosSengAdmin::getSelf()->retrieveObj(siteid, 
				"", 0, AOSUSER_SYSTEM, AOSPASSWD_SYSTEM, objid, doc);
		mLock->lock();
		if (!rslt)
		{
			OmnAlarm << "No ID found in server: " << siteid << enderr;
			sgCrtIds[siteid] = eNotDefined;
			mCondVar->signal();
			mLock->unlock();
			return "";
		}

		// 'doc' should contain the attribute AOSTAG_CRTID
		u32 crtid = doc->getAttrU64(AOSTAG_CRTID, 0);
		if (crtid == 0)
		{
			OmnAlarm << "HTML ID Doc not correct: " << doc->toString() << enderr;
			sgCrtIds[siteid] = eNotDefined;
			mCondVar->signal();
			mLock->unlock();
			return "";
		}

		sgCrtIds[siteid] = crtid;
		mCondVar->signal();
	}

	u64 id = sgCrtIds[siteid]++;
	mLock->unlock();
	OmnString idstr;
	idstr << id;
	return idstr;
}

