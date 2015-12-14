////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 05/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataShuffler/DataShuffler.h"

#include "DataShuffler/DocidShufflerProc.h"
#include "DataShuffler/DistMapShufflerProc.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"

static AosDataShufflerPtr 	sgShuffler[AosDataShufflerType::eMax];
static OmnString		sgNames[AosDataShufflerType::eMax];
static OmnMutex			sgLock;
static OmnMutex			sgInitLock;
static bool				sgInited = false;

AosDataShuffler::AosDataShuffler(
		const OmnString &name, 
		const AosDataShufflerType::E type,
		const bool flag)
:
mType(type),
mNumServers(-1),
mNumThrds(-1)
{
	if (flag)
	{
		 AosDataShufflerPtr thisptr(this, false);   // bool false: true :  
		if (!registerShuffler(name, thisptr))
		{
			OmnThrowException("Failed registering");
			return;
		}
	}
}


AosDataShuffler::~AosDataShuffler()
{
}


bool
AosDataShuffler::registerShuffler(const OmnString &name, const AosDataShufflerPtr  &b)
{
	sgLock.lock();
	if (!AosDataShufflerType::isValid(b->mType))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect data shuffler type: ";
		errmsg << b->mType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgShuffler[b->mType])
	{
		sgLock.unlock();
		OmnString errmsg = "Proc already registered: ";
		errmsg << b->mType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgShuffler[b->mType] = b;
	AosDataShufflerType::addName(name, b->mType);
	sgLock.unlock();
	return true;
}


bool
AosDataShuffler::init(const AosRundataPtr &rdata)
{
	static AosDocidShuffler 		lsDocidShuffler(true);

	sgInitLock.lock();
	sgInited = true;
	AosDataShufflerType::check();
	sgInitLock.unlock();
	return true;
}

bool
AosDataShuffler::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	mNumServers = conf->getAttrInt("num_servers", 1);
	//mRunMultiThrd = conf->getAttrBool("run_multi_thrd", false);
	
	mDataCachers.clear();
	AosXmlTagPtr def = conf->getFirstChild("data_cacher");
	if (def)
	{
		def = def->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(def, false);
		for (int i = 0; i < mNumServers; i++)
		{
			def->setAttr("zky_numserver", i);
			AosDataCacherPtr dc = AosDataCacher::getDataCacher(def, rdata);
			mDataCachers.push_back(dc);
		}
		aos_assert_r(mDataCachers.size() == (u32)mNumServers, false);
	}
	return true;
}


AosDataShufflerPtr
AosDataShuffler::getDataShuffler(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(def, rdata, 0);
	OmnString typestr = def->getAttrStr(AOSTAG_TYPE);
	AosDataShufflerType::E type = AosDataShufflerType::toEnum(typestr);
	try
	{
		switch (type)
		{
		case AosDataShufflerType::eDocid:
			 return OmnNew AosDocidShuffler(def, rdata);

		case AosDataShufflerType::eDistMap:
			 return OmnNew AosDistMapShuffler(def, rdata);

		default:
			 AosSetErrorU(rdata, "invalid_shuffler_type") << ": " << def->toString() << enderr;
			 return 0;
		}
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

/*
bool
AosDataShuffler::sendFinish(const AosRundataPtr &rdata)
{
	aos_assert_r(mDataCachers.size() == (u32)mNumServers, false);
	for (int i = 0; i < mNumServers; i++)
	{
		AosDataCacherPtr datacacher = mDataCachers[i];
		aos_assert_r(datacacher, false);
		datacacher->sendFinish(rdata);
	}
	return true;
}


bool
AosDataShuffler::sendStart(const AosRundataPtr &rdata)
{
	aos_assert_r(mDataCachers.size() == (u32)mNumServers, false);
	for (int i = 0; i < mNumServers; i++)
	{
		AosDataCacherPtr datacacher = mDataCachers[i];
		aos_assert_r(datacacher, false);
		datacacher->sendStart(rdata);
	}
	return true;
}
*/

bool 
AosDataShuffler::appendEntry(const char *data, const int len, const AosRundataPtr &rdata)
{
	return true;
}

bool
AosDataShuffler::shufflerInThreads(
		const AosDataCacherPtr &cacher,
		vector<AosDataCacherPtr> &cachers,	
		const AosRundataPtr &rdata)
{
	aos_assert_r(cacher, false);
	aos_assert_r(mNumServers > 0 && cachers.size() == (u32)mNumServers, false); 

	AosDataBlobPtr blob = cacher->getBlob();
	mNumThrds = blob->size() % eMinSizeToRunInThreads;
	int64_t psize = blob->size()/mNumThrds; 
	int64_t start_idx = 0;
	AosDataShufflerPtr thisptr(this, false);
	vector<OmnThrdShellProcPtr> runners;

	for (int i = 0; i < mNumThrds; i++)
	{
		 OmnThrdShellProcPtr runner = OmnNew Shuffler(
                 thisptr, cacher, cachers, start_idx, psize, rdata);
         runners.push_back(runner);
         start_idx += psize;
	}
	return OmnThreadShellMgr::getSelf()->proc(runners);
}

bool
AosDataShuffler::Shuffler::run()
{
	for (u32 i=0; i<mDataCachers.size(); i++) aos_assert_rr(mDataCachers[i], mRundata, false);
	mDataShuffler->shuffle(mDataCacher, mDataCachers, mStartIdx, mNumEntries, mRundata); 
	return true;
}

bool
AosDataShuffler::Shuffler::procFinished()
{
	return true;
}

AosDataShufflerPtr
AosDataShuffler::getDistMapObj(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	return OmnNew AosDistMapShuffler(def, rdata);
}

void
AosDataShuffler::updateDistMap(const AosBuffArrayPtr &mm)
{
	aos_assert(mType == AosDataShufflerType::eDistMap);
	AosDataShufflerPtr thisptr(this, false);
	((AosDistMapShuffler *) (thisptr.getPtr()))->updateMap(mm);
}
