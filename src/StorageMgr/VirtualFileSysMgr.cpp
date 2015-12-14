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
// 12/13/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StorageMgr/VirtualFileSysMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "ReplicMgr/ReplicPolicy.h"
#include "SEUtil/SystemIdNames.h"
#include "SEUtil/SeConfig.h"
#include "StorageMgr/StorageMgr.h"
#include "StorageMgr/FileId.h"
#include "StorageMgr/IdPrefix.h"
#include "StorageMgr/SpaceAlloc.h"
#include "StorageMgr/SystemId.h"
#include "StorageMgr/VirtualFileSys.h"
#include "DocFileMgr/RaidFile.h"

#include <sys/stat.h>
#include <dirent.h>

void 
CacheFile::setRundata(AosRundata *rdata)
{
	mRundata = rdata;
}

OmnLocalFilePtr
CacheFile::getData(const u64 file_id)
{
	AosVfsMgrObjPtr vfs_mgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfs_mgr, 0);

	if (AosIsRaidFile(file_id))
	{
		u64 fileId = file_id & 0x7fffffffffffffffLL;
		OmnFilePtr file = vfs_mgr->openRawFile(fileId, mRundata.getPtr());
		aos_assert_r(file && file->isGood(), 0);
		OmnLocalFilePtr local_file = OmnNew AosRaidFile(mRundata, file);
		aos_assert_r(local_file->isGood(), 0);
		return local_file;
	}
	OmnFilePtr file = vfs_mgr->openRawFile(file_id, mRundata.getPtr());
	aos_assert_r(file && file->isGood(), 0);
	return file;
}


OmnLocalFilePtr  
CacheFile::getData(const u64 file_id, const OmnLocalFilePtr &file)
{
	AosVfsMgrObjPtr vfs_mgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfs_mgr, 0);

	if (AosIsRaidFile(file_id))
	{
		u64 fileId = file_id & 0x7fffffffffffffffLL;
		OmnFilePtr ff = vfs_mgr->openRawFile(fileId, mRundata.getPtr());
		aos_assert_r(ff && ff->isGood(), 0);
		OmnLocalFilePtr local_file = OmnNew AosRaidFile(mRundata, ff);
		aos_assert_r(local_file->isGood(), 0);
		return local_file;
	}
	OmnFilePtr ff = vfs_mgr->openRawFile(file_id, mRundata.getPtr());
	aos_assert_r(ff && ff->isGood(), 0);
	return ff;
}


AosVirtualFileSysMgr::AosVirtualFileSysMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosVirtualFileSysMgr::~AosVirtualFileSysMgr()
{
}


bool
AosVirtualFileSysMgr::stop()
{
	map<u32, AosVirtualFileSysPtr>::iterator iter = mVirtualFileSys.begin();
	for(; iter != mVirtualFileSys.end(); iter++)
	{
		iter->second->startStop();
	}

	map<u32, AosVirtualFileSysPtr>::iterator itr = mVirtualFileSys.begin();
	for(; itr != mVirtualFileSys.end(); itr++)
	{
		itr->second->stop();
	}
	return true;
}


bool
AosVirtualFileSysMgr::config(const AosXmlTagPtr &config)
{
	// 1. init mStorageMgr
	bool rslt;
	mStorageMgr = OmnNew AosStorageMgr(); 
	rslt = mStorageMgr->config(config);
	aos_assert_r(rslt, false);

	// 2. init mVirtualFileSys
	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);

	if(!AosIsSelfCubeSvr())
	{
		aos_assert_r(total_vids.size() == 0,false);
		
		// the FrontEnd svr. use virtual 0 to save tmp file.
		total_vids.push_back(0);
	}
	
	for(u32 i=0; i < total_vids.size(); i++)
	{
		u32 virtual_id = total_vids[i];
		AosVirtualFileSysPtr vfs = OmnNew AosVirtualFileSys(virtual_id, mStorageMgr);
	
		mVirtualFileSys.insert(make_pair(virtual_id, vfs));
	}

	//Linda 2013/04/18
	u32 virtual_id = AOS_TEMPFILE_VIRTUALID;
	AosVirtualFileSysPtr vfs = OmnNew AosVirtualFileSys(virtual_id, mStorageMgr);
	mVirtualFileSys.insert(make_pair(virtual_id, vfs));
	
	return true;
}


