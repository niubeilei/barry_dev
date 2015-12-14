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
// 07/22/2011 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/FileGroupMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Debug/Debug.h"
#include "DocFileMgr/DfmLog.h"
#include "Rundata/Rundata.h"
#include "ReliableFile/ReliableFile.h"
#include "StorageMgr/VirtualFileSys.h"
#include "StorageMgr/SystemId.h"
#include "StorageMgr/FileId.h"
#include "StorageMgr/FileKeys.h"
#include "DocFileMgr/DocFileMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilHash/HashedObjU64.h"
#include "UtilHash/StrObjHash.h"

const bool sgSanityCheck = false;

AosFileGroupMgr::AosFileGroupMgr(const AosVirtualFileSysPtr &vfs)
:
mLock(OmnNew OmnMutex()),
mVirtualId(vfs->getVirtualId()),
mVfs(vfs),
mCrtDFMId(eDocFileMgrIdStart)		// current docfilemgr id.
{
}


AosFileGroupMgr::~AosFileGroupMgr()
{
}


bool
AosFileGroupMgr::init()
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(AOS_SYS_SITEID);

	u64 file_id = ((u64)mVirtualId << 32) + AOSFILEID_FILEGROUPMGR;
	mConfigFile = mVfs->openRlbFile(rdata.getPtr(), file_id, 
						"fgm", eFileSize, true, AosRlbFileType::eNormal);
	aos_assert_r(mConfigFile, false);
	mConfigFile->setUseGlobalFmt(true);
	u64 crtFileSize = mConfigFile->getFileCrtSize();
	if(crtFileSize)
	{
		mCrtDFMId = crtFileSize/eEntrySize + eDocFileMgrIdStart + 1;
		if(mCrtDFMId < eDocFileMgrIdStart)	mCrtDFMId = eDocFileMgrIdStart;
	}

	// init mStrHashMap.
	u64 strkey_fid = ((u64)mVirtualId << 32) + AOSFILEID_FGM_STRKEY;
	AosReliableFilePtr strkey_file = mVfs->openRlbFile(rdata.getPtr(), 
			strkey_fid, "fgm_strkey", eFileSize, true, 
			AosRlbFileType::eNormal);
	aos_assert_r(strkey_file, false);
	strkey_file->setUseGlobalFmt(true);

	AosHashedObjPtr dftObj = OmnNew AosHashedObjU64();         
	aos_assert_r(dftObj, false);
	AosErrmsgId::E errorId;
	OmnString errmsg;
	mStrHashMap = OmnNew AosStrObjHash(dftObj, strkey_file, 
			                1000, 5, 1000, false, errorId, errmsg, rdata);
	aos_assert_r(mStrHashMap, false);

	AosFileGroupMgrPtr thisPtr(this, false);
	AosDfmLog::init(rdata, mVirtualId, thisPtr);
	// Ketty temp.
	rdata->setSiteid(100);
	AosDfmLog::init(rdata, mVirtualId, thisPtr);
	rdata->setSiteid(AOS_SYS_SITEID);

	return true;
}


bool
AosFileGroupMgr::clean()
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(AOS_SYS_SITEID);
	AosDfmLog::clear(rdata, mVirtualId);
	
	// Ketty temp.
	rdata->setSiteid(100);
	AosDfmLog::clear(rdata, mVirtualId);
	return true;
}


bool
AosFileGroupMgr::stopDfmLogCache()
{
	return AosDfmLog::stopProcCache();
}


bool
AosFileGroupMgr::continueProcDfmLogCache()
{
	return AosDfmLog::continueProcCache();
}

AosDocFileMgrObjPtr
AosFileGroupMgr::retrieveDfmByKey(
		const AosRundataPtr &rdata,
		const OmnString dfm_key,
		const bool create_flag,
		const AosDfmConfig &config)
{
	aos_assert_r(mStrHashMap, 0);
	bool exist = false;
	u32 dfm_id = 0;
	bool rslt = checkStrKeyExist(dfm_key, exist, dfm_id, rdata);
	aos_assert_r(rslt, 0);
	if(exist)
	{
		AosDocFileMgrObjPtr dfm = retrieveDfm(rdata, dfm_id, config);
		//dfm->setFileNamePrefix(dfm_key);
		return dfm;	
	}
	if(!create_flag)	return 0;

	// create it.
	AosDocFileMgrObjPtr dfm = createDfm(rdata, config);
	aos_assert_r(dfm, 0);
	//dfm->setFileNamePrefix(dfm_key);

	// add the key.
	AosHashedObjU64Ptr hashObjU64 = OmnNew AosHashedObjU64(dfm_key, dfm->getId());
	rslt = mStrHashMap->addDataPublic(dfm_key, hashObjU64, true, rdata);
	aos_assert_r(rslt, 0);
	return dfm;
}


