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
// 10/01/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtilServer/SeIdGenMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "IILUtil/IILId.h"
#include "IdGen/U64IdGenWithIIL.h"
#include "IdGen/IdGenMgr.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEModules/SiteMgr.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SystemIdNames.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SiteMgr/SyncServer.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosSeIdGenMgrSingleton,
	AosSeIdGenMgr,
	AosSeIdGenMgrSelf,
	"AosSeIdGenMgr");

const OmnString sgDftCidPrefix = "";

AosSeIdGenMgr::AosSeIdGenMgr()
:
mStarted(false),
mLock(OmnNew OmnMutex())
{
	OmnScreen << "SeIdGenCreated" << endl;	
}


AosSeIdGenMgr::~AosSeIdGenMgr()
{
}


bool
AosSeIdGenMgr::start()
{
	mLock->lock();
	if (mStarted)
	{
		mLock->unlock();
		return true;
	}

	AosXmlTagPtr conf = OmnApp::getAppConfig();
	AosXmlTagPtr defs;
	if (conf)
	{
		AosXmlTagPtr def = OmnApp::getAppConfig()->getFirstChild(AOSCONFIG_IDGENS);
		if (def)
		{
			defs = def->getFirstChild("defs");
		}
	}

	try
	{
		// Chen Ding, 02/10/2012
		mCompIdGen = OmnNew AosU64IdGenWithIIL(defs, AOSIDGEN_COMPID, 
				AOSSYSTEMID_COMPID, AOSIILID_COMPID);

		mSsidGen = OmnNew AosU64IdGenWithIIL(defs, AOSIDGEN_SSID, 
				AOSSYSTEMID_SSID, AOSIILID_SSID);

		mCloudidGen = OmnNew AosU64IdGenWithIIL(defs, AOSIDGEN_CLOUDID, 
				AOSSYSTEMID_CLOUDID, AOSIILID_CLOUDID);
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to create ID GEN: " << e.getErrmsg() << enderr;
		mLock->unlock();
		return false;
	}

	mStarted = true;
	mLock->unlock();
	return true;
}


bool
AosSeIdGenMgr::stop()
{
	return true;
}


OmnString 
AosSeIdGenMgr::nextCid(const u32 siteid, const AosRundataPtr &rdata)
{
	// Chen Ding, 02/10/2012
	// This function retrieves the next Cloudid. Cloudids are stored in 
	// 		AOSIILID_CLOUDID + virtual ID
	if (!mStarted) start();
	aos_assert_rr(mCloudidGen, rdata, "");
	aos_assert_rr(siteid == rdata->getSiteid(), rdata, "");
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_rr(docclient, rdata, "");
	while (1)
	{
		u32 cc = mCloudidGen->nextId(siteid, rdata);
		OmnString cid = AosCloudid::composeCloudid(cc);
		AosXmlTagPtr doc = docclient->getDocByCloudid(cid, rdata);
		if (!doc)
		{
			return cid;
		}
	}

	OmnShouldNeverComeHere;
	return "";
}


u64	
AosSeIdGenMgr::nextSsid(const AosRundataPtr &rdata)
{
	if (!mStarted) start();
	aos_assert_rr(mSsidGen, rdata, 0);
	return mSsidGen->nextId(AosGetDftSiteId(), rdata);
}


u32
AosSeIdGenMgr::nextCompIdBlock()
{
	if (!mStarted) start();
	AosRundataPtr rdata = OmnApp::getRundata();
	aos_assert_rr(mCompIdGen, rdata, 0);
	u32 sid = rdata->setSiteid(AosGetDftSiteId());
	u32 cmpid = mCompIdGen->nextId(AosGetDftSiteId(), rdata);
	rdata->setSiteid(sid);
	return cmpid;
}