bool
AosVirtualFileSysMgr::diskCheck()
{
	aos_assert_r(AosIsSelfCubeSvr(), false);
	aos_assert_r(AosGetReplicPolicyType() != AosReplicPolicyType::eNoBkp, false);
	
	bool rslt;
	bool has_new = mStorageMgr->hasNewDevice();
	if(!has_new) return true;

	OmnScreen << "VfsMgr; SWH; try to recover disk." << endl;
	AosVirtualFileSysPtr vfs;
	u32 cube_id;
	map<u32, AosVirtualFileSysPtr>::iterator itr = mVirtualFileSys.begin();
	for(; itr != mVirtualFileSys.end(); itr++)
	{
		cube_id = itr->first;
		if(cube_id == AOS_TEMPFILE_VIRTUALID)	continue;

		vfs = itr->second;
		rslt = vfs->recover();
		aos_assert_r(rslt, false);
	}

	rslt = mStorageMgr->setNewDiskAvail();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVirtualFileSysMgr::start()
{
	//Linda, 2013/09/16
	//AosIILSaveDfmLog::getSelf()->start();
	
	if(mVirtualFileSys.size() ==0)
	{
		OmnScreen << "This server has no virtual!" << endl;
		return true;
	}

	bool rslt;
	AosRundataPtr rdata = OmnApp::getRundata();
	AosVirtualFileSysPtr vfs;
	u32 vid;
	map<u32, AosVirtualFileSysPtr>::iterator itr = mVirtualFileSys.begin();
	for(; itr != mVirtualFileSys.end(); itr++)
	{
		vid = itr->first;
		vfs = itr->second;
		
		if(vid == AOS_TEMPFILE_VIRTUALID)
		{
			//Linda 2013/04/19
			rslt = vfs->removeAllTempFile(rdata.getPtr());
			aos_assert_r(rslt, false);
		}
		vfs->start();
	}
	
	rslt = mStorageMgr->setNewDiskAvail();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVirtualFileSysMgr::clean()
{
	// to clean the DfmLog.
	if(mVirtualFileSys.size() ==0)
	{
		OmnScreen << "This server has no virtual!" << endl;
		return true;
	}

	AosVirtualFileSysPtr vfs;
	map<u32, AosVirtualFileSysPtr>::iterator itr = mVirtualFileSys.begin();
	for(; itr != mVirtualFileSys.end(); itr++)
	{
		vfs = itr->second;
		vfs->clean();
	}
	return true;
}


bool
AosVirtualFileSysMgr::switchToBkp()
{
	if(mVirtualFileSys.size() ==0)
	{
		OmnScreen << "This server has no virtual!" << endl;
		return true;
	}

	AosVirtualFileSysPtr vfs;
	map<u32, AosVirtualFileSysPtr>::iterator itr = mVirtualFileSys.begin();
	for(; itr != mVirtualFileSys.end(); itr++)
	{
		vfs = itr->second;
		vfs->switchToBkp();
	}
	return true;
}


bool
AosVirtualFileSysMgr::reStart()
{
	// when called switchToBkp. but master switchToMaster failed.
	// then will call this func.
	if(mVirtualFileSys.size() ==0)
	{
		OmnScreen << "This server has no virtual!" << endl;
		return true;
	}

	AosVirtualFileSysPtr vfs;
	map<u32, AosVirtualFileSysPtr>::iterator itr = mVirtualFileSys.begin();
	for(; itr != mVirtualFileSys.end(); itr++)
	{
		vfs = itr->second;
		vfs->reStart();
	}
	return true;
}


AosDocFileMgrObjPtr
AosVirtualFileSysMgr::retrieveDocFileMgr(
		AosRundata *rdata,
		const u32	virtual_id,
		const AosDfmConfig &config)
		//const AosModuleId::E module_id,
		////const u32 siteid,
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	//return vfs->retrieveDocFileMgr(module_id, siteid, proc, rdata); 
	return vfs->retrieveDocFileMgr(rdata, config); 
}


AosDocFileMgrObjPtr
AosVirtualFileSysMgr::createDocFileMgr(
		AosRundata *rdata,
		const u32 virtual_id,
		const AosDfmConfig &config)
		//const AosModuleId::E module_id,
		////const u32 siteid, 
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	//return vfs->createDocFileMgr(module_id, siteid, proc, rdata); 
	return vfs->createDocFileMgr(rdata, config); 
}


