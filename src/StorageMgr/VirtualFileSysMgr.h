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
//	This class is used to manage a group of files, used to store 
//	either IILs or Documents. There are a number of files in the
//  group, each with a unique sequence number. Each file can 
//  store up to a given amount of data. 
//	
// Modification History:
// 12/13/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgr_VirtualFileSysMgr_h
#define AOS_StorageMgr_VirtualFileSysMgr_h

#include "DocFileMgr/DocFileMgr.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "ReliableFile/ReliableFile.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "StorageMgr/ConfigMgr.h"
#include "StorageMgr/DiskAlloc.h"
#include "StorageMgr/Ptrs.h"
#include "StorageMgrUtil/FileInfo.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "UtilData/ModuleId.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/LRUCache.h"
#include "Util/Ptrs.h"
#include "Util/File.h"


class CacheFile
{
public:
	AosRundataPtr 			mRundata;

	void setRundata(AosRundata *rdata);

	OmnLocalFilePtr getData(const u64 file_id);

	OmnLocalFilePtr getData(const u64 file_id, const OmnLocalFilePtr &file);
};



class AosVirtualFileSysMgr : public AosVfsMgrObj
{
	OmnDefineRCObject;

private:
	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	AosStorageMgrPtr		mStorageMgr;
	map<u32, AosVirtualFileSysPtr>	mVirtualFileSys;

	AosLRUCache<u64, OmnLocalFilePtr>    mLRUCache;
	CacheFile						mCache;

public:
	AosVirtualFileSysMgr();
	~AosVirtualFileSysMgr();

	virtual bool        config(const AosXmlTagPtr &def);
	virtual bool 		clean();
	virtual bool        start();
	virtual bool        stop();
	virtual bool 		switchToBkp();
	virtual bool 		reStart();

	virtual AosDocFileMgrObjPtr retrieveDocFileMgr(
				AosRundata *rdata, 
				const u32	virtual_id,
				const AosDfmConfig &config);
				//const AosModuleId::E module_id,
				////const u32 siteid,
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata);      

	virtual AosDocFileMgrObjPtr createDocFileMgr(
				AosRundata *rdata, 
				const u32	virtual_id,
				const AosDfmConfig &config);
			    //const AosModuleId::E module_id,
				////const u32 siteid,
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata);

	virtual bool 	removeDocFileMgr(
				AosRundata *rdata,
				const u32 virtual_id,
				const AosModuleId::E module_id);
				//const u32 siteid,

	virtual AosDocFileMgrObjPtr retrieveDocFileMgrByKey(
				AosRundata *rdata, 
				const u32   virtual_id,
				const OmnString dfm_key,
				const bool create_flag,
				const AosDfmConfig &config);
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata);    

	virtual AosDocFileMgrObjPtr createDocFileMgrByKey(
				AosRundata *rdata, 
			    const u32   virtual_id,
				const OmnString dfm_key, 
				const AosDfmConfig &config);
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata);

	virtual AosDocFileMgrObjPtr   createDocFileMgr(
				AosRundata *rdata, 
				const u32 virtual_id, 
				const u32 dfm_id, 
				const AosDfmConfig &config);
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata);
	virtual AosDocFileMgrObjPtr retrieveDocFileMgr(
				AosRundata *rdata, 
				const u32 virtual_id, 
				const u32 dfm_id, 
				const AosDfmConfig &config);
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata);

	virtual AosXmlTagPtr getConfigDoc(
					const u32 virtual_id, 
					const u64 &docid, 
					AosRundata *rdata);
	virtual bool saveConfigDoc(
					const u32 virtual_id, 
					const u64 &docid, 
					const AosXmlTagPtr &doc, 
					AosRundata *rdata);

