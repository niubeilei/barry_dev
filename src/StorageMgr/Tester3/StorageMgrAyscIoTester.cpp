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
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageMgr/Tester2/StorageMgrAyscIoTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/ActImportDoc.h"
#include "Debug/Debug.h"
#include "DocFileMgr/DocFileMgr.h"
#include "DocFileMgr/FileGroupMgr.h"
#include "DocFileMgr/StorageApp.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "TaskMgr/TaskData.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "StorageMgr/StorageMgr.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageMgr/Tester2/Ptrs.h"
#include "StorageMgr/StorageMgrAyscIo.h"
#include "StorageMgr/AioRequest.h"
#include "StorageMgr/Ptrs.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/VirtualFileObj.h"
#include "VirtualFile/VirtualFile.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"
#include "Util1/Time.h"
#include "Util/ReadFile.h"
#include "Thread/ThreadPool.h"
#include "SEInterfaces/VirtualFileObj.h"
#include "VirtualFile/VirtualFile.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>


OmnThreadPool sgThreadPool1("test");

AosStorageMgrAyscIoTester::AosStorageMgrAyscIoTester()
:
mCrtVidIdx(0),
mLock(OmnNew OmnMutex())
{
}


AosStorageMgrAyscIoTester::~AosStorageMgrAyscIoTester()
{
}


bool 
AosStorageMgrAyscIoTester::start()
{
	AosRundataPtr rdata = OmnApp::getRundata()->clone(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(100);
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	basicTest(rdata);
	return true;
}


bool
AosStorageMgrAyscIoTester::basicTest(const AosRundataPtr &rdata)
{
	int num_thrds = 5;
	for (int i = 0; i < num_thrds; i++)
	{
OmnScreen << "===============: " << i << endl;
		mVfs.push_back(0);
		mFileId.push_back(0);
		mSize.push_back(0);
		//createData(rdata);
		AosStorageMgrAyscIoTesterPtr thisptr(this, true);
		OmnThrdShellProcPtr req = OmnNew ReadData(rdata, i, thisptr, true);
		sgThreadPool1.proc(req);
	}

//	while (1)
//	{
///		int i;
//		for (i = 0; i< num_thrds; i++)
//		{
//			if (mSize[i] == 0) break;
//		}
//		if (i == num_thrds) break;
//	}
//
//	for (int i = 0; i< num_thrds; i++)
//	{
//		createVfsObj(i, rdata);
//	}
	return true;
}

void
AosStorageMgrAyscIoTester::createData(const int index, const AosRundataPtr &rdata)
{
	int64_t crt_last_offset = 0;
	OmnLocalFilePtr file = 0;
	int64_t check_num = 0;
	while(1)
	{
		AosBuffPtr buff = readyData(check_num);
		if (crt_last_offset + buff->dataLen() > eMaxFileSize) break;
		if (!file || crt_last_offset + buff->dataLen() > eMaxFileSize)
		{
			file = createFileLocked(rdata, crt_last_offset, index);      
			aos_assert(file);
		}

OmnScreen << " ================ write write write index :" << index << " , mCrtLastOffset: " << crt_last_offset << " , " << buff->dataLen() << endl;
		file->writelock();
		file->write(crt_last_offset, buff->data(), buff->dataLen());
		file->unlock();

		crt_last_offset += buff->dataLen();
	}
	mLock->lock();
	mSize[index] = crt_last_offset;
	mLock->unlock();
	createVfsObj(index, rdata);
}

OmnLocalFilePtr
AosStorageMgrAyscIoTester::createFileLocked(
		const AosRundataPtr &rdata,
		int64_t &crt_last_offset,
		const int &index) 
{
	OmnString fname_prefix = "temp";
	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);
	aos_assert_r(total_vids.size()>0 && mCrtVidIdx < (int)total_vids.size(), 0);
	u32 virtual_id = total_vids[mCrtVidIdx];
	mCrtVidIdx++;
	if(mCrtVidIdx == (int)total_vids.size())  mCrtVidIdx = 0;

	u64 file_id;
	virtual_id = 0;
	OmnLocalFilePtr file = AosVfsMgrObj::getVfsMgr()->createRaidFile(rdata, virtual_id, file_id, fname_prefix, eMaxFileSize, false);
	crt_last_offset = 0;
	mLock->lock();
	mFileId[index] = file_id;
	mLock->unlock();
	OmnScreen << "file_id: " << file_id << endl;
	return file;
}