bool
AosVirtualFileSysMgr::removeDocFileMgr(
		AosRundata *rdata,
		const u32 virtual_id,
		const AosModuleId::E module_id)
		//const u32 siteid)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, false);
	//return vfs->removeDocFileMgr(module_id, siteid, rdata); 
	return vfs->removeDocFileMgr(rdata, module_id); 
}
	

AosDocFileMgrObjPtr
AosVirtualFileSysMgr::retrieveDocFileMgrByKey(
		AosRundata *rdata,
		const u32   virtual_id,
		const OmnString dfm_key,
		const bool create_flag,
		const AosDfmConfig &config)
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	return vfs->retrieveDocFileMgrByKey(rdata, dfm_key, create_flag, config);	
}


AosDocFileMgrObjPtr
AosVirtualFileSysMgr::createDocFileMgrByKey(
		AosRundata *rdata,
		const u32   virtual_id,
		const OmnString dfm_key, 
		const AosDfmConfig &config)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	return vfs->createDocFileMgrByKey(rdata, dfm_key, config);	
}


AosDocFileMgrObjPtr
AosVirtualFileSysMgr::createDocFileMgr(
		AosRundata *rdata,
		const u32 virtual_id,
		const u32 dfm_id,
		const AosDfmConfig &config)
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	return vfs->createDocFileMgrById(rdata, dfm_id, config);
}


AosDocFileMgrObjPtr
AosVirtualFileSysMgr::retrieveDocFileMgr(
		AosRundata *rdata,
		const u32 virtual_id,
		const u32 dfm_id,
		const AosDfmConfig &config)
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	return vfs->retrieveDocFileMgrById(rdata, dfm_id, config);
}


AosXmlTagPtr
AosVirtualFileSysMgr::getConfigDoc(const u32 virtual_id, const u64 &docid, AosRundata *rdata)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	return vfs->getConfigDoc(docid, rdata); 
}


bool
AosVirtualFileSysMgr::saveConfigDoc(const u32 virtual_id, const u64 &docid, const AosXmlTagPtr &doc, AosRundata *rdata)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	return vfs->saveConfigDoc(docid, doc, rdata); 
}


// Ketty 2012/07/20
OmnFilePtr
AosVirtualFileSysMgr::createNewFile(
		AosRundata *rdata,
		const u32 virtual_id,
		u64 &file_id,
		const OmnString &fname_prefix,
		const u64 &request_space,
		const bool reserve_flag)
{
	// This function determines whether it can create a new file for the
	// requester, which is determined by the Storage Management Policy.
	
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);

	AosStorageFileInfo file_info;
	bool rslt = vfs->addNewFile(rdata, fname_prefix,
			request_space, reserve_flag, file_info);
	aos_assert_r(rslt, 0);

	file_id = file_info.fileId;
	OmnString full_fname = file_info.getFullFname();
	// 2. new the file
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);
	if(!file->isGood())
	{
		AosSetErrorU(rdata, "failed_create_file") << ": " << full_fname << enderr;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;	
		
	}
	return file;
}


// Ketty 2012/08/25
AosReliableFilePtr 
AosVirtualFileSysMgr::createRlbFile(
		AosRundata *rdata,
		const u32 virtual_id,
		//u64 &file_id,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const AosRlbFileType::E file_type,
		const bool reserve_flag)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	
	//return vfs->createRlbFile(rdata, file_id, fname_prefix,
	//		requested_space, file_type, reserve_flag);
	return vfs->createRlbFile(rdata, fname_prefix,
			requested_space, file_type, reserve_flag);
}