AosDocFileMgrObjPtr
AosFileGroupMgr::createDfmByKey(
		const AosRundataPtr &rdata,
		const OmnString dfm_key,
		const AosDfmConfig &config)
{
	aos_assert_r(mStrHashMap, 0);
	
	bool exist = false;
	u32 dfm_id = 0;
	// 1.first check whethre this dfm_key is exist.
	bool rslt = checkStrKeyExist(dfm_key, exist, dfm_id, rdata);	
	if(exist)
	{
		AosSetErrorU(rdata, "dfm_key_already_exist") << ":" << dfm_key << ":" << mVirtualId << enderr;
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return 0;
	}

	
	AosDocFileMgrObjPtr dfm = createDfm(rdata, config);
	aos_assert_r(dfm, 0);
	
	// add the key.
	AosHashedObjU64Ptr hashObjU64 = OmnNew AosHashedObjU64(dfm_key, dfm->getId());
	rslt = mStrHashMap->addDataPublic(dfm_key, hashObjU64, true, rdata);
	aos_assert_r(rslt, 0);
	return dfm;
}


bool
AosFileGroupMgr::checkStrKeyExist(
		const OmnString &dfm_key, 
		bool &exist, 
		u32 &dfm_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mStrHashMap, false);

	exist = false;
	AosHashedObjPtr hashObj = mStrHashMap->find(dfm_key, rdata);
	if (hashObj)
	{
		exist = true;
		AosHashedObjU64Ptr hashObjU64 = (AosHashedObjU64 *)(hashObj.getPtr());
		dfm_id = hashObjU64->getValue();
	}
	return true;
}


AosDocFileMgrObjPtr
AosFileGroupMgr::createDfm(
		const AosRundataPtr &rdata,
		const AosDfmConfig &config)
{
	// This function creates a new AosDocFileMgr, 
	// and returns it.
	mLock->lock();
	u32 dfm_id = mCrtDFMId++;
	// Gavin, 2015/10/15
	mConfig = config;
	if (OmnApp::eNewDFMVersion == OmnApp::getDFMVersion())
	{
		//mConfig.mModuleId = (AosModuleId::E)dfm_id;
		AosDocFileMgrObjPtr dfm = OmnNew AosDfmWrapper(rdata.getPtr(), mVirtualId, mConfig);
		aos_assert_rl(dfm, mLock, NULL);
		insertToListPriv(dfm);
		mLock->unlock();
		return dfm;
	}
	mLock->unlock();

	return createDfmById(rdata, dfm_id, config);
}


AosDocFileMgrObjPtr
AosFileGroupMgr::createDfmById(
		const AosRundataPtr &rdata,
		const u32 dfm_id, 
		const AosDfmConfig &config)
{
	// This function creates a new DfmSaveCacher, 
	// and returns it.

	AosFileGroupMgrPtr thisPtr(this, false);
	//AosDocFileMgrObjPtr dfm = OmnNew AosDocFileMgr(rdata, thisPtr,
	//		mVirtualId, dfm_id, config AosMemoryCheckerArgs);
	AosDocFileMgrObjPtr dfm = AosDocFileMgr::createDfmStatic(
			rdata, thisPtr, mVirtualId, dfm_id, config AosMemoryCheckerArgs);

	mLock->lock();
	insertToListPriv(dfm);
	mLock->unlock();
	return dfm;
}


AosDocFileMgrObjPtr
AosFileGroupMgr::retrieveDfm(
		const AosRundataPtr &rdata,
		const u32 dfm_id,
		const AosDfmConfig &config)
{
	mConfig = config;
	// Gavin, 2015/09/02
	// if (OmnApp::eNewDFMVersion == OmnApp::getDFMVersion())
	// {
	// 	return OmnNew AosDfmWrapper(rdata.getPtr(), config);
	// }

	AosDocFileMgrObjPtr dfm = retrieveDfm(rdata, dfm_id);
	//aos_assert_r(dfm && dfm->isSameConf(config), 0);
	
	//dfm->mShowLog = config.mShowLog;
	return dfm;
}

