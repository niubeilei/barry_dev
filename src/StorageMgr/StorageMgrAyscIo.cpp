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
// 	Created: 04/28/2013 by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "StorageMgr/StorageMgrAyscIo.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "StorageMgr/AioRequest.h"
#include "SEInterfaces/AioCaller.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"

OmnSingletonImpl(AosStorageMgrAyscIoSingleton, 
				 AosStorageMgrAyscIo, 
				 AosStorageMgrAyscIoSelf, 
				"AosStorageMgrAyscIo");

AosStorageMgrAyscIo::AosStorageMgrAyscIo()
:
mNumThrds(0),
mTotalReceivedSize(0)
{
	init();
}


AosStorageMgrAyscIo::~AosStorageMgrAyscIo()
{
}


bool
AosStorageMgrAyscIo::start()
{
	return true;
}


bool
AosStorageMgrAyscIo::stop()
{
	return true;
}

bool
AosStorageMgrAyscIo::config(const AosXmlTagPtr &config)
{
	return true;
}

void
AosStorageMgrAyscIo::init()
{
	AosXmlTagPtr config = OmnApp::getAppConfig();
	aos_assert(config);
	AosXmlTagPtr stm_cfg = config->getFirstChild(AOSCONFIG_STORAGEMGR);
	aos_assert(stm_cfg);

	AosXmlTagPtr dev_config = stm_cfg->getFirstChild(AOSCONFIG_DEVICE);
	aos_assert(dev_config);

	AosXmlTagPtr partition_config = dev_config->getFirstChild(AOSCONFIG_PARTITION);
	int crt_num = 0;
	while(partition_config)
	{
		OmnString userdir = partition_config->getAttrStr("userdir", "");
		if(userdir == "" ) 
		{
			partition_config = dev_config->getNextChild(AOSCONFIG_PARTITION);
			continue;
		}

		u32 len = userdir.length();
		if(userdir.data()[len-1] != '/')   userdir << "/";
		// Ketty 2013/05/28
		OmnString full_dir;
		if(userdir.data()[0] == '/')
		{
			full_dir = userdir;
		}
		else
		{
			full_dir = OmnApp::getAppBaseDir();
			full_dir << userdir;
		}
		//int64_t devid = OmnFile::getDeviceId(userdir);
		int64_t devid = OmnFile::getDeviceId(full_dir);
		if (devid == 1)
		{
			OmnAlarm << "devid == 1" << enderr;
		}
		mDeviceId.insert(make_pair(devid, crt_num));
		crt_num++;
		partition_config = dev_config->getNextChild(AOSCONFIG_PARTITION);
	}
	mDeviceId.insert(make_pair(1, crt_num++));
	mNumThrds = crt_num;

	mNumThrds++;
	OmnThreadedObjPtr thisptr(this, false); 
	for (int i = 0; i < mNumThrds; i++)
	{
		mThread[i] = OmnNew OmnThread(thisptr, "storageMgrAysclo", i, false, true, __FILE__, __LINE__);
		mLock[i] = OmnNew OmnMutex();
		mCondVar[i] = OmnNew OmnCondVar();
		mThread[i]->start(); 
	}
}


bool
AosStorageMgrAyscIo::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		u32 logicid = thread->getLogicId();
		mLock[logicid]->lock();
		if (mProcData[logicid].size() <= 0)
		{
			mCondVar[logicid]->wait(mLock[logicid]);
			mLock[logicid]->unlock();
			continue;
		}

		AosAioRequestPtr request = mProcData[logicid].top();
		aos_assert_r(request, false);
		mProcData[logicid].pop();

		mLock[logicid]->unlock();

		switch (request->mType)
		{
		case AosAioRequest::eAioReqRead:
		 	 readFileToBuff(request);
			 break;

		case AosAioRequest::eAioReqWrite:
			 writeFile(request);
			 break;
		default:
			 OmnAlarm << "Unrecognized  type " << enderr; 
		}

		for (int i = 0; i < mNumThrds; i++)
		{
			mLock[i]->lock();
			mCondVar[i]->broadcastSignal();
			mLock[i]->unlock();
		}
	}
	return true;
}


bool
AosStorageMgrAyscIo::signal(const int threadLogicId)
{
	return true;
}