AosReliableFilePtr
AosVirtualFileSysMgr::createRlbFileById(
		AosRundata *rdata,
		const u32 virtual_id,
		const u64 file_id,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const AosRlbFileType::E file_type,
		const bool reserve_flag)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	
	return vfs->createRlbFileById(rdata, file_id, fname_prefix,
			requested_space, file_type, reserve_flag);
}


AosReliableFilePtr 
AosVirtualFileSysMgr::openRlbFile(
		const u64 &file_id,
		const AosRlbFileType::E file_type,
		AosRundata *rdata)
{
	// 'file_id' is an ID. Its upper four bytes are used to identify
	// VirtualId and lower four bytes are the ID used by the VirtualFileSys 
	// to uniquely identify a file. 

	u32 virtual_id = (u32)(file_id>> 32);
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	
	return vfs->openRlbFile(file_id, file_type, rdata);
}

	
AosReliableFilePtr
AosVirtualFileSysMgr::openRlbFile(
		AosRundata *rdata,
		const u32 virtual_id,
		const u64 file_id,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag,
		const AosRlbFileType::E file_type,
		const bool create_flag)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	
	return vfs->openRlbFile(rdata, file_id, fname_prefix,
			requested_space, reserve_flag, file_type, create_flag);
}


OmnLocalFilePtr
AosVirtualFileSysMgr::openLocalFile(
		const u64 &file_id,
		AosRundata *rdata)
{
	mCache.setRundata(rdata);
	OmnLocalFilePtr file = mLRUCache.get(file_id, mCache); 
	if(!file || !file->isGood())
	{
		file = openFilePriv(file_id, rdata);
		mLRUCache.remove(file_id);
		return file;
	}
	return file;
}

/*
OmnLocalFilePtr
AosVirtualFileSysMgr::openLocalFile(
		const u64 &file_id,
		AosRundata *rdata)
{
	// 'file_id' is an ID. Its upper four bytes are used to identify
	// VirtualId and lower four bytes are the ID used by the VirtualFileSys 
	// to uniquely identify a file. 

	//Linda, 2013/05/17
	mCache.setRundata(rdata);
	if (AosIsRaidFile(file_id))
	{
		u64 fileId  = file_id & 0x7fffffffffffffffLL;

		//OmnFilePtr file = openFilePriv(fileId, rdata);
		OmnFilePtr file = mLRUCache.get(fileId, mCache);
		if(!file || !file->isGood())
		{
		    file = openFilePriv(fileId, rdata);
			mLRUCache.remove(fileId);
		}
		aos_assert_r(file, 0);

		OmnLocalFilePtr local_file = OmnNew AosRaidFile(rdata, file);
		aos_assert_r(local_file->isGood(), 0);

		return local_file;
	}

	//OmnFilePtr file = openFilePriv(file_id, rdata);
	OmnFilePtr file = mLRUCache.get(file_id, mCache); 
	if(!file || !file->isGood())
	{
		file = openFilePriv(file_id, rdata);
		mLRUCache.remove(file_id);
	}
	return file;
}
*/


OmnFilePtr
AosVirtualFileSysMgr::openFilePriv(
		const u64 &file_id,
		AosRundata *rdata)
{
	// 1. find the file_name by file_id.
	u32 virtual_id = (u32)(file_id >> 32);
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);

	OmnString fname = vfs->getFileName(rdata, file_id); 
	aos_assert_r(fname != "", 0);
	
	// 2. open the file.
	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!file->isGood())
	{
		OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
		AosSetErrorU(rdata, "failed_open_file_001") << ": " << file_id
			<< ". " << AOSTERM_U("filename", rdata) << ": " << fname;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	return file;
}


bool
AosVirtualFileSysMgr::removeFile(
		const u64 file_id,
		AosRundata *rdata)
{
	//Linda, 2013/05/17
	u64 fileId = file_id;
	if (AosIsRaidFile(file_id))
	{
		fileId = file_id & 0x7fffffffffffffffLL;
		removeRaidFilePriv(fileId, rdata);
	}

	u32 virtual_id = (u32)(fileId >> 32);
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);

	mLRUCache.remove(fileId);
	bool rt = vfs->removeFile(rdata, fileId);
	aos_assert_r(rt, false);
	return true;
}


