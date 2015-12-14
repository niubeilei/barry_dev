////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// DescriptiOn:
// A Virtual File System is a file container with certain rules. 
// Virtual file systems are identified through a File Key. There are
// System Virtual File Systems and User Virtual File Systems.
// System Virtual File Systems are identified by reserved Virtual
// File System Keys. To open a system virtual file system, the caller
// must have a correct password.
//
// There can be different types of virtual file systems. In the current
// implementations, only one is supported. This virtual file systems
// maintains a group of files. Each file is a Virtual File (implemented
// by DocFileMgr). Each virtual file is identified by two u32 integers:
//
// 			site id
// 			virtual server id
//
// This is mainly used by IILs and Doc Servers. For IILs, Virtual Server
// ID identifies a virtual server and Site ID identifies a site.
//	
// Modification History:
// 07/28/2011 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StorageMgr/VirtualFileSys.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocFileMgr/FileGroupMgr.h"
#include "DfmWrapper/DfmWrapper.h"
#include "Porting/Sleep.h"
#include "ReliableFile/ReliableFile.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Passwords.h"
#include "StorageMgr/StorageMgr.h"
#include "StorageMgr/FileKeys.h"
#include "StorageMgr/SystemId.h"
#include "StorageMgr/ConfigMgr.h"
#include "StorageMgrUtil/FileInfo.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "UtilHash/HashedObjU64.h"
#include "UtilHash/StrObjHash.h"  
#include "XmlUtil/XmlTag.h"

#include "SysMsg/GetTotalFileInfoTrans.h"
#include "SysMsg/RecoverFileTrans.h"


AosVirtualFileSys::AosVirtualFileSys(
		const u32 virtual_id,
		const AosStorageMgrPtr &stm)
:
mStartLock(OmnNew OmnMutex()),
mLock(OmnNew OmnMutex()),
mVirtualId(virtual_id),
mStorageMgr(stm),
//mFileSeqno(eFileSeqnoStart),
mFileSeqno(0),
mCubeGrpId(-1),
mStart(false)
{
	//if (mVirtualId == AOS_TEMPFILE_VIRTUALID) mIsBkp = false; 
	mRdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRdata->setSiteid(AOS_SYS_SITEID);
	
	bool rslt = initAllocFile();
	aos_assert(rslt && mAllocFile);
	
	AosVirtualFileSysPtr thisptr(this, false);      
	mFileGroupMgr = OmnNew AosFileGroupMgr(thisptr);
	mStorageApp = OmnNew AosStorageApp(thisptr);
	mConfigMgr = OmnNew AosConfigMgr(thisptr);
}


AosVirtualFileSys::~AosVirtualFileSys()
{
}


