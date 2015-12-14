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
// Modification History:
// 11/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BitmapMgr/BitmapMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "AppIdGens/IdNames.h"
#include "AppIdGens/SysIdGenU64.h"
#include "Porting/Sleep.h"
#include "BitmapMgr/Bitmap.h"
#include "BitmapMgr/BitmapThrdShellProc.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/RCObjImp.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosBitmapMgrSingleton,
                 AosBitmapMgr,
                 AosBitmapMgrSelf,
                "BitmapMgr");

OmnThreadPoolPtr 	AosBitmapMgr::smThreadPool = OmnNew OmnThreadPool("bitmap_mgr", __FILE__, __LINE__);
OmnMutexPtr     	AosBitmapMgr::smPoolLock  = OmnNew OmnMutex();


AosBitmapMgr::AosBitmapMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mLeafNodeListSize(0),
mBranchNodeListSize(0),
mBitmapListSize(0),
mBitmapLock(OmnNew OmnMutex()),
mNodeLock(OmnNew OmnMutex())
{
	// mBitmapIdGen = OmnNew AosSysIdGenU64(AOS_SYSTEMID_BITMAP_IDGEN, 0);
	// Chen Ding, 2013/10/21
}


AosBitmapMgr::~AosBitmapMgr()
{
}


bool
AosBitmapMgr::start()
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(AosGetDftSiteId());
	mDftBitmapIdBlockSize = 1000000ULL;
	// Chen Ding, 2014/03/26
	// mDftBitmapIdInitValue = 1000000ULL;
	mDftBitmapIdInitValue = 1000*1000*1000;
	mDftBitmapMaxId = 99999999999ULL;
	mDftBitmapidPrefix = 0ULL;
	
	
//	mBitmapIdGen = OmnNew AosSysIdGenU64(rdata, AOS_SYSTEMID_BITMAP_IDGEN, 
//				mDftBitmapIdBlockSize, mDftBitmapIdInitValue, 
//				mDftBitmapMaxId, mDftBitmapidPrefix);
	OmnThrdShellProcPtr runner;
	vector<OmnThrdShellProcPtr> runners;
	runner = OmnNew  AosBmpStartThrd(rdata);
	runners.push_back(runner);
	AosBitmapMgr::smThreadPool->procAsync(runners);

	return true;
}

void 
AosBitmapMgr::getIDGen(const AosRundataPtr &rdata)
{
	mLock->lock();
	mBitmapIdGen = OmnNew AosSysIdGenU64(rdata, AOS_SYSTEMID_BITMAP_IDGEN, 
				mDftBitmapIdBlockSize, mDftBitmapIdInitValue, 
				mDftBitmapMaxId, mDftBitmapidPrefix);
	OmnScreen << "BitmapIdGen created" << endl;
	mCondVar->signal();
	mLock->unlock();
}


bool		
AosBitmapMgr::config(const AosXmlTagPtr &def)
{
	AosBitmapObjPtr obj = getBitmap();
	AosBitmapObj::setObject(obj);

	for(int i = 0;i < 65536;i++)
    {
    	int j = i;
    	int k = 0;
    	while(j)
    	{
    		if((j&1) != 0)
    		{
    			break;
    		}
    		j = j >> 1;
    		k++;
    	} 
    	AosBitmapNode::smBitFindingList[i] = k;
    	
		int x = i;
		int y = -1;
		while(x)
		{
			x = x >> 1;
			y ++;
		}
		if(y>= 0)
		{
			AosBitmapNode::smBitRevFindingList[x] = y;
    	}		
    	AosBitmapNode::smBitCountList[i] = countU16Bit(i);
    }
	AosBitmapNode::smBitFindingList[0] = 0;
	AosBitmapNode::smBitRevFindingList[0] = 0;
			
	AosBitmap::smMasks[0] = 0xffffffffffffc000ULL;//14 bit mask
	AosBitmap::smMasks[1] = 0xffffffffffc00000ULL;//22 bit mask
	AosBitmap::smMasks[2] = 0xffffffffc0000000ULL;//30 bit mask
	AosBitmap::smMasks[3] = 0xffffffc000000000ULL;//38 bit mask
	AosBitmap::smMasks[4] = 0xffffc00000000000ULL;//46 bit mask
	AosBitmap::smMasks[5] = 0xffc0000000000000ULL;//54 bit mask
	AosBitmap::smMasks[6] = 0xc000000000000000ULL;//62 bit mask 11000.........
	AosBitmap::smMasks[7] = 0ULL;
			
	return true;
}


AosBitmapObjPtr 
AosBitmapMgr::getBitmap()
{
	mLock->lock();	
	if(mBitmaps.size() <= 0)
	{
		mLock->unlock();	
		return OmnNew AosBitmap();
	}
	AosBitmapObjPtr bitmap = mBitmaps.front();
	mBitmaps.pop();
	mLock->unlock();	
	return bitmap;
}


void 
AosBitmapMgr::returnBitmap(const AosBitmapObjPtr &bitmap)
{
	bitmap->clean();

	// Chen Ding, 2014/04/26
	/*
	mLock->lock();
	if (mBitmaps.size() > eMaxIdleRsltNode)
	{
		mLock->unlock();
		return;
	}
	mBitmaps.push(bitmap);
	mLock->unlock();
	*/
}


u64 
AosBitmapMgr::nextBitmapId(
		const AosRundataPtr &rdata, 
		const u64 iilid, 
		const int level)
{
	// Level 0 bitmaps use IILIDs as their bitmap IDs. 
	// Level 1 bitmaps are stored on the same machine on which its
	// IIL resides.
	//
	// For higher level bitmap IDs, they are not distributed. 
	//
	// Chen Ding, 2013/10/21
	/*
	if (!mBitmapIdGen)
	{
		// Chen Ding, 2013/10/21
mDftBitmapIdBlockSize = 1000000ULL;
mDftBitmapIdInitValue = 1000000ULL;
mDftBitmapMaxId = 99999999999ULL;
mDftBitmapidPrefix = 0ULL;
		mBitmapIdGen = OmnNew AosSysIdGenU64(rdata, AOS_SYSTEMID_BITMAP_IDGEN, 
				mDftBitmapIdBlockSize, mDftBitmapIdInitValue, 
				mDftBitmapMaxId, mDftBitmapidPrefix);
	}
	*/
	mLock->lock();
	if(!mBitmapIdGen)
	{
		mCondVar->wait(mLock);
		mLock->unlock();
	}
	else
	{
		mLock->unlock();
	}
	/*
	while(!mBitmapIdGen)
	{
		OmnScreen << "mBitmapIdGen Not Ready." << endl;
		OmnSleep(1);
	}*/

	aos_assert_rr(mBitmapIdGen, rdata, 0);

	if (level == 0)
	{
		return iilid;
	}

	if (level == 1)
	{
		// Chen Ding, 2013/05/06
		// Need to get the ID so that it is local to this machine.
		// return mBitmapIdGen->getNextId(rdata, AosGetRegionId(iilid));
	}

	return mBitmapIdGen->getNextId(rdata);
}

