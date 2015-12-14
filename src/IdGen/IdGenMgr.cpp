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
#include "IdGen/IdGenMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenDef.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ObjMgrObj.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEUtil/DocTags.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/StrSplit.h"
#include "Util/FileWBack.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "SiteMgr/SyncServer.h"
#include "XmlUtil/XmlTag.h"

static OmnString sgFileFlag = "idgen_dcv163356adf8dgod5623dc5";
const OmnString sgFname = "idgen";

OmnSingletonImpl(AosIdGenMgrSingleton,
	AosIdGenMgr,
	AosIdGenMgrSelf,
	"AosIdGenMgr");


AosIdGenMgr::AosIdGenMgr()
:
mLock(OmnNew OmnMutex()),
mFname(sgFname)
{
}


AosIdGenMgr::~AosIdGenMgr()
{
}


bool
AosIdGenMgr::start()
{
	// Configure the IdGenMgr through the application configuration 
	// object 'OmnApp::getAppConfig()'
	aos_assert_r(OmnApp::getAppConfig(), false);

	AosXmlTagPtr def = OmnApp::getAppConfig()->getFirstChild(AOSCONFIG_IDGENS);
	aos_assert_r(def, false);

	try
	{
		mFile = OmnNew AosFileWBack(def);
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to create IDGEN: " << e.getErrmsg() << enderr;
		return false;
	}

	bool rslt = initIdGens();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIdGenMgr::stop()
{
	return true;
}


bool
AosIdGenMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


AosIdGenDefPtr 
AosIdGenMgr::getIdDef(const OmnString &name)
{
	mLock->lock();
	for (int i=0; i<mNumDefs; i++)
	{
		aos_assert_rl(mDefs[i], mLock, 0);
		if (mDefs[i]->getName() == name)
		{
			AosIdGenDefPtr def = mDefs[i];
			mLock->unlock();
			return def;
		}
	}
	mLock->unlock();
	return 0;
}


bool
AosIdGenMgr::initIdGens()
{
	// It initializes all the IdGens based on the config:
	// 	<...>
	// 		<AOSCONFIG_IDGENS>
	// 			<AOSCONFIG_DEFS>
	// 				<name .../>
	// 				<name .../>
	// 				...
	// 			</AOSCONFIG_DEFS>
	// 		</AOSCONFIG_IDGENS>
	// 		...
	// 	</...>
	//
	aos_assert_r(OmnApp::getAppConfig(), false);

	// Check it can initialize Ids, which is determined by the 
	// attribute AOSCONFIG_INIT_IDGENS and the password
	// must be AOSCONFIG_INIT_IDGENPWD == AOSPASSWD_INIT_IDGENS
	AosXmlTagPtr def = OmnApp::getAppConfig()->getFirstChild(AOSCONFIG_IDGENS);
	aos_assert_r(def, false);

	AosXmlTagPtr iddefs = def->getFirstChild(AOSCONFIG_DEFS);
	aos_assert_r(iddefs, false);

	if (def->getAttrStr(AOSCONFIG_INIT_IDGENS) != "true") 
	{
		return AosIdGenDef::readDefs(mFile, mDefs, mNumDefs);
	}

	aos_assert_r(def->getAttrStr(AOSCONFIG_PASSWD) == AOSPASSWD_INIT_IDGENS, false);
	def->removeAttr(AOSCONFIG_INIT_IDGENS);
	def->removeAttr(AOSCONFIG_PASSWD);

	// Initializing IdGens is allowed. Next, create the new files.
	if (!mFile) mFile = OmnNew AosFileWBack(def);
	aos_assert_r(mFile, false);
	mFile->resetToEmpty();

	AosXmlTagPtr iddef = iddefs->getFirstChild();
	mNumDefs = 0;
	while (iddef)
	{
		AosIdGenDefPtr dd = AosIdGenDef::createNewDef(iddef->getTagname(),
			mFile, iddef, mNumDefs);
		aos_assert_r(dd, false);
		mDefs[mNumDefs++] = dd;

		iddef = iddefs->getNextChild();
	}
	return true;
}


AosIdGenDefPtr
AosIdGenMgr::getIdDef(const AosXmlTagPtr &def)
{
	// The 'def' is in the format:
	// 	<def AOSCONFIG_INIT_IDGENS="true|false"
	// 		AOSCONFIG_PASSWD="xxx"
	// 		filename="xxx"
	// 		index="xxx"
	// 		...>
	// 	</def>
	// This function checks whether it is to initialize the idgen. If not, 
	// it reads in the idgen def. Otherwise, it creates an entry based on 'def'.
	aos_assert_r(def, 0);

	int idx = def->getAttrInt("index", -1);
	if (idx < 0) idx = 0;
	AosFileWBackPtr file = OmnNew AosFileWBack(def);
	if (!file->isGood())
	{
		OmnAlarm << "Failed creating the idgen file: " << def->toString() << enderr;
		return 0;
	}

	if (def->getAttrStr(AOSCONFIG_INIT_IDGENS) != "true") 
	{
		// It does not need to initialize the idgen. Read it.
		try
		{

			return OmnNew AosIdGenDef(file, idx);
		}

		catch (...)
		{
			OmnAlarm << "Failed creating idgen!" << def->toString() << enderr;
			return 0;
		}
		OmnShouldNeverComeHere;
		return 0;
	}

	aos_assert_r(def->getAttrStr(AOSCONFIG_PASSWD) == AOSPASSWD_INIT_IDGENS, 0);
	def->removeAttr(AOSCONFIG_INIT_IDGENS);
	def->removeAttr(AOSCONFIG_PASSWD);

	// Initializing IdGens is allowed. Next, create the new files.
	AosIdGenDefPtr dd = AosIdGenDef::createNewDef(def->getTagname(), file, def, idx);
	aos_assert_r(dd, 0);
	return dd;
}


/*
bool
AosIdGenMgr::getNextId(
		const OmnString &idgen_objid, 
		u64 &next_id,
		const u64 &init_value,
		const AosRundataPtr &rdata)
{
	// This function retrieves the next id from the ID Gen 'idgen_objid',
	// which is the objid of the ID gen.
	AosXmlTagPtr idgen_doc = AosDocClientObj::getDocClient()->getDocByObjid(idgen_objid, rdata);
	if (!idgen_doc)
	{
		AosSetError(rdata, AosErrmsgId::eIDGenNotDefined);
		OmnAlarm << rdata->getErrmsg() << ". Objid: " << idgen_objid << enderr;
		return false;
	}

	if (!AosSecurityMgrObj::getSecurityMgr()->checkModify(idgen_doc, rdata))
	{
		return false;
	}

	next_id = 0;
	bool rslt = AosIILClient::getSelf()->incrementDocid(AOSZTG_IDGEN_IIL_NAME,
			false, idgen_objid, next_id, 1, init_value, true, rdata);
	aos_assert_rr(rslt, rdata, false);
	return next_id;
}
*/


AosXmlTagPtr
AosIdGenMgr::createIdGen(
		const OmnString &objid, 
		const bool is_public,
		const u64 &init_value,
		const int blocksize, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	AosXmlTagPtr idgen_doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (idgen_doc)
	{
		mLock->unlock();
		return idgen_doc;
	}

	AosObjMgrObjPtr objmgr = AosObjMgrObj::getObjMgr();
	aos_assert_rl(objmgr, mLock, 0);
	idgen_doc = objmgr->createIdGenDoc(objid, is_public, rdata);
	mLock->unlock();

	if (!idgen_doc)
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << ". Failed creating IDGen Doc: " 
			<< objid << enderr;
		return idgen_doc;
	}
	return idgen_doc;
}