bool
AosStorageMgrAyscIoTester::createVfsObj(const int idx, const AosRundataPtr &rdata)
{
	OmnScreen << " idx :" << idx << endl;
	AosVirtualFileObjPtr file = OmnNew AosVirtualFile(AosGetSelfServerId(), mFileId[idx]);
	mVfs[idx] = file;

	AosStorageMgrAyscIoTesterPtr thisptr(this, false);
	mVfs[idx]->setCaller(thisptr);

	int64_t len = (eMaxReadSize - sizeof(u64)) / sizeof(u64);
	len = len * sizeof(u64); 
	mVfs[idx]->readData(idx, len, rdata);
	return true;
}


bool
AosStorageMgrAyscIoTester::readData(const int idx, const AosRundataPtr &rdata)
{
	AosBuffPtr buff = mVfs[idx]->getBuff();
	aos_assert_r(buff, false);
OmnScreen << "call back :" << idx  << " , " << mVfs[idx]->getOffset() << " , " << buff->dataLen() << endl;
	checkData(buff, mVfs[idx]->getOffset());

	int64_t offset = 0;
	int64_t size = 0;
	if (rand()%10 == 0)
	{
OmnScreen  << " ============================================ create cread " << idx <<endl;
		AosVfsMgrObj::getVfsMgr()->removeFile(mFileId[idx], rdata);
		//AosVfsMgrObj::getVfsMgr()->openFile(mFileId[idx], rdata AosMemoryCheckerArgs);
		createData(idx, rdata);
	}

	offset = rand()% mSize[idx];
	if (offset % sizeof(u64) != 0)
	{
		offset = offset - (offset % sizeof(u64));
		aos_assert_r(offset %sizeof(u64) == 0, false);
	}

	size = rand()% eMaxReadSize;
	size = size > (mSize[idx] - offset)?(mSize[idx] - offset):size;
	size = ((size % sizeof(u64)) != 0)?(size - (size % sizeof(u64))): size;
	aos_assert_r(size < eMaxReadSize, false);
	mVfs[idx]->setOffset(offset);
	mVfs[idx]->readData(idx, size, rdata);
	//AosVfsMgrObj::getVfsMgr()->removeFile(mFileId[idx], rdata);
	return true;
}


bool
AosStorageMgrAyscIoTester::checkData(const AosBuffPtr &buff, const int64_t &offset)
{
	buff->reset();
	aos_assert_r(offset % sizeof(u64) == 0, false);
	u64 vv = offset / sizeof(u64);
	while(buff->getCrtIdx() < buff->dataLen())
	{
		u64 num = buff->getU64(0);
		aos_assert_r(vv == num, false);
		vv ++;
	}
	return true;
}


AosBuffPtr
AosStorageMgrAyscIoTester::readyData(int64_t &check_num)
{                                                                                   
	AosBuffPtr buff = OmnNew AosBuff(eMaxWriteSize, 0 AosMemoryCheckerArgs);
	while(1)
	{
		if (buff->dataLen() +  sizeof(u64) > eMaxWriteSize) break;
		buff->setU64(check_num);
		check_num ++;
	}
	return buff;
}


void 
AosStorageMgrAyscIoTester::fileReadCallBack(const u64 &reqId, const int64_t &expected_size, const bool &finished)
{
	AosStorageMgrAyscIoTesterPtr thisptr(this, true);
	OmnThrdShellProcPtr req = OmnNew ReadData(mRundata, reqId, thisptr);
	sgThreadPool1.proc(req);
}
