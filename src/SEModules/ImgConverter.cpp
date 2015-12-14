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
#include "SEModules/ImgConverter.h"

#if 0
#include "alarm_c/alarm.h"

#include "Alarm/Alarm.h"
#include "DataMgr/GroupDataMgr.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "XmlInterface/Server/MainUtil.h"
#include "SEModules/ImgProc.h"
#include <ImageMagick/Magick++.h>
using namespace Magick;


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

}


AosImgConverter::~AosImgConverter()
{
}


bool
AosImgConverter::stop()
{
	mLock->lock();
	if(mThread.isNull())
	{
		return true;
	}
	mCondVar->signal();
	mLock->unlock();

	OmnScreen << "ImgConverter is stopping" << endl;
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
			bool timeout;
			static int sec = 30;
			mCondVar->timedWait(mLock, timeout, sec);
			if (timeout)
			{
				state = OmnThrdStatus::eExit;
				mThread = NULL;
			}
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
				req->dirname,
				req->dirSeqno);
		} 

		mLock->lock();
		req->next = mIdle;
		mIdle = req;
		mLock->unlock();
	}
	mLock->unlock();

	OmnScreen << "ImgConverter thread existing!" << endl;
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
		const OmnString &dirname,
		const int dirSeqno) 

{       
	mLock->lock();

	if (mThread)
	{
		OmnThreadedObjPtr thisPtr(this, false);
		mThread = OmnNew OmnThread(thisPtr, "ImgConvert", 0, true, true, __FILE__, __LINE__);
		mThread->start();
	}

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
	req->dirSeqno= dirSeqno;

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
		const OmnString &dirname,
		const int dirSeqno)
{       
    OmnString fn = dirname;
    fn << filetype << type << seqno << "." << fileext;

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
        system(cmd.data());
        return true;
    }
   
    // create thumbname:
    // 'convert <fn> -resize wwxhh <fn>
    if (fileExist)
    {
        return true;
    }
    
	Image *image = NULL;
	OmnString fsizestr;
	if (ww > hh)
	{
		try
		{
			image = OmnNew Image(orig_fn.data());
			image->resolutionUnits(PixelsPerInchResolution);
			fsizestr << fsize;
			image->sample(fsizestr.data());
			image->density("96x96");
			image->quality(60);
			//image->strip();
			image->write(fn.data());
		}
		catch(Magick::Exception &error)
		{
			OmnDelete image;
			cerr << "Caught Magick++ exception: " << error.what() << endl;
			return false;
		}
	}
	else
	{
		try
		{
			image = OmnNew Image(orig_fn.data());
			image->resolutionUnits(PixelsPerInchResolution);
			fsizestr << "x"<<fsize;
			image->sample(fsizestr.data());
			image->density("96x96");
			image->quality(60);
			//image->strip();
			image->write(fn.data());
		}
		catch(Magick::Exception &error)
		{
			OmnDelete image;
			cerr << "Caught Magick++ exception: " << error.what() << endl;
			return false;
		}
	}
	AosImgProc::addOneImage(dirSeqno);
	OmnDelete image;
    return true;
}   

#endif