OmnFilePtr	
AosVirtualFileSysMgr::openFileByKey(
		const u32 virtual_id,
		const OmnString &str_key,
		const bool create_flag,
		AosRundata *rdata AosMemoryCheckDecl)
{
	OmnFilePtr file = openFileByStrKey(virtual_id, str_key, rdata AosMemoryCheckerFileLine);
	if (!create_flag) return file;
	if (!file)
	{
		file = createFileByStrKey(virtual_id, str_key, 0, false, rdata AosMemoryCheckerFileLine);
	}
	return file;
}


OmnFilePtr
AosVirtualFileSysMgr::createFileByStrKey(
		const u32 virtual_id,
		const OmnString &str_key,
		const u64 &requested_space,
		const bool reserve_flag,
		AosRundata *rdata AosMemoryCheckDecl)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);

	AosStorageFileInfo file_info;
	bool rslt = vfs->addNewFileByStrKey(rdata, str_key, requested_space, 
			reserve_flag, file_info);
	aos_assert_r(rslt, 0);

	// 2. new the file
	OmnString full_fname = file_info.getFullFname();
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerFileLine);
	if(!file->isGood())
	{
		AosSetErrorU(rdata, "failed_create_file") << ": " << full_fname << enderr;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;	
		
	}
	return file;
}


OmnFilePtr
AosVirtualFileSysMgr::openFileByStrKey(
		const u32 virtual_id,
		const OmnString &str_key,
		AosRundata *rdata AosMemoryCheckDecl)
{
	// This class uses a string hash map to map string keys to 
	// file id. This function uses the hash map to map the key.
	// If no entry is found, it returns 0. Otherwise, it uses
	// the mapped file id to open the file.
	//
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);

	bool exist = false;
	AosStorageFileInfo file_info;
	bool rslt = vfs->getFileInfoByStrKey(rdata, str_key, file_info, exist); 
	aos_assert_r(rslt, 0);
	if(!exist)	return 0;

	OmnString fname = file_info.getFullFname();
	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerFileLine);
	if (!file->isGood())
	{
		AosSetErrorU(rdata, "failed_open_file") << ": " << ": " << fname;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;	
	}

	return file;
}


OmnString
AosVirtualFileSysMgr::getFileName(
		const u64 &file_id,
		AosRundata *rdata)
{
	// 1. find the virtual_id by file_id.
	u32 virtual_id = (u32)(file_id>> 32);
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);

	OmnString fname = vfs->getFileName(rdata, file_id); 
	aos_assert_r(fname != "", 0);
	return fname;
}
	

AosReliableFilePtr
AosVirtualFileSysMgr::openRlbFileByStrKey(
		AosRundata *rdata,
		const u32 virtual_id,
		const OmnString &str_key,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag,
		const AosRlbFileType::E file_type,
		const bool create_flag)
{
	// Ketty 2014/03/05
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	
	AosReliableFilePtr file = openRlbFileByStrKey(rdata, virtual_id, str_key, file_type);
	if(file)	return file;
	if (!create_flag) return 0;
	
	return createRlbFileByStrKey(rdata, virtual_id, str_key, file_type, 0, false);
}


AosReliableFilePtr
AosVirtualFileSysMgr::createRlbFileByStrKey(
		AosRundata *rdata,
		const u32 virtual_id,
		const OmnString &str_key,
		const AosRlbFileType::E file_type,
		const u64 &requested_space,
		const bool reserve_flag)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);

	AosStorageFileInfo file_info;
	bool rslt = vfs->addNewFileByStrKey(rdata, str_key, requested_space, 
			reserve_flag, file_info);
	aos_assert_r(rslt, 0);

	AosReliableFilePtr rfile = AosReliableFile::getReliableFile(
			file_info, file_type, true, rdata);
	aos_assert_r(rfile, 0);
	return rfile;
}


AosReliableFilePtr 
AosVirtualFileSysMgr::openRlbFileByStrKey(
		AosRundata *rdata,
		const u32 virtual_id,
		const OmnString &str_key,
		const AosRlbFileType::E file_type)
{
	// Ketty 2014/03/05
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);
	
	bool exist = false;
	AosStorageFileInfo file_info;
	bool rslt = vfs->getFileInfoByStrKey(rdata, str_key, file_info, exist); 
	aos_assert_r(rslt, 0);
	if(!exist)	return 0;

	AosReliableFilePtr rfile = AosReliableFile::getReliableFile(
			file_info, file_type, false, rdata);
	aos_assert_r(rfile, 0);
	return rfile;
}