AosDocFileMgrObjPtr
AosFileGroupMgr::retrieveDfm(
		const AosRundataPtr &rdata,
		const u32 dfm_id)
{
	// Each DocFileMgr is identified by an ID (passed in through 'docFileMgrId'.
	// This function uses the ID to retrieve the DocFileMgr.

	mLock->lock();
	// check Whether this docFileMgr is in memory.
	AosDocFileMgrObjPtr dfm = getFromListPriv(dfm_id);
	if(dfm)
	{
		mLock->unlock();
		return dfm;	
	}

	// Gavin, 2015/10/13
	if (OmnApp::eNewDFMVersion == OmnApp::getDFMVersion())
	{
		//mConfig.mModuleId = (AosModuleId::E)dfm_id;
		AosDocFileMgrObjPtr dfm = OmnNew AosDfmWrapper(rdata.getPtr(), mVirtualId, mConfig);
		aos_assert_rl(dfm, mLock, NULL);
		insertToListPriv(dfm);
		mLock->unlock();
		return dfm;
	}


	// This means that is not in the memory
	AosBuffPtr dfm_buff;
	bool rslt = readDfmBuffFromFile(rdata, dfm_id, dfm_buff);
	aos_assert_rl(rslt, mLock, 0);
	if(!dfm_buff)
	{
		// this means this dfm has't create yet. return. 
		mLock->unlock();
		return 0;
	}
	
	AosFileGroupMgrPtr thisPtr(this, false);
	//dfm = OmnNew AosDocFileMgr(rdata, thisptr, mVirtualId, 
	//		dfm_id, dfm_buff, config AosMemoryCheckerArgs);
	dfm = AosDocFileMgr::serializeDfmStatic(rdata, thisPtr,
			dfm_buff AosMemoryCheckerArgs);
	aos_assert_r(dfm, 0);
	
	insertToListPriv(dfm);
	mLock->unlock();
	return dfm;
}


AosDocFileMgrObjPtr
AosFileGroupMgr::getFromListPriv(const u32 dfm_id)
{
	AosDocFileMgrObjPtr docFileMgr;
	map<u32, AosDocFileMgrObjPtr>::iterator itmap = mDocFileMgrMap.find(dfm_id);
	if(itmap != mDocFileMgrMap.end())
	{
		// this docFileMgr is in the memory, return it.
		aos_assert_r(mDocFileMgrHead, 0);
		docFileMgr = itmap->second;
		aos_assert_r(docFileMgr->getId() == dfm_id, 0);

		// push the block to the first of list
		bool rslt = mDocFileMgrHead->moveToFront(docFileMgr);
		mDocFileMgrHead = docFileMgr;
		aos_assert_r(rslt, 0);
		// aos_assert_rl(sanityCheck(), mLock, false);
	}
	return docFileMgr;	
}


bool
AosFileGroupMgr::insertToListPriv(const AosDocFileMgrObjPtr &docFileMgr)
{
	// This func is cacheing the docFileMgr.
	if(mDocFileMgrMap.size() > eMaxDocFileMgr)
	{
		// This means chose the last of the list, remove old one from the map and list
		AosDocFileMgrObjPtr old_dfm = mDocFileMgrHead->prevEntry();
		aos_assert_r(old_dfm && old_dfm != mDocFileMgrHead, false);
		u32 old_dfm_id= old_dfm->getId();

		map<u32, AosDocFileMgrObjPtr>::iterator itmap = mDocFileMgrMap.find(old_dfm_id);
		aos_assert_r(itmap != mDocFileMgrMap.end(), false);

		aos_assert_r(old_dfm == itmap->second, false);
		old_dfm->removeFromList();	
		mDocFileMgrMap.erase(itmap);
		// aos_assert_rl(sanityCheck(), false);

		// Chen Ding, 12/05/2012
		// Need to move this line to DocFileMgr's destructor.
		//old_dfm->stop();
	}

	if (!mDocFileMgrHead)
	{
		docFileMgr->setPointers();
		mDocFileMgrHead = docFileMgr;
	}
	else
	{
		mDocFileMgrHead->insertAt(docFileMgr);
		mDocFileMgrHead = docFileMgr;
	}
	mDocFileMgrMap[docFileMgr->getId()] = docFileMgr;
	// aos_assert_rl(sanityCheck(), mLock, false);
	return true;
}


bool
AosFileGroupMgr::startStop()
{
	mLock->lock();
	map<u32, AosDocFileMgrObjPtr>::iterator itr = mDocFileMgrMap.begin();
	for(; itr != mDocFileMgrMap.end(); itr++)
	{
		AosDocFileMgrObjPtr dfm = itr->second; 
		dfm->startStop();
	}
	mLock->unlock();
	return true;
}
	