bool
AosVirtualFileSys::initAllocFile()
{
	OmnString alloc_fname = getAllocFileName();
	mAllocFile = OmnNew OmnFile(alloc_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(!mAllocFile || !mAllocFile->isGood())
	{
		mAllocFile = OmnNew OmnFile(alloc_fname, OmnFile::eCreate AosMemoryCheckerArgs);
		if(!mAllocFile || !mAllocFile->isGood())
		{
			OmnAlarm << "Failed to open file: " << alloc_fname << enderr;
			return false;
		}
	}
	
	u64 crtFileSize = mAllocFile->getFileCrtSize();
	mFileSeqno = crtFileSize/eEntrySize;
	return true;
}


bool
AosVirtualFileSys::resetAllocFile()
{
	OmnString alloc_fname = getAllocFileName();
	mAllocFile = OmnNew OmnFile(alloc_fname, OmnFile::eCreate AosMemoryCheckerArgs);
	
	mFileSeqno = 0;
	return true;
}


bool
AosVirtualFileSys::start()
{
	mStartLock->lock();
	if(mStart)
	{
		mStartLock->unlock();
		return true;
	}
	
	//OmnScreen << "--------start virtual "
	//	<< "; virtual_id:" << mVirtualId
	//	<< endl; 

	mStart = true;
	
	mCubeGrpId = AosGetSelfCubeGrpId();
	//aos_assert_r(mCubeGrpId != -1, false); 	
	
	u64 strkey_fid = ((u64)mVirtualId << 32) + AOSFILEID_STRKEY;
	AosReliableFilePtr str_key_file = openRlbFile(mRdata.getPtr(), strkey_fid, 
			"strkey_map", eStrKeyFileSize, true, AosRlbFileType::eNormal, true); 
	str_key_file->setUseGlobalFmt(true);
	aos_assert_rl(str_key_file, mStartLock, false);

	AosHashedObjPtr dftObj = OmnNew AosHashedObjU64();
	aos_assert_rl(dftObj, mStartLock, false);
	AosErrmsgId::E errorId;
	OmnString errmsg;
	mStrHashMap = OmnNew AosStrObjHash(dftObj, str_key_file, 
			        1000, 5, 1000, false, errorId, errmsg, mRdata);
	aos_assert_rl(mStrHashMap, mStartLock, false);

	mFileGroupMgr->init();
	mConfigMgr->init();
	mStorageApp->init();

	mStartLock->unlock();
	return true;
}


bool
AosVirtualFileSys::clean()
{
	mFileGroupMgr->clean();
	return true;
}

bool
AosVirtualFileSys::isStart()
{
	mStartLock->lock();
	bool start = mStart;
	mStartLock->unlock();
	return start;
}


bool
AosVirtualFileSys::startStop()
{
	mStartLock->lock();
	if(!mStart)
	{
		mStartLock->unlock();
		return true;
	}

	mFileGroupMgr->startStop();
	mStartLock->unlock();
	return true;
}


bool
AosVirtualFileSys::stop()
{
	mStartLock->lock();
	if(!mStart)
	{
		mStartLock->unlock();
		return true;
	}
	mStart = false;

	mConfigMgr->stop();
	mFileGroupMgr->stop();
	mStorageApp->stop();  
	
	mStartLock->unlock();
	return true;
}


bool
AosVirtualFileSys::switchToBkp()
{
	return mFileGroupMgr->stopDfmLogCache();	
}


bool
AosVirtualFileSys::reStart()
{
	// when called switchToBkp. but master switchToMaster failed.
	// then will call this func.
	return mFileGroupMgr->continueProcDfmLogCache();
}


OmnString
AosVirtualFileSys::getAllocFileName()
{
	// This function retrieves the logic file 'file_id'. A logic file is a file
	// that is identified through a File ID (an integer). This class maintains
	// a map that maps File IDs to actual file names. 
	
	int device_id;
	OmnString virtual_dir = mStorageMgr->getVirtualSysDir(mVirtualId, isLocalBackupVFS(), device_id);
	
	OmnString alloc_fname = mStorageMgr->getBaseDir(device_id);
	alloc_fname << virtual_dir << "fname_alloc_vir_" << mVirtualId;

	return alloc_fname;
}


bool
AosVirtualFileSys::getFileInfo(
		AosRundata *rdata,
		const u64 file_id, 
		bool &find,  
		AosStorageFileInfo & file_info)
{
	// This function retrieves the virtual file 'file_id'. A virtual file is a file
	// that is identified through a File ID (an integer). This class maintains
	// a map that maps File IDs to actual file names. 

	// This func maybe called by ReplicMgr.
	/*
	while(!mSyncDataFinish)
	{
		OmnSleep(2);
		continue;
	}
	*/

	u32 virtual_id = (u32)(file_id >>32);
	aos_assert_r(virtual_id == mVirtualId, false);

	//u32 fseqno = (u32)file_id;
	//if(fseqno >= mFileSeqno)
	//{
	//	find = false;
	//	return true;
	//}
	
	bool rslt = getFromAllocFile(rdata, file_id, find, file_info);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVirtualFileSys::removeDocFileMgr(
		AosRundata *rdata,
		const AosModuleId::E module_id)
{
	//start();
	//return mStorageApp->removeDocFileMgr(module_id, siteid, rdata);	
	//return mStorageApp->removeDocFileMgr(module_id, rdata);	

	u32 dfm_id;
	bool rslt = mStorageApp->removeDfmId(rdata, module_id, dfm_id);
	aos_assert_r(rslt, false);
	if(dfm_id)
	{
		rslt = mFileGroupMgr->removeDfm(rdata, dfm_id);
		aos_assert_r(rslt, false);
	}
	else
	{
		OmnScreen << "remove dfm failed. can't find; module_id:" << module_id
				<< "; site_id:" << rdata->getSiteid()
				<< endl;
	}
	return true;
}


AosDocFileMgrObjPtr
AosVirtualFileSys::retrieveDocFileMgr(
		AosRundata *rdata,
		const AosDfmConfig &config)
		//const AosModuleId::E module_id,
		////const u32 siteid, 
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	//start();
	//return mStorageApp->retrieveDocFileMgr(module_id, rdata);
	//return mStorageApp->retrieveDocFileMgr(module_id, proc, rdata);

	//OmnScreen << "Ktttttttttt retrieveDfm; "
	//	<< "; virtual_id:" << mVirtualId
	//	<< "; site_id:" << rdata->getSiteid()
	//	<< "; m_id:" << config.mModuleId
	//	<< endl;

	// Gavin, 2015/09/02
	// if (OmnApp::eNewDFMVersion == OmnApp::getDFMVersion())
	// {
	// 	AosDocFileMgrObjPtr dfm = OmnNew AosDfmWrapper(rdata, config);
	// 	aos_assert_rr(dfm, rdata, 0);
	// 	return dfm;
	// }

	u32 dfm_id;
	bool rslt = mStorageApp->getDfmId(rdata, config.mModuleId, dfm_id);
	aos_assert_r(rslt, 0);
	if(!dfm_id)		return 0;
	
	//OmnScreen << "temp----: retrieve dfm:"
	//	<< "; module_id:" << config.mModuleId
	//	<< "; dfm_id:" << dfm_id
	//	<< endl;
	
	//OmnScreen << "Ktttttttttt retrieveDfm; "
	//	<< "; site_id:" << rdata->getSiteid()
	//	<< "; m_id:" << config.mModuleId
	//	<< "; get dfm_id:" << dfm_id
	//	<< endl;

	return mFileGroupMgr->retrieveDfm(rdata, dfm_id, config);
}

	
AosDocFileMgrObjPtr
AosVirtualFileSys::createDocFileMgr(
		AosRundata *rdata,
		const AosDfmConfig &config)
		//const AosModuleId::E module_id,
		////const u32 siteid, 
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	//start();
	//return mStorageApp->createDocFileMgr(module_id, prefix, location, rdata);
	//return mStorageApp->createDocFileMgr(module_id, proc, rdata);
	
	AosDocFileMgrObjPtr dfm = mFileGroupMgr->createDfm(rdata, config);
	aos_assert_r(dfm, 0);

	u32 dfm_id = dfm->getId();
	bool rslt = mStorageApp->addDfmId(rdata, config.mModuleId, dfm_id);
	aos_assert_r(rslt, 0);
	
	//OmnScreen << "Ktttttttttt createDfm; "
	//	<< "; virtual_id:" << mVirtualId
	//	<< "; site_id:" << rdata->getSiteid()
	//	<< "; m_id:" << config.mModuleId
	//	<< "; dfm_id:" << dfm_id
	//	<< endl;
	
	return dfm;
}


