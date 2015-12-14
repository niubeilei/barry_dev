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
// 2013/06/18	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "TaskUtil/Recycle.h"

#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "SingletonClass/SingletonImpl.cpp"

OmnSingletonImpl(AosRecycleSingleton,
                 AosRecycle,
                 AosRecycleSelf,
                "AosRecycle");

AosRecycle::AosRecycle()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mThread(0)
{
	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "garbagerecycle", 0, false, true, __FILE__, __LINE__);
	mThread->start();
}


AosRecycle::~AosRecycle()
{
}


bool
AosRecycle::start()
{
	return true;
}


bool
AosRecycle::config(const AosXmlTagPtr &conf)
{
	return true;
}


bool
AosRecycle::stop()
{
	return true;
}

bool
AosRecycle::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mQueue.empty())
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		GReq req = mQueue.front();
		mQueue.pop(); 
		mLock->unlock();

		deleteFile(req.grinfo, req.rdata);
	}
	return true;
}


bool
AosRecycle::deleteFile(
		vector<RInfo> &grinfo,
		const AosRundataPtr &rdata)
{
	sort(grinfo.begin(), grinfo.end());	

	bool svr_death = false;
	for (u32 i = 0; i < grinfo.size(); i++)
	{
		int physicalid = grinfo[i].physicalid;
		u64 file_id = grinfo[i].file_id;
OmnScreen << "remove File physicalid: " << physicalid << " ; file_id: " << file_id << endl; 
		AosNetFileCltObj::deleteFileStatic(file_id, physicalid, svr_death, rdata.getPtr());
		if (svr_death)
		{
			diskRecycle(physicalid, file_id, rdata);
		}
	}
	return true;
}


bool
AosRecycle::signal(const int threadLogicId)
{
	return true;
}


bool
AosRecycle::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosRecycle::addRequest(
		vector<RInfo> &grinfo,
		const AosRundataPtr &rdata)
{
	GReq req;
	req.grinfo = grinfo;
	req.rdata = rdata->clone(AosMemoryCheckerArgsBegin);

	mLock->lock();
	mQueue.push(req);	
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosRecycle::diskRecycle(
		const int physicalid, 
		const u64 &file_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(physicalid >= 0, false);
	OmnString vv;
	vv << physicalid << "_" << file_id;
	return AosIILClientObj::getIILClient()->addStrValueDoc(AOSZTG_DISK_RECYCLE, vv, physicalid, true, false, rdata);
}