bool
AosVirtualFileSysMgr::removeFileByStrKey(
		AosRundata *rdata,
		const u32 virtual_id,
		const OmnString &str_key)
{
	// Ketty 2014/03/05
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);

	bool exist = false;
	u64 file_id;
	bool rslt = vfs->removeFileByStrKey(rdata, str_key, file_id, exist); 
	aos_assert_r(rslt, 0);
	if(!exist)	return 0;
	
	aos_assert_r(file_id, false);
	mLRUCache.remove(file_id);
	return true;
}

/*
bool
AosVirtualFileSysMgr::isBackupVirtual(const u32 virtual_id, bool & is_backup)
{
	//Linda 2013/04/19
	if (virtual_id == AOS_TEMPFILE_VIRTUALID)
	{
		is_backup = false;
		return true;
	}
	int svr_id = AosGetMainSvrId(virtual_id);
	aos_assert_r(svr_id != -1, false);
	
	is_backup = false;
	if(svr_id == AosGetSelfServerId())
	{
		return true;
	}

	svr_id = AosGetBackupSvrId(virtual_id);
	if(svr_id == -1)
	{
		// means this svr has no bkp.		
		return true;
	}
	
	if(svr_id == AosGetSelfServerId())
	{
		is_backup = true;
	}
	
	return true;
}
*/


AosVirtualFileSysPtr 
AosVirtualFileSysMgr::getVirtualFileSys( const u32 virtual_id)
{
	// Each virtual machine has a AosVirtualFileSys. It is used to manage
	// all the files created by that virtual machine.

	// the other server will sync data with this server. 
	// but this server's mVirtualFileSys maybe not inited yet.

	AosVirtualFileSysPtr vfs;
	mLock->lock();
	map<u32, AosVirtualFileSysPtr>::iterator itr = mVirtualFileSys.find(virtual_id);
	if(itr != mVirtualFileSys.end())
	{
		vfs = itr->second;
		mLock->unlock();
		return vfs;
	}
	
	mLock->unlock();
	return 0;
}


bool
AosVirtualFileSysMgr::getFileInfo(
		const u64 &file_id, 
		AosStorageFileInfo &file_info,
		bool &find,
		AosRundata *rdata)
{
	u32 virtual_id = (u32)(file_id>> 32);
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	if (!vfs)
	{
		AosSetErrorU(rdata, "virtual_file_sys_not_found") << ": " << virtual_id << enderr;
		return false;
	}
	
	return vfs->getFileInfo(rdata, file_id, find, file_info);
}


bool
AosVirtualFileSysMgr::addFileByFileId(
		AosRundata *rdata,
		AosStorageFileInfo &file_info,
		const int filter_deviceid)
{
	u32 virtual_id = (u32)(file_info.fileId >>32);
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0); 

	bool rslt = vfs->addNewFileById(rdata, file_info, filter_deviceid);	
	aos_assert_r(rslt, false);
	return true;
}


OmnString
AosVirtualFileSysMgr::getBaseDir(const u32 device_id)
{
	return mStorageMgr->getBaseDir(device_id);
}


OmnFilePtr  
AosVirtualFileSysMgr::createNewTempFile(          
		AosRundata *rdata,
		u64 &file_id,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag)
{
	//Linda 2013/04/19
	u32 virtual_id = AOS_TEMPFILE_VIRTUALID; 
	return createNewFile(rdata, virtual_id, file_id,
		fname_prefix, requested_space, reserve_flag);
}


