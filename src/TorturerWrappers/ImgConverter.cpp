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
//
// Modification History:
// 09/30/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TorturerWrappers/ImgConverter.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataMgr/GroupDataMgr.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "XmlInterface/Server/MainUtil.h"


const char *sgFileTags = "yitsmlh";
const int sgFilesizes[] = {
	AosImgConverter::eTinySize, 
	AosImgConverter::eIconSize, 
	AosImgConverter::eThumbnailSize, 
	AosImgConverter::eSmallSize,
	AosImgConverter::eMediumSize,
	AosImgConverter::eLargeSize,
	AosImgConverter::eHugeSize};


AosImgConverter::AosImgConverter()
:
mHead(0),
mTail(0),
mIdle(0),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "Thrd", 0, true, true);
	mThread->start();
}


AosImgConverter::~AosImgConverter()
{
}


bool
AosImgConverter::stop()
{
	if(mThread.isNull())
	{
		return true;
	}
	mThread->stop();
	return true;
}


bool	
AosImgConverter::threadFunc(
		OmnThrdStatus::E &state, 
		const OmnThreadPtr &thread)
{
	Request *req;
    while (state == OmnThrdStatus::eActive)
    {
		mThreadStatus = true;
		mLock->lock();
		if (!mHead)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		req = mHead; 
		mHead = mHead->next;
		if (!mHead) mTail = 0;
		mLock->unlock();

		for (int i=0; i<eNumFlags; i++)
		{
			mThreadStatus = true;
			convertFile(i <= req->maxsize, 
				req->filetype,
				req->orig_fn, 
				sgFileTags[i],
				req->seqno,
				req->fileext, 
				req->ww, 
				req->hh, 
				sgFilesizes[i], 
				req->dirname);
		} 

		mLock->lock();
		req->next = mIdle;
		mIdle = req;
		mLock->unlock();
	}
	mLock->unlock();
	return true;
}


bool
AosImgConverter::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;	
}


bool    
AosImgConverter::checkThread(OmnString &err, const int thrdLogicId) const
{
	return mThreadStatus;
}


bool
AosImgConverter::addRequest(
		const int maxsize,
        const char filetype,
        const OmnString &orig_fn,
        const u64 seqno,
        const OmnString &fileext,
        const int ww,
        const int hh, 
		const OmnString &dirname) 
{       
	mLock->lock();
	Request *req = mIdle;
	if (mIdle) mIdle = mIdle->next;
	mLock->unlock();
	if (!req)
	{
		req = OmnNew Request();
		aos_assert_r(req, false);
	}

	req->maxsize = maxsize;
	req->filetype = filetype;
	req->orig_fn = orig_fn;
	req->seqno = seqno;
	req->fileext = fileext;
	req->ww = ww;
	req->hh = hh;
	req->dirname = dirname;
	req->next = 0;

	mLock->lock();
	if (!mHead)
	{
		mHead = req;
	}

	if (mTail)
	{
		mTail->next = req;
	}
	mTail = req;

	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosImgConverter::convertFile(
        const bool removeFlag,
        const char filetype,
        const OmnString &orig_fn,
        const char type,
        const u64 seqno,
        const OmnString &fileext,
        const int ww,
        const int hh, 
        const int fsize, 
		const OmnString &dirname)
{       
    OmnString fn = AosGetOpenLzHome();
	fn << "/images/";
    fn << dirname << filetype << type << seqno << fileext;

    bool fileExist = OmnFile::fileExist(fn);
    if (!removeFlag)
    {
        // If the file exists, need to remove it.
        if (!fileExist) 
        {
            return true;
        }
        OmnString cmd = "rm ";
        cmd << fn;
        system(cmd);
        return true;
    }
   
    // create thumbname:
    // 'convert <fn> -resize wwxhh <fn>
    if (fileExist)
    {
        return true;
    }
    
    OmnString cmd = "convert ";
    if (ww > hh)
    {
        cmd << orig_fn << " -resize " << fsize << " " << fn;
    }
    else
    {
        cmd << orig_fn << " -resize x" << fsize << " " << fn;
    }
OmnTrace << "command: " << cmd << endl;
    system(cmd);
    return true;
}   