bool
AosStorageMgrAyscIo::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosStorageMgrAyscIo::readFileToBuff(const AosAioRequestPtr &request)
{
	aos_assert_r(request, false);
	int64_t size = request->mSize;
	aos_assert_r(size >= 0, false);

	bool finished = false;
	AosBuffPtr buff = (request->mCaller)->getIoBuff(request->mReqId, size AosMemoryCheckerArgs);	

	if (size == 0)
	{
		buff->setDataLen(0);	
		finished = true;

		AosAioCaller::Parameter parm = {request->mReqId, size, finished, request->mOffset};
		(request->mCaller)->dataRead(parm, buff);

		return true;
	}

	int64_t offset = request->mOffset;
	aos_assert_r(offset >= 0, false);

	OmnLocalFilePtr file = request->mFile;
	aos_assert_r(file && file->isGood(), false);

	int64_t len = file->read(offset, buff->data(), size);
	if (len == -1)
	{
		OmnString errmsg = "len == -1";
		(request->mCaller)->readError(request->mReqId, errmsg);

		if (request->mFile->getDeviceId() != -1)
		__sync_fetch_and_sub(&mTotalReceivedSize, size);
		return true;
	}

	if (len > 0)
	{
		buff->setDataLen(len);
	}
	else
	{
		buff->setDataLen(0);
		//OmnAlarm << "len :" << len << enderr;
	}

	if (len < size) finished = true;

	AosAioCaller::Parameter parm = {request->mReqId, size, finished, request->mOffset};
	(request->mCaller)->dataRead(parm, buff);

	if (request->mFile->getDeviceId() != -1)
		__sync_fetch_and_sub(&mTotalReceivedSize, size);
	return true;
}


bool
AosStorageMgrAyscIo::writeFile(const AosAioRequestPtr &request)
{
	aos_assert_r(request, false);

	int64_t offset = request->mOffset;
	aos_assert_r(offset >= 0, false);

	int64_t size = request->mSize;
	aos_assert_r(size > 0, false);
	
	AosBuffPtr buff = (request->mCaller)->getIoBuff(request->mReqId, size AosMemoryCheckerArgs);
	aos_assert_r(buff && buff->dataLen() == size, false);

	OmnLocalFilePtr file = request->mFile;
	aos_assert_r(file && file->isGood(), false);

	bool rslt = file->write(offset, buff->data(), size, true);
	aos_assert_r(rslt, false);
	
	(request->mCaller)->dataWrite(request->mReqId);

	if (request->mFile->getDeviceId() != -1)
		__sync_fetch_and_sub(&mTotalReceivedSize, size);
	 return true;
}


bool
AosStorageMgrAyscIo::addRequest(const AosAioRequestPtr &request)
{
	int thrd_id = getDevId(request->mFile);
	aos_assert_r(thrd_id >= 0 && thrd_id < mNumThrds, false);
	aos_assert_r(request->mSize < eMaxMemory, false);
	while(1)
	{
		aos_assert_r(request->mSize >= 0, false);
		mLock[thrd_id]->lock();
		if (mTotalReceivedSize + request->mSize > eMaxMemory)
		{
			bool timeout;
			mCondVar[thrd_id]->timedWait(mLock[thrd_id], timeout, 30);
			// mLock[thrd_id]->unlock();
			// continue;              
		}
		if (request->mFile->getDeviceId() != -1)
			__sync_fetch_and_add(&mTotalReceivedSize, request->mSize);

		mProcData[thrd_id].push(request);
		mCondVar[thrd_id]->signal();
		mLock[thrd_id]->unlock();
		break;
	}
	return true;
}


bool
AosStorageMgrAyscIo::addRequest(const vector<AosAioRequestPtr> &request)
{
	for (u32 i = 0; i < request.size(); i++)
	{
		AosAioRequestPtr req = request[i];
		aos_assert_r(req, false);

		int thrd_id = getDevId(req->mFile);
		aos_assert_r(thrd_id >= 0 && thrd_id < mNumThrds, false);
		aos_assert_r(req->mSize < eMaxMemory, false);
		while(1)
		{
			aos_assert_r(req->mSize >= 0, false);
			mLock[thrd_id]->lock();
			if (mTotalReceivedSize + req->mSize > eMaxMemory)
			{
				bool timeout;
				mCondVar[thrd_id]->timedWait(mLock[thrd_id], timeout, 30);
				// mLock[thrd_id]->unlock();
				// continue;              
			}
			if (req->mFile->getDeviceId() != -1)
				__sync_fetch_and_add(&mTotalReceivedSize, req->mSize);

			mProcData[thrd_id].push(req);
			mCondVar[thrd_id]->signal();
			mLock[thrd_id]->unlock();
			break;
		}
	}
	return true;
}


int
AosStorageMgrAyscIo::getDevId(const OmnLocalFilePtr &file)
{
	aos_assert_r(file && file->isGood(), -1);
	int64_t dev_id = file->getDeviceId();		
	map<int64_t, int>::iterator itr = mDeviceId.find(dev_id);
	if (itr == mDeviceId.end())
		return mNumThrds-1; 
	return mDeviceId[dev_id];
}