AosDocFileMgrObjPtr
AosVirtualFileSys::createDocFileMgrById(
		AosRundata *rdata,
		const u32 dfm_id, 
		const AosDfmConfig &config)
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	//start();
	return mFileGroupMgr->createDfmById(rdata, dfm_id, config);
}


AosDocFileMgrObjPtr
AosVirtualFileSys::retrieveDocFileMgrById(
		AosRundata *rdata,
		const u32 dfm_id, 
		const AosDfmConfig &config)
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	//start();
	return mFileGroupMgr->retrieveDfm(rdata, dfm_id, config);	
}


AosDocFileMgrObjPtr
AosVirtualFileSys::retrieveDocFileMgrByKey(
		AosRundata *rdata,
		const OmnString dfm_key,
		const bool create_flag,
		const AosDfmConfig &config)
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	return mFileGroupMgr->retrieveDfmByKey(rdata, dfm_key, create_flag, config);	
}


AosDocFileMgrObjPtr
AosVirtualFileSys::createDocFileMgrByKey(
		AosRundata *rdata,
		const OmnString dfm_key,
		const AosDfmConfig &config)
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	return mFileGroupMgr->createDfmByKey(rdata, dfm_key, config);	
}



AosXmlTagPtr
AosVirtualFileSys::getConfigDoc(const u64 &docid, AosRundata *rdata)
{
	//start();
	aos_assert_r(mConfigMgr, 0);
	return mConfigMgr->getDoc(docid, rdata);
}


bool
AosVirtualFileSys::saveConfigDoc(const u64 &docid, const AosXmlTagPtr &doc, AosRundata *rdata)
{
	//start();
	aos_assert_r(mConfigMgr, 0);
	return mConfigMgr->saveDoc(docid, doc, rdata);
}


bool
AosVirtualFileSys::addNewFile(
		AosRundata *rdata,
		const OmnString &name_prefix,
		const u64 request_space,
		const bool reserve_flag,
		AosStorageFileInfo &file_info)
{
	//Linda 2013/05/14
	int device_id = -1;
	return addNewFile(rdata, name_prefix, request_space, reserve_flag, file_info, device_id); 
}