public:
	// The following functions are used to create and open files

	virtual OmnFilePtr	createNewFile(
					AosRundata *rdata,
					const u32 virtual_id,
					u64 &file_id,
					const OmnString &fname_prefix,
					const u64 &requested_space,
					const bool reserve_flag);

	virtual OmnFilePtr	createNewTempFile(
					AosRundata *rdata,
					u64 &file_id,
					const OmnString &fname_prefix,
					const u64 &requested_space,
					const bool reserve_flag);
	
	virtual OmnLocalFilePtr	createRaidFile(
					AosRundata *rdata,
					const u32 virtual_id,
					u64 &file_id,
					const OmnString &fname_prefix,
					const u64 &requested_space,
					const bool reserve_flag);

	// Chen Ding, 2014/07/26
	// virtual OmnLocalFilePtr	openFile( const u64 &file_id,
	virtual OmnLocalFilePtr	openLocalFile( const u64 &file_id,
					AosRundata *rdata);

	//------This following func. the VirtualFileSys also has.
	virtual AosReliableFilePtr 	createRlbFile(
					AosRundata *rdata,
					const u32 virtual_id,
					//u64 &file_id,
					const OmnString &fname_prefix,
					const u64 &requested_space,
					const AosRlbFileType::E file_type,
					const bool reserve_flag);

	virtual AosReliableFilePtr openRlbFile(
					const u64 &file_id, 
					const AosRlbFileType::E file_type,
					AosRundata *rdata);
	
	virtual AosReliableFilePtr 	openRlbFile(
					AosRundata *rdata,
					const u32 virtual_id,
					const u64 file_id,
					const OmnString &fname_prefix,
					const u64 &requested_space,
					const bool reserve_flag,
					const AosRlbFileType::E file_type,
					const bool create_flag = true);
	
	virtual AosReliableFilePtr 	createRlbFileById(
					AosRundata *rdata,
					const u32 virtual_id,
					const u64 file_id,
					const OmnString &fname_prefix,
					const u64 &requested_space,
					const AosRlbFileType::E file_type,
					const bool reserve_flag);

	virtual bool removeFile(const u64 file_id, AosRundata *rdata);
	
	// -----------end. 

	virtual OmnFilePtr	createFileByStrKey(
					const u32 virtual_id,
					const OmnString &str_key,
					const u64 &requested_space,
					const bool reserve_flag,
					AosRundata *rdata
					AosMemoryCheckDecl);
	
	virtual OmnFilePtr	openFileByStrKey(
					const u32 virtual_id,
					const OmnString &str_key,	
					AosRundata *rdata 
					AosMemoryCheckDecl);
	
	virtual OmnFilePtr	openFileByKey(
					const u32 virtual_id,
					const OmnString &str_key,
					const bool create_flag,
					AosRundata *rdata
					AosMemoryCheckDecl);

	virtual OmnString 	getFileName( 
					const u64 &file_id, 
					AosRundata *rdata);

	// Ketty 2014/03/05
	virtual AosReliableFilePtr 	openRlbFileByStrKey(
				AosRundata *rdata,
				const u32 virtual_id,
				const OmnString &str_key,
				const OmnString &fname_prefix,
				const u64 &requested_space,
				const bool reserve_flag,
				const AosRlbFileType::E file_type,
				const bool create_flag = true);
	
	// Ketty 2014/03/05
	virtual AosReliableFilePtr openRlbFileByStrKey(
				AosRundata *rdata,
				const u32 virtual_id,
				const OmnString &str_key,
				const AosRlbFileType::E file_type);

	// Ketty 2014/03/05
	virtual bool removeFileByStrKey(
				AosRundata *rdata,
				const u32 virtual_id,
				const OmnString &str_key);

public:
	// This following is for Replic and manage the files.
	//virtual bool 	isBackupVirtual(const u32 virtual_id, bool & is_backup);

	virtual bool 	getFileInfo(
				const u64 &file_id,
				AosStorageFileInfo &file_info,
				bool &find, 
				AosRundata *);
	
	virtual bool	addFileByFileId(AosRundata *rdata, 
				AosStorageFileInfo &file_info,
				const int filter_deviceid);

	virtual bool	rebindFile(AosStorageFileInfo &file_info, AosRundata *rdata);

	virtual bool 	getTotalFileInfo(
				AosRundata *rdata, 
				const u32 virtual_id,
				AosBuffPtr &data);
	//virtual bool 	getTotalFileInfo(
	//			AosRundata *rdata, 
	//			const u32 dist_id, 
	//			vector<AosStorageFileInfo> &total_files); 

	//virtual bool 	checkSpace(const u64 need_space, bool &enough);
	//virtual bool	freeSpace( const int device_id, const u64 free_size);
	//virtual bool 	deleteFiles(const vector<AosStorageFileInfo> &total_files);

	virtual OmnString getBaseDir(const u32 device_id);
	
	// Ketty 2013/07/26
	// Chen Ding, 2014/07/26
	// virtual OmnFilePtr	openFile(
	virtual OmnFilePtr	openRawFile(
				const u64 &file_id,
				AosRundata *rdata)
	{
		return openFilePriv(file_id, rdata);
	}
	
	virtual bool diskCheck();

private:
	AosVirtualFileSysPtr 	getVirtualFileSys(const u32 virtual_id);

	bool	removeRaidFilePriv(
				const u64 &file_id,
				AosRundata *rdata);

	OmnFilePtr	openFilePriv(
				const u64 &file_id,
				AosRundata *rdata);
	
	// Ketty 2014/03/05
	AosReliableFilePtr createRlbFileByStrKey(
				AosRundata *rdata,
				const u32 virtual_id,
				const OmnString &str_key,
				const AosRlbFileType::E file_type,
				const u64 &requested_space,
				const bool reserve_flag);
	
};
#endif
