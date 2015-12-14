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
// 07/28/2011 Created by Ketty 
// 03/03/2012 Moved to VirtualFileSys.cpp
// 07/30/2012 Moved from VirtualFileSys.cpp	by Ketty.
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/StorageApp.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "ReliableFile/ReliableFile.h"
#include "XmlUtil/XmlTag.h"
#include "DocFileMgr/DocFileMgr.h"
#include "StorageMgr/FileKeys.h"
#include "StorageMgr/SystemId.h"
#include "StorageMgr/VirtualFileSys.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/File.h"

AosStorageApp::AosStorageApp(const AosVirtualFileSysPtr &vfs)
:
mLock(OmnNew OmnMutex()),
//mVirtualId(virtual_id),
mVfs(vfs)
{
}


AosStorageApp::~AosStorageApp()
{
}


bool
AosStorageApp::init()
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);              
	rdata->setSiteid(AOS_SYS_SITEID);

	u64 file_id = ((u64)mVfs->getVirtualId() << 32) + AOSFILEID_STORAGEAPP;
	mAppFile = mVfs->openRlbFile(rdata.getPtr(), file_id, 
						"app", eFileSize, true, AosRlbFileType::eNormal);
	aos_assert_r(mAppFile, false);
	mAppFile->setUseGlobalFmt(true);

	return true;	
}


void
AosStorageApp::stop()
{
}


bool
AosStorageApp::getDfmId(
		const AosRundataPtr &rdata,
		const AosModuleId::E module_id,
		u32 &dfm_id)
{
	aos_assert_r(rdata && module_id < AosModuleId::eMax, 0);
	u32 siteid = rdata->getSiteid();
	dfm_id = 0;

	AosBuffPtr module_buff;
	bool rslt = getModuleBuff(rdata, module_id, module_buff);
	aos_assert_r(rslt, false);	
	if(!module_buff)	return true;

	module_buff->setCrtIdx(0);
	u32 crt_rcd_num = module_buff->getU32(0);
	for(u32 i=0; i<crt_rcd_num; i++)
	{
		u64 id = module_buff->getU32(0);        // site_id
		if(id != siteid)
		{
			module_buff->getU32(0);             // dfm_id
			continue;
		}
				    
		dfm_id = module_buff->getU32(0);
		aos_assert_r(dfm_id != 0, false);
		return true;
	}
	
	// This means not find.
	//OmnScreen << " not find the dfmid:" << module_id << "; "
	//      << " site_id:" << site_id << "; "
	return true;
}


bool
AosStorageApp::addDfmId(
		const AosRundataPtr &rdata,
		const AosModuleId::E module_id,
		const u32 dfm_id)
{
	aos_assert_r(rdata && dfm_id, false);
	u32 siteid = rdata->getSiteid();
	
	u32 exist_dfm_id = 0;
	bool rslt = getDfmId(rdata, module_id, exist_dfm_id);
	if(exist_dfm_id)
	{
		OmnAlarm << "this siteid already has an docFileMgr!"
				<< "; module_id:" << module_id
				<< "; siteid:" << siteid
				<< "; dfm_id:" << exist_dfm_id
				<< enderr;
		return false;
	}

	
	AosBuffPtr module_buff;
	rslt = getModuleBuff(rdata, module_id, module_buff);
	aos_assert_r(rslt, false);

	if(!module_buff)
	{
		module_buff = OmnNew AosBuff(eBlockSize, 0 AosMemoryCheckerArgs);
	}
	
	module_buff->setCrtIdx(0);
	u32 crt_rcd_num = module_buff->getU32(0);
	if(crt_rcd_num >= eMaxRcdEachBlock)
	{
		OmnAlarm << "app num is up to the max!" << enderr;
		return false;
	}

	// save the siteid and docfilemgrid to file
	module_buff->setCrtIdx(0);
	module_buff->setU32(crt_rcd_num+1);
	u32 idx = eBlockRcdStart + crt_rcd_num * eRcdSize; 
	module_buff->setCrtIdx(idx);
	module_buff->setU32(siteid);
	module_buff->setU32(dfm_id);
	
	rslt = saveModuleBuff(rdata, module_id, module_buff);
	aos_assert_r(rslt, false);	

	return true;
}


bool
AosStorageApp::removeDfmId(
		const AosRundataPtr &rdata,
		const AosModuleId::E module_id,
		u32 &dfm_id)
{
	aos_assert_r(rdata, false);
	u32 siteid = rdata->getSiteid();
	dfm_id = 0;
	
	AosBuffPtr module_buff;
	bool rslt = getModuleBuff(rdata, module_id, module_buff);
	aos_assert_r(rslt, false);	
	if(!module_buff)	return true;

	module_buff->setCrtIdx(0);
	u32 crt_rcd_num = module_buff->getU32(0);
	u32 i=0;
	for(i=0; i<crt_rcd_num; i++)
	{
		u64 id = module_buff->getU32(0);        // site_id
		if(id != siteid)
		{
			module_buff->getU32(0);             // dfm_id
			continue;
		}
				    
		dfm_id = module_buff->getU32(0);
		aos_assert_r(dfm_id != 0, false);
		break;
	}
	if(!dfm_id)	return true;

	// remove from buff.
	u32 offset = eBlockRcdStart + i * eRcdSize; 
	module_buff->setCrtIdx(offset);
	module_buff->setU32(0);
	module_buff->setU32(0);

	rslt = saveModuleBuff(rdata, module_id, module_buff);
	aos_assert_r(rslt, false);	

	return true;
}


bool
AosStorageApp::getModuleBuff(
		const AosRundataPtr &rdata,
		const AosModuleId::E module_id,
		AosBuffPtr &buff)
{
	aos_assert_r(module_id <= AosModuleId::eMax, false);

	mLock->lock();
	u32 offset = module_id * eBlockSize;
	
	u64 crtFileSize = mAppFile->getFileCrtSize();
	if(offset > crtFileSize)
	{
		mLock->unlock();
		return true;
	}
	
	buff = OmnNew AosBuff(eBlockSize, 0 AosMemoryCheckerArgs);
	bool rslt = mAppFile->readToBuff(offset, eBlockSize, buff->data(), rdata.getPtr());
	mLock->unlock();
	aos_assert_r(rslt, false);

	buff->setDataLen(eBlockSize);
	
	//OmnScreen << "Ktttttttttt getModuleBuff; "
	//	<< "; offset:" << offset 
	//	<< "; fname:" << mAppFile->getFileName() 
	//	<< endl;

	return true;
}

	
bool
AosStorageApp::saveModuleBuff(
		const AosRundataPtr &rdata,
		const AosModuleId::E module_id,
		const AosBuffPtr &module_buff)
{
	aos_assert_r(module_id <= AosModuleId::eMax, false);

	mLock->lock();
	u32 offset = module_id * eBlockSize;    
	mAppFile->put(offset, module_buff->data(), module_buff->dataLen(), true, rdata.getPtr());
	mLock->unlock();
	
	//OmnScreen << "Ktttttttttt saveModuleBuff; "
	//	<< "; offset:" << offset 
	//	<< "; fname:" << mAppFile->getFileName() 
	//	<< endl;

	return true;
}