// Linda 2013/05/14
OmnLocalFilePtr
AosVirtualFileSysMgr::createRaidFile(
		AosRundata *rdata,
		const u32 virtual_id,
		u64 &file_id,
		const OmnString &fname_prefix,
		const u64 &request_space,
		const bool reserve_flag)
{
	// This function determines whether it can create a new file for the
	// requester, which is determined by the Storage Management Policy.
	
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, 0);

	int device_num = mStorageMgr->getDeviceNum();
	aos_assert_r(device_num > 0, 0);
	
	vector<AosStorageFileInfo> fileinfos;
	u64 file_space = request_space / device_num;
	u64 space = file_space + (request_space % device_num);

	for (int i=0; i<device_num; i++)
	{
		AosStorageFileInfo file_info;
		bool rslt = vfs->addNewFile(rdata, fname_prefix,
				space, reserve_flag, file_info, i);
		aos_assert_r(rslt, 0);

		OmnString full_fname = file_info.getFullFname();
		OmnFilePtr ff = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);
		if (!ff->isGood())
		{
			AosSetErrorU(rdata, "failed_create_file") << ":" << full_fname << enderr;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;	
		}

		aos_assert_r(file_info.deviceId == i, 0);
		fileinfos.push_back(file_info);
		space = file_space;
	}
	
	u64 fileId = 0;
	u64 indexfile_space = AosRaidFile::eIndexFileSize;
	OmnFilePtr ff = createNewFile(rdata, virtual_id,
		fileId, fname_prefix, indexfile_space, reserve_flag); 
	file_id = fileId | 0x8000000000000000LL; 

	// create raid File
	OmnLocalFilePtr local_file = OmnNew AosRaidFile(rdata, ff, fileinfos); 
	aos_assert_r(local_file->isGood(), 0);

	return local_file;
}


bool
AosVirtualFileSysMgr::removeRaidFilePriv(
		const u64 &file_id,
		AosRundata *rdata)
{
	u32 virtual_id = (u32)(file_id >> 32);
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, false);

	OmnString fname = vfs->getFileName(rdata, file_id);
	//Jozhi 2014-04-09 task to remove this file, but file has remove before;
	//aos_assert_r(fname != "", 0);
	if (fname != "" && access(fname.data(), F_OK) == 0)
	{
		// 2. open the file.
		OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
		if (!file->isGood())
		{
			AosSetErrorU(rdata, "failed_open_file_001") << ": " << file_id
				<< ". " << AOSTERM_U("filename", rdata) << ": " << fname;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		OmnLocalFilePtr local_file = OmnNew AosRaidFile(rdata, file);
		aos_assert_r(local_file->isGood(), false);

		local_file->deleteFile(rdata);
	}
	return true;
}


bool
AosVirtualFileSysMgr::rebindFile(
		AosStorageFileInfo &file_info,
		AosRundata *rdata)
{
	int filter_deviceid = file_info.deviceId;
	bool rslt = addFileByFileId(rdata, file_info, filter_deviceid); 
	aos_assert_r(rslt, false);

	return true;
}


bool
AosVirtualFileSysMgr::getTotalFileInfo(
		AosRundata *rdata, 
		const u32 virtual_id,
		AosBuffPtr &data_buff)
{
	AosVirtualFileSysPtr vfs = getVirtualFileSys(virtual_id);
	aos_assert_r(vfs, false);

	vfs->getTotalFileInfo(rdata, data_buff);
	return true;
}


/*
bool
AosVirtualFileSysMgr::getTotalFileInfo(
		AosRundata *rdata, 
		const u32 disk_id, 
		vector<AosStorageFileInfo> &total_files) 
{
	AosVirtualFileSysPtr vfs;
	map<u32, AosVirtualFileSysPtr>::iterator itr =	mVirtualFileSys.begin();
	for(; itr != mVirtualFileSys.end(); itr++)
	{
		vfs = itr->second;	
		vfs->getTotalFileInfo(rdata, total_files, disk_id);
	}

	return true;
}


bool
AosVirtualFileSysMgr::checkSpace(const u64 need_space, bool &enough)
{
	return mStorageMgr->checkSpace(need_space, enough);
}


bool
AosVirtualFileSysMgr::freeSpace( const int device_id, const u64 free_size)
{
	return mStorageMgr->freeSpace(device_id, free_size);
}


bool
AosVirtualFileSysMgr::deleteFiles(const vector<AosStorageFileInfo> &total_files)
{
	return mStorageMgr->deleteFiles(total_files);
}
*/


