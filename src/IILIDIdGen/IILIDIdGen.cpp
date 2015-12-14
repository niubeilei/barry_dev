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
#include "IILIDIdGen/IILIDIdGen.h"

#include "API/AosApiG.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "IILIDIdGen/U64IdGenForIIL.h"
#include "IdGen/IdGenMgr.h"
#include "IILIDIdGen/U64IdGenForIIL.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Docid.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "StorageMgr/SystemId.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"

static int sgMaxTries = 100000;

OmnSingletonImpl(AosIILIDIdGenSingleton,
				 AosIILIDIdGen,
				 AosIILIDIdGenSelf,
				"AosIILIDIdGen");

AosIILIDIdGen::AosIILIDIdGen()
:
mLock(OmnNew OmnMutex()),
mNumVirtuals(0),
mCrtid(0),
mMaxid(0),
mBlocksize(0)
{
	OmnScreen << "SeIdGenCreated" << endl;	
}


AosIILIDIdGen::~AosIILIDIdGen()
{
}


bool
AosIILIDIdGen::start()
{
	// createIdGens
	/*
	u64 docid = AOSSTORAGEDOCID_IILID_IDGEN;
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(AOS_SYS_SITEID);
	
	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);
	for(u32 i=0; i < total_vids.size(); i++)
	{
		u32 cube_id = total_vids[i];
	
		AosU64IdGenForIILPtr iil_idgen = AosU64IdGenForIIL::createNewIdGen(
			cube_id, docid, mCrtid, mMaxid, mBlocksize, rdata);
		
		mIILIdGens.insert(make_pair(cube_id, iil_idgen));
	}
	*/	
	return true;
}


bool
AosIILIDIdGen::stop()
{
	// Ketty 2013/02/27
	//OmnDelete [] mIILIdGens;
	return true;
}


bool
AosIILIDIdGen::config(const AosXmlTagPtr &gconfig)
{
	aos_assert_r(gconfig, false);
	
	AosXmlTagPtr config = gconfig->getFirstChild(AOSCONFIG_IILMGR);
	aos_assert_r(config, false);

	mNumVirtuals = AosGetNumCubes();
	aos_assert_r(mNumVirtuals > 0, false);
	
	// Ketty 2013/02/27
	//mIILIdGens = OmnNew AosU64IdGenForIILPtr[mNumVirtuals];
	// memset(mIILIdGens, 0, mNumVirtuals);	

	AosXmlTagPtr def = config->getFirstChild(AOSCONFIG_IDGENS);
	aos_assert_r(def, false);

	
	AosXmlTagPtr defs = def->getFirstChild("defs");
	aos_assert_r(defs, false);

	AosXmlTagPtr cfg = defs->getFirstChild("iilid");
	aos_assert_r(cfg, false);

	mCrtid = cfg->getAttrU64(AOSTAG_CRTID, 5000);
	mMaxid = cfg->getAttrU64("maxid", 1000000);
	mBlocksize = cfg->getAttrU32("blocksize", 1);

	return true;
}


u64 
AosIILIDIdGen::nextIILId(
		const u32 vid, 
		const AosRundataPtr &rdata)
{
	// This function assumes the caller ensured the function is thread
	// safe. For the time being, to ensure iilids are managed correctly,
	// we will check whether the iilid was used. If yes, it will raise
	// alarm and try the next one.
	
	aos_assert_r(vid < mNumVirtuals, 0);
	aos_assert_r(rdata->getSiteid(), false);		// Chen Ding, 07/11/2012

	// Ketty 2013/02/27
	//AosU64IdGenForIILPtr idgen = mIILIdGens[vid];
	AosU64IdGenForIILPtr idgen = getIILIdGen(vid, rdata);
	aos_assert_r(idgen, 0);
	
	int guard = sgMaxTries; 
	while (guard-- > 0)
	{
		u64 localid = idgen->getNextId(rdata);
		u64 iilid = localid * mNumVirtuals + vid;
		//OmnScreen << "Trying next iilid: " << iilid << endl;
		// if (!AosIIL::staticIsIILIDUsed(iilid, 100, rdata)) // Chen Ding, 07/11/2012
		//if (!AosIIL::staticIsIILIDUsed(iilid, rdata->getSiteid(), rdata)) // shawnqu
		if (!AosIILMgrObj::getIILMgr()->checkIsIILIDUsed(iilid, rdata->getSiteid(), rdata)) // shawnqu
		{
			//OmnScreen << "Retrieved: " << iilid << endl;
			// OmnScreen << "-----vid and localid:" << vid 
			// 	<< "," << localid << ":" << idgen.getPtr() 
			// 	<< ":" << rdata->getEvent() << endl;
			return iilid;
		}
		OmnScreen << "IILID used: " << iilid << endl;
	}
	OmnShouldNeverComeHere;
	return AOS_INVDID;
}


// Ketty 2013/03/25
/*
bool
AosIILIDIdGen::createIdGens(const AosXmlTagPtr &defs, const bool create_flag)
{
	//AosRundataPtr rdata = OmnApp::getRundata();
	//rdata->setSiteid(AOS_SYS_SITEID);	// Ketty 2012/03/19
	//if (create_flag)
	//{
	AosXmlTagPtr cfg = defs->getFirstChild("iilid");
	aos_assert_r(cfg, false);

	// Ketty 2013/02/27
	//u64 crtid = cfg->getAttrU64(AOSTAG_CRTID, 5000);
	//u64 maxid = cfg->getAttrU64("maxid", 1000000);
	//u32 blocksize = cfg->getAttrU32("blocksize", 1);
	mCrtid = cfg->getAttrU64(AOSTAG_CRTID, 5000);
	mMaxid = cfg->getAttrU64("maxid", 1000000);
	mBlocksize = cfg->getAttrU32("blocksize", 1);

	// Ketty 2013/01/27
	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);
	for(u32 i=0; i < total_vids.size(); i++)
	{
		// Ketty 2012/07/30
		//u64 docid = AOSSTORAGEDOCID_IILID_IDGEN + i;
		//mIILIdGens[i] = AosU64IdGenForIIL::createNewIdGen(docid, crtid, maxid, blocksize, rdata);
		u64 docid = AOSSTORAGEDOCID_IILID_IDGEN;
		u32 vid = total_vids[i];
		mIILIdGens[vid] = AosU64IdGenForIIL::createNewIdGen(vid, docid, mCrtid, mMaxid, mBlocksize, rdata);
	}
	//}

	return true;
}
*/

// Ketty 2013/02/27
AosU64IdGenForIILPtr
AosIILIDIdGen::getIILIdGen(const int virtual_id, const AosRundataPtr &rdata)
{
	aos_assert_r(virtual_id >= 0, 0);       
	AosU64IdGenForIILPtr iil_idgen; 
	
	mLock->lock();
	map<u32, AosU64IdGenForIILPtr>::iterator it = mIILIdGens.find(virtual_id);
	if(it != mIILIdGens.end())
	{
		iil_idgen = it->second;
		mLock->unlock();
		return iil_idgen;
	}
	
	// createIdGens
	u32 old_site_id = rdata->getSiteid();
	rdata->setSiteid(AOS_SYS_SITEID);
	u64 docid = AOSSTORAGEDOCID_IILID_IDGEN;
	iil_idgen = AosU64IdGenForIIL::createNewIdGen(
		virtual_id, docid, mCrtid, mMaxid, mBlocksize, rdata);
	mIILIdGens.insert(make_pair(virtual_id, iil_idgen));
	rdata->setSiteid(old_site_id);
	mLock->unlock();

	return iil_idgen; 
}