bool
AosFileGroupMgr::stop()
{
	mLock->lock();
	map<u32, AosDocFileMgrObjPtr>::iterator itr = mDocFileMgrMap.begin();
	for(; itr != mDocFileMgrMap.end(); itr++)
	{
		AosDocFileMgrObjPtr dfm = itr->second; 
		dfm->stop();
	}
	
	mDocFileMgrMap.clear();
	mDocFileMgrHead = 0;
	mLock->unlock();

	OmnScreen << "Ktttt FileGroupMgr:"
		<< "clean; virtual_id:" << mVirtualId
		<< endl;

	return true;
}


bool 
AosFileGroupMgr::sanityCheck()
{
	if(!sgSanityCheck)
	{
		return true;
	}
	return true;
}


bool
AosFileGroupMgr::removeDfm(
		const AosRundataPtr &rdata,
		const u32 dfm_id)
{
	// check Whether this docFileMgr is in memory.
	mLock->lock();
	AosDocFileMgrObjPtr dfm = getFromListPriv(dfm_id);
	if(dfm)
	{
		bool rslt = dfm->removeAllFiles(rdata);
		aos_assert_rl(rslt, mLock, false);
		
		// maybe need remove from list. not implement.
		mLock->unlock();
		return true;
	}
		
	// This means that is not in the memory
	AosBuffPtr dfm_buff;
	bool rslt = readDfmBuffFromFile(rdata, dfm_id, dfm_buff);
	aos_assert_rl(rslt, mLock, false);
	if(!dfm_buff)
	{
		// this means this docFileMgr has't create yet. return. 
		mLock->unlock();
		return 0;
	}

	rslt = AosDocFileMgr::removeFilesByDfmBuff(rdata, dfm_id, dfm_buff);
	aos_assert_rl(rslt, mLock, false);

	rslt = removeDfmBuffFromFile(rdata, dfm_id, dfm_buff);
	aos_assert_rl(rslt, mLock, false);

	mLock->unlock();
	return true;
}


bool
AosFileGroupMgr::saveDfmBuffToFile(
		const AosRundataPtr &rdata,
		const u32 dfm_id,
		const AosBuffPtr &dfm_buff)
{
	// sizeof(u32) is the dfm_buff len.
	//aos_assert_r((u32)dfm_buff->dataLen() <= (eEntrySize - sizeof(u32)), false);
	if((u32)dfm_buff->dataLen() > (eEntrySize - sizeof(u32)))
	{
		OmnAlarm << "FileGroupMgr error!"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << dfm_id
			<< "dfm_buff_len:" << dfm_buff->dataLen()
			<< enderr;
		return false;
	}

	aos_assert_r(mConfigFile, false);
	u64 offset = eDFMInfoStart + dfm_id * eEntrySize;
	mConfigFile->setU32(offset, dfm_buff->dataLen(), false, rdata.getPtr());
	bool rslt = mConfigFile->put(offset + sizeof(u32),
			dfm_buff->data(), dfm_buff->dataLen(), true, rdata.getPtr());
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosFileGroupMgr::removeDfmBuffFromFile(
		const AosRundataPtr &rdata,
		const u32 dfm_id,
		AosBuffPtr &dfm_buff)
{
	if(!dfm_buff)
	{
		dfm_buff = OmnNew AosBuff(eEntrySize  AosMemoryCheckerArgs);
	}
	memset(dfm_buff->data(), 0, eEntrySize);
	
	return saveDfmBuffToFile(rdata, dfm_id, dfm_buff);
}


bool
AosFileGroupMgr::readDfmBuffFromFile(
		const AosRundataPtr &rdata,
		const u32 dfm_id,
		AosBuffPtr &dfm_buff)
{
	aos_assert_r(mConfigFile && dfm_id, false);
	
	u32 offset = eDFMInfoStart + dfm_id * eEntrySize;
	u64 crtFileSize = mConfigFile->getFileCrtSize();
	if(offset > crtFileSize)
	{
		// this means this docFileMgr has't create yet. return. 
		return true;
	}
	
	dfm_buff = OmnNew AosBuff(eEntrySize  AosMemoryCheckerArgs);
	memset(dfm_buff->data(), 0, eEntrySize);
	bool rslt = mConfigFile->readToBuff(offset, eEntrySize, dfm_buff->data(), rdata.getPtr());
	aos_assert_r(rslt, false);
	dfm_buff->setDataLen(eEntrySize);

	u32 buff_len = dfm_buff->getU32(0);
	if(!buff_len)
	{
		dfm_buff = 0;
		return true;
	}
	
	aos_assert_r(buff_len < eEntrySize, false);
	char *data = dfm_buff->data();
	memmove(data, data + sizeof(u32), buff_len);
	dfm_buff->setDataLen(buff_len);
	dfm_buff->setCrtIdx(0);

	return true;
}


