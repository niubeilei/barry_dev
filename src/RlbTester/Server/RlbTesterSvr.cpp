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
// 2013/07/01	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "RlbTester/Server/RlbTesterSvr.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "SEInterfaces/FmtMgrObj.h"
#include "StorageMgrUtil/FileInfo.h"
#include "FmtMgr/Fmt.h"


AosRlbTesterSvr::AosRlbTesterSvr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar),
mShowLog(true)
{
}


AosRlbTesterSvr::~AosRlbTesterSvr()
{
}


bool 
AosRlbTesterSvr::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mSendFmtThrd = OmnNew OmnThread(thisPtr, "RlbTesterSvr_SendFmtThrd",
			eSendFmtThrdId, true, true, __FILE__, __LINE__);
	mSendFmtThrd->start();
	return true;
}


bool
AosRlbTesterSvr::threadFunc(
		  OmnThrdStatus::E &state,
		  const OmnThreadPtr &thread)
{
	u32 tid = thread->getLogicId();
	
	if(tid == eSendFmtThrdId)
	{
		return sendFmtThrdFunc(state, thread);
	}
	return true;
}


bool
AosRlbTesterSvr::sendFmtThrdFunc(
		  OmnThrdStatus::E &state,
		  const OmnThreadPtr &thread)
{
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert_r(fmt_mgr, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(1);

	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if(mFmts.size() == 0 || mCacheSize < eMaxCacheSize)
		{
			bool timeout = false;
			mCondVar->timedWait(mLock, timeout, 60);
			mLock->unlock();
			continue;
		}
		
		AosFmtPtr fmt = mFmts.front();
		mFmts.pop();
		
		AosBuffPtr fmt_buff = fmt->getFmtBuff();
		mCacheSize -= fmt_buff->dataLen();
		mLock->unlock();
		
		if(mShowLog)
		{
			OmnScreen << "RlbTesterSvr; sendFmt"
				<< "; fmt:" << fmt.getPtr() 
				<< endl;
		}

		fmt_mgr->sendFmt(rdata, fmt);
	}
	return true;
}


bool 
AosRlbTesterSvr::createFile(
		const AosRundataPtr &rdata,
		const AosTransId &trans_id,
		const u32 cube_id,
		const u32 site_id, 
		const u64 requested_size,
		u64 &file_id,
		OmnString &fname)
{
	AosVfsMgrObjPtr vfs = AosVfsMgrObj::getVfsMgr(); 
	aos_assert_r(vfs, false);
	
	OmnFilePtr file = vfs->createNewFile(rdata, cube_id,
			file_id, "RlbTest", requested_size, false);
	aos_assert_r(file && file->isGood(), false);

	OmnString full_fname = file->getFileName();
	fname = getPureFname(full_fname);

	AosFileInfo file_info;
	bool find;	
	vfs->getFileInfo(file_id, file_info, find, rdata);
	aos_assert_r(find, false);
	
	vector<AosTransId> trans_ids;
	trans_ids.push_back(trans_id);

	AosFmtPtr fmt = OmnNew AosFmt();
	fmt->add(file_info);
	fmt->add(trans_ids);
	
	if(mShowLog)
	{
		OmnScreen << "RlbTesterSvr; createFile"
			<< "; file_id:" << file_id
			<< "; trans_id:" << trans_id.toString()
			<< "; fmt:" << fmt.getPtr()
			<< endl;
	}

	
	mLock->lock();
	mFmts.push(fmt);
	mLock->unlock();
	return true;
}


OmnString
AosRlbTesterSvr::getPureFname(const OmnString &full_fname)
{
	int pos = full_fname.find('/', true);
	if(pos == -1)	return full_fname;

	OmnString pure_fname = full_fname.substr(pos+1);
	return pure_fname;
}


bool 
AosRlbTesterSvr::modifyFile(
		const AosRundataPtr &rdata,
		const AosTransId &trans_id,
		const u64 file_id,
		const u64 offset,
		const AosBuffPtr &data)
{
	AosVfsMgrObjPtr vfs = AosVfsMgrObj::getVfsMgr(); 
	aos_assert_r(vfs, false);
	
	OmnFilePtr file = vfs->openFile(file_id, rdata);
	aos_assert_r(file && file->isGood(), false);

	file->put(offset, data->data(), data->dataLen(), true);
	
	vector<AosTransId> trans_ids;
	trans_ids.push_back(trans_id);

	AosFmtPtr fmt = OmnNew AosFmt();
	fmt->add(file_id, offset, data->data(), data->dataLen(), true);
	fmt->add(trans_ids);

	if(mShowLog)
	{
		OmnScreen << "RlbTesterSvr; modifyFile"
			<< "; file_id:" << file_id
			<< "; offset:" << offset
			<< "; data_len:" << data->dataLen()
			<< "; trans_id:" << trans_id.toString()
			<< "; fmt:" << fmt.getPtr()
			<< endl;
	}

	mLock->lock();
	mFmts.push(fmt);
	AosBuffPtr fmt_buff = fmt->getFmtBuff();
	mCacheSize += fmt_buff->dataLen();
	if(mCacheSize >= eMaxCacheSize)
	{
		mCondVar->signal();
	}
	mLock->unlock();

	return true;
}


bool 
AosRlbTesterSvr::deleteFile(
		const AosRundataPtr &rdata,
		const AosTransId &trans_id,
		const u64 file_id)
{
	AosVfsMgrObjPtr vfs = AosVfsMgrObj::getVfsMgr(); 
	aos_assert_r(vfs, false);
	
	bool rslt = vfs->removeFile(file_id, rdata);
	aos_assert_r(rslt, false);

	vector<AosTransId> trans_ids;
	trans_ids.push_back(trans_id);

	AosFmtPtr fmt = OmnNew AosFmt();
	fmt->addDeleteFile(file_id);
	fmt->add(trans_ids);
	
	mLock->lock();
	mFmts.push(fmt);
	mLock->unlock();
	return true;
}


bool 
AosRlbTesterSvr::readData(
		const AosRundataPtr &rdata,
		const u64 file_id,
		const u64 offset,
		const u32 data_len,
		AosBuffPtr &data)
{
	AosVfsMgrObjPtr vfs = AosVfsMgrObj::getVfsMgr(); 
	aos_assert_r(vfs, false);
	
	OmnFilePtr file = vfs->openFile(file_id, rdata);
	aos_assert_r(file && file->isGood(), false);

	data = OmnNew AosBuff(data_len, 0 AosMemoryCheckerArgs);
	int bytes_read = file->readToBuff(offset, data_len, data->data());
	aos_assert_r((u32)bytes_read == data_len, false);
	data->setDataLen(data_len);
	
	if(mShowLog)
	{
		OmnScreen << "RlbTesterSvr; readData"
			<< "; file_id:" << file_id
			<< "; offset:" << offset
			<< "; data_len:" << data_len
			<< endl;
	}

	return true;
}