bool
AosVirtualFileSys::addNewFile(
		AosRundata *rdata,
		const OmnString &name_prefix,
		const u64 request_space,
		const bool reserve_flag,
		AosStorageFileInfo &file_info,
		int &device_id)
{
	//if(request_space == 0)
	//{
	//	OmnScreen << "file request_space is 0 ??" << endl;
	//}
	// 1. first get the available device_id.
	//int device_id = -1;
	//Linda 2013/05/14
	bool appoint_device_id = false;
	if (device_id >= 0)  appoint_device_id = true;

	int filter_deviceid = -1;
	OmnString sub_dir_name;
	bool rslt = mStorageMgr->allocateSpace(
			rdata, mVirtualId, rdata->getSiteid(), request_space, 
			reserve_flag, filter_deviceid, appoint_device_id, 
			device_id, sub_dir_name);
	aos_assert_r(rslt && sub_dir_name != "" && device_id >=0, false);	
	
	// 2. alloc a fileName
	mLock->lock();
	// mark 11/01
	if(mFileSeqno < eFileSeqnoStart)
	{
		mFileSeqno = eFileSeqnoStart; 
	}
	u32 fseqno = mFileSeqno++;
	mLock->unlock();

	OmnString fname = sub_dir_name;
	fname << ( name_prefix != "" ? name_prefix : "Data");
	fname << "_" << fseqno; 
	if(fname.length() > eMaxNameLen)
	{
		AosSetErrorU(rdata, "eInternalError")
			<< ". generate name too long: " << fname << enderr;
		return false;
	}

	u64 file_id = ((u64)mVirtualId << 32) + fseqno;
	file_info = AosStorageFileInfo(file_id,
		rdata->getSiteid(), device_id, request_space, fname);
	
	// 3. save the allocation into the file;  
//OmnScreen << "============================saveToAllocFile" << endl;
	rslt = saveToAllocFile(rdata, file_info);
	aos_assert_r(rslt, false);
	return true;
}

	
bool
AosVirtualFileSys::addNewFileById(
		AosRundata *rdata, 
		AosStorageFileInfo &file_info,
		const int filter_deviceid)
{
	// This func alloc a new fname for this file_info.fileId.
	// the file_info has enough informaton. the only things this Func do is
	// re find a available device_id. and re alloc a fname, and save this alloc
	// into the allocFile.
	//
	
	u32 vir_id = (u32)(file_info.fileId >> 32);
	aos_assert_r(vir_id == mVirtualId, false);

	u32 file_seqno = (u32)file_info.fileId;
	if( file_seqno >= mFileSeqno)
	{
		mLock->lock();
		mFileSeqno = file_seqno + 1;
		mLock->unlock();
	}

	// 1. first get the available device_id.
	int device_id = -1;
	OmnString sub_dir_name;
	bool rslt = mStorageMgr->allocateSpace(
			rdata, mVirtualId, file_info.siteId, file_info.requestSize, 
			true, filter_deviceid, false, device_id, sub_dir_name); 
	aos_assert_r(rslt && sub_dir_name != "" && device_id >=0, false);	

	// 2. generate  fileName
	OmnString new_fname = sub_dir_name;	
	new_fname << file_info.getPureFname();
	if(new_fname.length() > eMaxNameLen)
	{
		OmnAlarm << "file_name too long: " << new_fname << enderr;
		return false;
	}
	
	file_info.deviceId = device_id;
	file_info.fname = new_fname;
	//file_info.fname = file_info.getFullFname();

	// 2. save the allocation into the file;  
//OmnScreen << "============================saveToAllocFile" << endl;
	rslt = saveToAllocFile(rdata, file_info);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVirtualFileSys::getFileInfoByStrKey(
		AosRundata *rdata,
		const OmnString &str_key,
		AosStorageFileInfo &file_info,
		bool &exist)
{
	exist = false;
	u64 file_id = 0;
	bool rslt = checkStrKeyExist(rdata, str_key, exist, file_id);
	aos_assert_r(rslt, false);
	if(!exist) return true;

	bool find;
	rslt = getFileInfo(rdata, file_id, find, file_info);
	aos_assert_r(rslt && find, false);
	return true;
}


OmnString
AosVirtualFileSys::getFileName(AosRundata *rdata, const u64 file_id)
{
	AosStorageFileInfo file_info;
	bool find;
	bool rslt = getFileInfo(rdata, file_id, find, file_info);
	//Jozhi 2014-04-10 this file is not exist
	//aos_assert_r(rslt && find, "");
	if (!rslt || !find)
	{
		OmnScreen << "may be Alarm, file not found, file id: " << file_id << endl;
		return "";
	}
	OmnString full_fname = file_info.getFullFname(); 
	return full_fname;
}


bool
AosVirtualFileSys::removeFile(
		AosRundata *rdata,
		const u64 file_id)
{
OmnScreen << "******************************************remove file id: " << file_id << endl;
	bool find = false;
	AosStorageFileInfo file_info;
	bool rslt = getFromAllocFile(rdata, file_id, find, file_info);
	//Jozhi 2014-04-11
	if (!rslt || !find)
	{
		OmnScreen << "may be Alarm, file not found, file id: " << file_id << endl;
		return true;
	}
	aos_assert_r(rslt, false);
	aos_assert_r(find, false);

	// remove this file info
	u64 offset = eEntryStart + (u32)file_id * eEntrySize;
	char data[eEntrySize];
	memset(data, 0, eEntrySize);
	mAllocFile->lock();
	mAllocFile->put(offset, data, eEntrySize, true);
	mAllocFile->unlock();
	
	// free the space.
	/*
	u64 reserved_size = file_info.requestSize;
	int device_id = file_info.deviceId;
	rslt = mStorageMgr->freeSpace(device_id, reserved_size);
	aos_assert_r(rslt, false);
	*/

	// unlink this fname.
	OmnString full_fname = file_info.getFullFname();
	//Jozhi 2014-04-09 task to remove this file, but file has remove before;
	if (full_fname != "" && access(full_fname.data(), F_OK) == 0)
	{
		int rs = unlink(full_fname.data());
		aos_assert_r(rs >=0, false);
	}
	return true;
}


bool
AosVirtualFileSys::addNewFileByStrKey(
		AosRundata *rdata,
		const OmnString &str_key,
		const u64 request_space,
		const bool reserve_flag,
		AosStorageFileInfo &file_info)
{
	//start();
	
	aos_assert_r(mStrHashMap, false);
	
	bool exist = false;
	u64 file_id = 0;
	bool rslt = checkStrKeyExist(rdata, str_key, exist, file_id);
	aos_assert_r(rslt, false);
	if(exist)
	{
		//u32 pure_vid = getPureVirtualId(mVirtualId);
		OmnAlarm << "File already exists: " << str_key
			<< "; virtual_id:" << mVirtualId 
			<< enderr;
		return false;
	}

	// create the new file.
	rslt = addNewFile(rdata, str_key, request_space, 
			reserve_flag, file_info);
	aos_assert_r(rslt, false);

	// addStrKey
	AosHashedObjU64Ptr hashObjU64 = OmnNew AosHashedObjU64(str_key, file_info.fileId);
	rslt = mStrHashMap->addDataPublic(str_key, hashObjU64, true, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVirtualFileSys::checkStrKeyExist(
		AosRundata *rdata,
		const OmnString &str_key, 
		bool &exist, u64 &file_id)
{
	//start();
	
	aos_assert_r(mStrHashMap, false);
	
	exist = false;
	AosHashedObjPtr hashObj = mStrHashMap->find(str_key, rdata);
	if (hashObj)
	{
		exist = true;
		AosHashedObjU64Ptr hashObjU64 = (AosHashedObjU64 *)(hashObj.getPtr());
		file_id = hashObjU64->getValue();
	}
	return true;
}


bool
AosVirtualFileSys::removeFileByStrKey(
		AosRundata *rdata,
		const OmnString &str_key,
		u64 &file_id,
		bool &exist)
{
	// Ketty 2014/03/05
	//start();
	
	aos_assert_r(mStrHashMap, false);
	
	bool rslt = checkStrKeyExist(rdata, str_key, exist, file_id);
	aos_assert_r(rslt, false);
	if(!exist) return true;

	// create the new file.
	rslt = removeFile(rdata, file_id);
	aos_assert_r(rslt, false);
	
	// removeStrKey
	rslt = mStrHashMap->erase(str_key, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVirtualFileSys::saveToAllocFile(
		AosRundata *rdata,
		const AosStorageFileInfo &file_info)
{
	AosBuffPtr buff = OmnNew AosBuff(eEntrySize, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, eEntrySize);
	setFileInfoToBuff(buff, file_info);
	
	u8 check_sum = AosCalculateCheckSum(buff->data(), eEntrySize);
	buff->setCrtIdx(eEntrySize - 1);
	buff->setU8(check_sum);	

	u32 fseqno = (u32)file_info.fileId;
	u64 offset = eEntryStart + fseqno * eEntrySize;
//OmnScreen << "============================ " << file_info.fname << " , " << file_info.fileId << " , " << offset << endl;
//OmnScreen << "save to alloc file:" << file_info.fileId << endl;
	mAllocFile->lock();
	bool rslt = mAllocFile->put(offset,  buff->data(), eEntrySize, true);
	mAllocFile->unlock();
//OmnScreen << "save to alloc file end:" << file_info.fileId << endl;
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVirtualFileSys::getFromAllocFile(
		AosRundata *rdata,
		const u64 file_id, 
		bool& find, 
		AosStorageFileInfo &file_info)
{
	// first read from the file. and check whether the data is good. 
	// if not. recover the mAllocFile. and then try again.

	find = false;
	u64 offset = eEntryStart + (u32)file_id * eEntrySize;

	mAllocFile->lock();
	u64 file_len = mAllocFile->getLength();
	if(offset + eEntrySize > file_len)
	{
		mAllocFile->unlock();
		return true;
	}

	AosBuffPtr buff = OmnNew AosBuff(eEntrySize, 0 AosMemoryCheckerArgs);
	int bytesread = mAllocFile->readToBuff(offset, eEntrySize, buff->data());
	mAllocFile->unlock();
	
	aos_assert_r(bytesread>0 && (u32)bytesread == eEntrySize, false);
	bool data_isgood = AosCheckCheckSum(buff->data(), eEntrySize);
	if(!data_isgood)
	{
		// means this file has damaged.
		OmnAlarm << "Alloc File damaged." << enderr;
		//OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	buff->setDataLen(eEntrySize);
	getFileInfoFromBuff(file_info, buff);

	if((u32)file_id < eFileSeqnoStart && !file_info.isValid())
	{
		return true;
	}

	//Jozhi 2014-04-10 file is not exist
	if (file_info.fileId == 0 || !file_info.isValid())
	{
		find = false;
		return true;
	}

	aos_assert_r(file_id == file_info.fileId && file_info.isValid(), false);
	find = true;
	return true;
}


void
AosVirtualFileSys::setFileInfoToBuff(const AosBuffPtr &buff, const AosStorageFileInfo & file_info)
{
	// Chen Ding, 08/18/2012
	// Setting data to buff and retrieving data from buff are handled by 
	// this and next member function to make sure that they use exactly 
	// the same format:
	// 		file id				(u64)
	// 		site id				(u32)
	// 		device_id			(u32)
	//		request space		(u64)
	//		backup policy		(u32)
	//		file name			(OmnString)

	buff->setU64(file_info.fileId);
	buff->setU32(file_info.siteId);
	buff->setU32(file_info.deviceId);
	buff->setU64(file_info.requestSize);
	buff->setOmnStr(file_info.fname);
}


bool
AosVirtualFileSys::getFileInfoFromBuff(
		AosStorageFileInfo &file_info,
		const AosBuffPtr &buff)
{
	// 'buff' format is:
	// 		file id				(u64)
	// 		site id				(u32)
	// 		device_id			(u32)
	//		request space		(u64)
	//		backup policy		(u32)
	//		file name			(OmnString)

	file_info.fileId = buff->getU64(0);
	file_info.siteId = buff->getU32(0);                  
	file_info.deviceId = buff->getU32(0);
	file_info.requestSize = buff->getU64(0);
	file_info.fname = buff->getOmnStr("");
	return true;
}


//OmnString
//AosVirtualFileSys::getBackupAllocFname()
//{
//	OmnString alloc_fname = "fname_alloc_vir_";
//	//u32 pure_vid = getPureVirtualId(mVirtualId);
//	//alloc_fname << pure_vid << "_backup";
//	alloc_fname << mVirtualId << "_backup";
//	return alloc_fname;	
//}


bool
AosVirtualFileSys::isLocalBackupVFS()
{
	//u32 pure_vid = getPureVirtualId(mVirtualId);
	OmnScreen << "Not ImplementedYet" << endl;
	return false;
	//bool local = AosIsVirtualIdLocal(pure_vid);	
	//return local;
}


// for create files.
AosReliableFilePtr 
AosVirtualFileSys::createRlbFile(
		AosRundata *rdata,
		//u64 &file_id,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const AosRlbFileType::E file_type,
		const bool reserve_flag)
{
	AosStorageFileInfo file_info;
	bool rslt = addNewFile(rdata, fname_prefix, requested_space, reserve_flag, file_info);
	aos_assert_r(rslt, 0);

	//file_id = file_info.fileId;
	AosReliableFilePtr rfile = AosReliableFile::getReliableFile(
			file_info, file_type, true, rdata);
	aos_assert_r(rfile, 0);
	return rfile;
}



AosReliableFilePtr 
AosVirtualFileSys::openRlbFile(
		const u64 &file_id,
		const AosRlbFileType::E file_type,
		AosRundata *rdata)
{
	// 'file_id' is an ID. Its upper four bytes are used to identify
	// VirtualId and lower four bytes are the ID used by the VirtualFileSys 
	// to uniquely identify a file. 

	bool find = false;
	AosStorageFileInfo file_info;
	bool rslt = getFileInfo(rdata, file_id, find, file_info);
	aos_assert_r(rslt, 0);
	if(!find)	return 0;

	AosReliableFilePtr rfile = AosReliableFile::getReliableFile(
			file_info, file_type, false, rdata);
	aos_assert_r(rfile, 0);
	return rfile;
}

	
AosReliableFilePtr
AosVirtualFileSys::openRlbFile(
		AosRundata *rdata,
		const u64 file_id,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag,
		const AosRlbFileType::E file_type,
		const bool create_flag)
{
	AosReliableFilePtr rfile = openRlbFile(file_id, file_type, rdata);
	if(!rfile && create_flag)
	{
		rfile = createRlbFileById(rdata, file_id, fname_prefix, 
				requested_space, file_type, reserve_flag);	
	}
		
	return rfile;
}


AosReliableFilePtr
AosVirtualFileSys::createRlbFileById(
		AosRundata *rdata,
		const u64 file_id,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const AosRlbFileType::E file_type,
		const bool reserve_flag)
{
	AosStorageFileInfo file_info(file_id, rdata->getSiteid(), -1, requested_space, fname_prefix);

	int filter_deviceid = -1;
	bool rslt = addNewFileById(rdata, file_info, filter_deviceid); 
	aos_assert_r(rslt, 0);

	AosReliableFilePtr rfile = AosReliableFile::getReliableFile(
			file_info, file_type, true, rdata);
	aos_assert_r(rfile, 0);
	return rfile;
}


bool
AosVirtualFileSys::removeAllTempFile(AosRundata *rdata)
{
	// Linda 2013/04/18
	// this func called before vfs start.
	aos_assert_r(mVirtualId == AOS_TEMPFILE_VIRTUALID && !mStart, false); 
	for (u32 fseqno = eFileSeqnoStart; fseqno < mFileSeqno; fseqno++)
	{
		removeTempFile(rdata, fseqno);
	}
	
	aos_assert_r(resetAllocFile(), false);
	return true;
}


bool
AosVirtualFileSys::removeTempFile(
		AosRundata *rdata,
		const u32 fseqno)
{
	AosStorageFileInfo file_info;
	u64 offset = eEntryStart + fseqno * eEntrySize;

	AosBuffPtr buff = OmnNew AosBuff(eEntrySize, 0 AosMemoryCheckerArgs);
	mAllocFile->lock();
	bool rslt = mAllocFile->readToBuff(offset, eEntrySize, buff->data());
	mAllocFile->unlock();
	aos_assert_r(rslt, false);

	buff->setDataLen(eEntrySize);
	getFileInfoFromBuff(file_info, buff);

	u64 file_id = ((u64)mVirtualId << 32) + fseqno;
	if((u32)file_id < eFileSeqnoStart && !file_info.isValid())
	{
		return true;
	}

	//OmnScreen << "fseqno: " << (u32)file_id 
	//	<< " , file_info.fileId: " << file_info.fileId 
	//	<<", fname:" << file_info.fname << endl;
	if (file_id == file_info.fileId && file_info.isValid())
	{
		// remove this file info
		u64 offset = eEntryStart + (u32)file_id * eEntrySize;
		char data[eEntrySize];
		memset(data, 0, eEntrySize);
		mAllocFile->lock();
		mAllocFile->put(offset, data, eEntrySize, true);    
		mAllocFile->unlock();

		// free the space.
		/*
		u64 reserved_size = file_info.requestSize;
		int device_id = file_info.deviceId;
		rslt = mStorageMgr->freeSpace(device_id, reserved_size);
		aos_assert_r(rslt, false);
		*/

		// unlink this fname.
		OmnString full_fname = file_info.getFullFname();
		int rs = unlink(full_fname.data());
		aos_assert_r(rs >=0, false);
	}
	return true;
}


bool
AosVirtualFileSys::recover()
{
	aos_assert_r(mStorageMgr->hasNewDevice(), false);
	
	int svr_id = getNextRecoverSvr(-1); 
	if(svr_id == -1)
	{
		OmnScreen << "recover vfs failed; no bkp svr." << endl;
		return true;
	}
	
	mCubeGrpId = AosGetSelfCubeGrpId();
	aos_assert_r(mCubeGrpId != -1, false); 	

	bool rslt;
	int sys_did;
	OmnString virtual_dir = mStorageMgr->getVirtualSysDir(
			mVirtualId, isLocalBackupVFS(), sys_did);
	aos_assert_r(sys_did >=0, false);
	
	set<AosStorageFileInfo> damage_files;
	if(!mStorageMgr->isAvailDevice(sys_did))
	{
		// means the allocFile has damaged. need recover this file first. 
		
		aos_assert_r(mAllocFile, false);
		rslt = recoverAllocFile(damage_files, svr_id);
		aos_assert_r(rslt, false);
		if(svr_id == -1)
		{
			OmnScreen << "recover alloc file failed. no bkp svr" << endl;
			return true;
		}
	}
	else
	{
		rslt = getDamagedFiles(damage_files);
		aos_assert_r(rslt, false);
	}

	rslt = recoverDamagedFiles(damage_files, svr_id);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVirtualFileSys::recoverAllocFile(
		set<AosStorageFileInfo> &damage_files,
		int &svr_id)
{
	aos_assert_r(svr_id >=0, false);
	AosTransPtr trans = OmnNew AosGetTotalFileInfoTrans(svr_id, mCubeGrpId, mVirtualId);

	AosBuffPtr resp;
	bool svr_death;
	bool rslt = AosSendTrans(mRdata, trans, resp, svr_death);
	aos_assert_r(rslt, false);
	if(svr_death)
	{
		svr_id = getNextRecoverSvr(svr_id);
		if(svr_id == -1)
		{
			OmnScreen << "recover vfs failed; no bkp svr." << endl;
			return true;
		}

		return recoverAllocFile(damage_files, svr_id);
	}
	
	aos_assert_r(resp, false);
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	u32 file_num = resp->getU32(0);
	
	//rslt = resetAllocFile();
	//aos_assert_r(rslt && mAllocFile, false);
	set<AosStorageFileInfo> good_files;
	set<AosStorageFileInfo>::iterator itr;
	rslt = mStorageMgr->getGoodFiles(good_files, mVirtualId); 
	aos_assert_r(rslt, false);

	u64 max_file_id = 0;
	AosStorageFileInfo file_info, bkp_file_info;
	bool find;
	for(u32 i=0; i<file_num; i++)
	{
		getFileInfoFromBuff(bkp_file_info, resp);
		u64 file_id = bkp_file_info.fileId; 
		aos_assert_r(file_id, false);
		if(file_id > max_file_id) max_file_id = file_id;

		itr = good_files.find(bkp_file_info);
		if(itr != good_files.end())
		{
			file_info = *itr; 
//OmnScreen << "============================saveToAllocFile" << endl;
			rslt = saveToAllocFile(mRdata.getPtr(), file_info);
			aos_assert_r(rslt, false);

			good_files.erase(itr);
			continue;
		}

		// means this file_id is damaged. need recover.
		rslt = getFromAllocFile(mRdata.getPtr(), file_id, find, file_info);
		aos_assert_r(rslt, false);
		if(find)
		{
			damage_files.insert(file_info);
		}
		else
		{
			bkp_file_info.deviceId = -1;
			damage_files.insert(bkp_file_info);
		}
	}
	
	mFileSeqno = (u32)max_file_id + 1;
	return true;
}


bool
AosVirtualFileSys::getDamagedFiles(set<AosStorageFileInfo> &damage_files)
{
	// read the mAllocFile. and find damage_files.	
	AosBuffPtr file_buff;
	bool rslt = getAllocFileData(file_buff);
	aos_assert_r(rslt, false);
	if(!file_buff)	return true;
	
	u32 max_seq = file_buff->dataLen()/eEntrySize;
	file_buff->setCrtIdx(eEntryStart);
	AosStorageFileInfo file_info;

	for(u32 i=0; i<max_seq; i++)
	{
		getFileInfoFromBuff(file_info, file_buff);
		if(file_info.fileId == 0)
		{
			// means no this file. maybe deleted.
			continue;
		}
		
		int device_id = file_info.deviceId;
		if(mStorageMgr->isAvailDevice(device_id)) continue;

		damage_files.insert(file_info);
	}
	return true;
}


bool
AosVirtualFileSys::getAllocFileData(AosBuffPtr &file_buff)
{
	mAllocFile->lock();
	u32 file_len = mAllocFile->getLength();
	mAllocFile->unlock();
	if(file_len <= eEntryStart)
	{
		// means this svr also has no data.
		return true;
	}
	file_buff = OmnNew AosBuff(file_len, 0 AosMemoryCheckerArgs);

	int bytesread = mAllocFile->readToBuff(0, file_len, file_buff->data());
	mAllocFile->unlock();
	aos_assert_r(bytesread >0 && (u32)bytesread == file_len, false); 
	
	bool data_isgood = AosCheckCheckSum(file_buff->data(), file_len);
	if(!data_isgood)
	{
		// means this file has damaged.
		OmnAlarm << "Alloc File damaged." << enderr;
		return false;
	}

	file_buff->setDataLen(file_len);   
	u32 max_seq = file_len/eEntrySize;
	aos_assert_r(mFileSeqno == max_seq, false);
	return true;
}


bool
AosVirtualFileSys::recoverDamagedFiles(
		set<AosStorageFileInfo> &damage_files,
		int &svr_id)
{
	aos_assert_r(svr_id >=0, false);
	
	u32 total_num = damage_files.size();
	set<AosStorageFileInfo>::iterator itr = damage_files.begin();
	set<AosStorageFileInfo>::iterator tmp_itr;
	bool svr_death, rslt;
	for(u32 i=0; i<total_num; i++)
	{
		AosStorageFileInfo file_info = *itr;;
		
		rslt = recoverDamagedFile(file_info, svr_id, svr_death); 
		aos_assert_r(rslt, false);
		if(svr_death)
		{
			svr_id = getNextRecoverSvr(svr_id);
			if(svr_id == -1)
			{
				OmnScreen << "recover vfs failed; no bkp svr." << endl;
				return true;
			}

			return recoverDamagedFiles(damage_files, svr_id);
		}
	
		tmp_itr = itr;
		itr++;
		damage_files.erase(tmp_itr);
	}
	
	return true;
}


OmnFilePtr
AosVirtualFileSys::createRecoverFile(AosStorageFileInfo &file_info)
{
	int device_id = file_info.deviceId;
	if(device_id != -1)
	{
		OmnScreen << "this damaged file maybe has created."
			<< "; new_device_id:" << device_id
			<< "; new_fname:" << file_info.fname
			<< endl;
	
		OmnString full_fname = file_info.getFullFname();
		OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);
		if(!file->isGood())
		{
			OmnAlarm << "failed to create file" << full_fname << enderr;
			return 0;
		}
		return file;
	}
	
	OmnString sub_dir_name;
	u32 old_site_id = mRdata->getSiteid();
	mRdata->setSiteid(file_info.siteId);
	bool rslt = mStorageMgr->allocSpaceFromNewDisk(mRdata.getPtr(), mVirtualId, 
			file_info.siteId, file_info.requestSize, device_id, sub_dir_name);
	aos_assert_r(rslt && sub_dir_name != "" && device_id >=0, 0);	
	mRdata->setSiteid(old_site_id);
	
	// 2. generate fileName.
	OmnString new_fname = sub_dir_name;	
	new_fname << file_info.getPureFname();
	if(new_fname.length() > eMaxNameLen)
	{
		OmnAlarm << "file_name too long: " << new_fname << enderr;
		return 0;
	}
	
	file_info.deviceId = device_id;
	file_info.fname = new_fname;
	//file_info.fname = file_info.getFullFname();

//OmnScreen << "============================saveToAllocFile" << endl;
	rslt = saveToAllocFile(mRdata.getPtr(), file_info);
	aos_assert_r(rslt, 0);
	
	OmnString full_fname = file_info.getFullFname();
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);
	if(!file->isGood())
	{
		OmnAlarm << "failed to create file" << full_fname << enderr;
		return 0;
	}
	return file;
}


bool
AosVirtualFileSys::recoverDamagedFile(
		AosStorageFileInfo &file_info,
		int &svr_id, 
		bool &svr_death)
{
	u64 file_id = file_info.fileId;
	aos_assert_r(file_id > 0, false);
		
	OmnFilePtr file = createRecoverFile(file_info);
	aos_assert_r(file, false);
	
	OmnScreen << "----- recover damaged file."
		<< "; virtual_id:" << mVirtualId
		<< "; fileId:" << file_id 
		<< "; fname:" << file->getFileName()
		<< endl;

	u64 file_off = 0;
	bool finish = false, rslt;
	AosBuffPtr resp;
	while(!finish)
	{
		AosTransPtr trans = OmnNew AosRecoverFileTrans(svr_id, mCubeGrpId, file_id, file_off);
		rslt = AosSendTrans(mRdata, trans, resp, svr_death);
		aos_assert_r(rslt, false);
		if(svr_death)	return true;

		aos_assert_r(resp, false);
		rslt = resp->getU8(0);
		aos_assert_r(rslt, false);
		
		finish = resp->getU8(0);
		u64 data_len = resp->getU64(0);
		if(!data_len)
		{
			aos_assert_r(finish, false);
			continue;
		}

		AosBuffPtr file_data = resp->getBuff(data_len, false AosMemoryCheckerArgs);
		aos_assert_r(file_data, false);

		rslt = file->put(file_off, file_data->data(), data_len, true); 
		aos_assert_r(rslt, false);
		
		file_off += data_len;
	}
	
	OmnScreen << "----- recover damaged file finish."
		<< "; virtual_id:" << mVirtualId
		<< "; file_id:" << file_id 
		<< endl;
	return true;
}


int
AosVirtualFileSys::getNextRecoverSvr(const int crt_sid)
{
	// first sync from bkp. if not succ. recover from crt_master.
	u32 cube_grp_id =  AosGetSelfCubeGrpId();
	int crt_master = AosGetSelfCrtMaster();
	aos_assert_r(crt_master != AosGetSelfServerId(), false);

	if(crt_sid == crt_master)
	{
		// the crt_master is the last sync.
		return -1;
	}

	int next_sid = crt_sid;
	while(1)
	{
		next_sid = AosGetNextSvrId(cube_grp_id, next_sid);
	
		if(next_sid != AosGetSelfServerId() && next_sid != crt_master) break;
	}

	if(next_sid == -1)
	{
		// try to crt_master 
		next_sid = crt_master;
	}
	return next_sid;
}


bool
AosVirtualFileSys::getTotalFileInfo(
		AosRundata *rdata,
		AosBuffPtr &data)
{
	// This func called when some svr recover disk.
	//
	AosBuffPtr file_buff;
	getAllocFileData(file_buff); 
	if(!file_buff)
	{
		data = OmnNew AosBuff(10, 0 AosMemoryCheckerArgs);
		data->setU32(0);
		return true;
	}
	
	u32 max_seq = file_buff->dataLen()/eEntrySize;
	file_buff->setCrtIdx(eEntryStart);
	AosStorageFileInfo file_info;

	vector<AosStorageFileInfo> total_file;
	for(u32 i=0; i<max_seq; i++)
	{
		u64 offset = eEntryStart + i * eEntrySize;
		file_buff->setCrtIdx(offset);

		getFileInfoFromBuff(file_info, file_buff);
		if(file_info.fileId == 0)
		{
			// means no this file. maybe deleted.
			continue;
		}
		total_file.push_back(file_info);
	}

	data = OmnNew AosBuff(file_buff->dataLen() + 10, 0 AosMemoryCheckerArgs);
	memset(data->data(), 0, data->buffLen());
	u32 file_num = total_file.size();
	data->setU32(file_num);
	for(u32 i=0; i< file_num; i++)
	{
		setFileInfoToBuff(data, total_file[i]);
	}
	return true;
}


OmnString
AosVirtualFileSys::getVirtualInfo(AosRundata *rdata)
{
	OmnNotImplementedYet;
	return "";
	/*
	vector<AosStorageFileInfo> total_files;
	getTotalFileInfo(rdata, total_files);
	u64 alloced_size = 0;
	for(u32 i=0; i<total_files.size(); i++)
	{
		u64 req_size = total_files[i].requestSize;
		alloced_size += req_size;
	}

	u32 k = 1024 * 1024;
	OmnString cont = "<virtual ";
	cont << "vid=\"" << mVirtualId << "\" "
		<< "server_id=\"" << AosGetSelfServerId() << "\" "
		<< "allocedsize=\"" << (alloced_size/k) << "MB\" "
		<< "/>";
	return cont;
	*/
}


