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
#ifndef AOS_SEInterfaces_VfsMgrObj_h
#define AOS_SEInterfaces_VfsMgrObj_h

#include "Rundata/Ptrs.h"
#include "DfmUtil/DfmConfig.h"
#include "ReliableFile/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/RlbFileType.h"
#include "StorageMgr/Ptrs.h"
#include "StorageMgrUtil/FileInfo.h"
#include "UtilData/ModuleId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/MemoryChecker.h"
#include "XmlUtil/Ptrs.h"

#include <vector>

class AosVfsMgrObj : virtual public OmnRCObject
{
protected:
	static AosVfsMgrObjPtr		smVfsMgr;	

	static vector<int>			smDeviceId;

public:
	virtual bool config(const AosXmlTagPtr &def) = 0;
	virtual bool clean() = 0;
	virtual bool start() = 0;
	virtual bool stop() = 0;
	virtual bool switchToBkp() = 0;
	virtual bool reStart() = 0;

	static void configStatic(const AosXmlTagPtr &app_conf);

	static bool getDeviceId(vector<int> &devid);

	virtual AosDocFileMgrObjPtr retrieveDocFileMgr(
				AosRundata *rdata, 
				const u32	virtual_id,
				const AosDfmConfig &config) = 0;
				//const AosModuleId::E module_id,
				////const u32 siteid,
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata) = 0; 

	virtual AosDocFileMgrObjPtr createDocFileMgr(
				AosRundata *rdata, 
				const u32	virtual_id,
				const AosDfmConfig &config) = 0;
			    //const AosModuleId::E module_id,
				////const u32 siteid,
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata) = 0;

	virtual bool 	removeDocFileMgr(
				AosRundata *rdata,
				const u32 virtual_id,
				const AosModuleId::E module_id) = 0;
				//const u32 siteid,

	virtual AosDocFileMgrObjPtr retrieveDocFileMgrByKey(
				AosRundata *rdata, 
				const u32   virtual_id,
				const OmnString dfm_key,
				const bool create_flag,
				const AosDfmConfig &config) = 0;
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata) = 0;

	virtual AosDocFileMgrObjPtr createDocFileMgrByKey(
				AosRundata *rdata, 
			    const u32   virtual_id,
				const OmnString dfm_key, 
				const AosDfmConfig &config) = 0;
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata) = 0;

	virtual AosDocFileMgrObjPtr   createDocFileMgr(
				AosRundata *rdata, 
				const u32 virtual_id, 
				const u32 dfm_id, 
				const AosDfmConfig &config) = 0;
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata) = 0;
	virtual AosDocFileMgrObjPtr retrieveDocFileMgr(
				AosRundata *rdata, 
				const u32 virtual_id, 
				const u32 dfm_id, 
				const AosDfmConfig &config) = 0;
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata) = 0;

	virtual AosXmlTagPtr getConfigDoc(
					const u32 virtual_id, 
					const u64 &docid, 
					AosRundata *rdata) = 0;
	virtual bool		 saveConfigDoc(
					const u32 virtual_id, 
					const u64 &docid, 
					const AosXmlTagPtr &doc, 
					AosRundata *rdata) = 0;

public:
	// The following functions are used to create and open files

	virtual OmnFilePtr	createNewFile(
					AosRundata *rdata,
					const u32 virtual_id,
					u64 &file_id,
					const OmnString &fname_prefix,
					const u64 &requested_space,
					const bool reserve_flag) = 0;
	
	// Chen Ding, 2014/07/26
	// virtual OmnLocalFilePtr	openFile( const u64 &file_id,
	virtual OmnLocalFilePtr	openLocalFile(
					const u64 &file_id,
					AosRundata *rdata) = 0;

	//------This following func. the VirtualFileSys also has.
	virtual AosReliableFilePtr 	createRlbFile(
					AosRundata *rdata,
					const u32 virtual_id,
					//u64 &file_id,
					const OmnString &fname_prefix,
					const u64 &requested_space,
					const AosRlbFileType::E file_type,
					const bool reserve_flag) = 0;

	virtual AosReliableFilePtr openRlbFile(
					const u64 &file_id, 
					const AosRlbFileType::E file_type,
					AosRundata *rdata) = 0;
	
	virtual AosReliableFilePtr 	openRlbFile(
					AosRundata *rdata,
					const u32 virtual_id,
					const u64 file_id,
					const OmnString &fname_prefix,
					const u64 &requested_space,
					const bool reserve_flag,
					const AosRlbFileType::E file_type,
					const bool create_flag = true) = 0;
	
	virtual AosReliableFilePtr 	createRlbFileById(
					AosRundata *rdata,
					const u32 virtual_id,
					const u64 file_id,
					const OmnString &fname_prefix,
					const u64 &requested_space,
					const AosRlbFileType::E file_type,
					const bool reserve_flag) = 0;

	virtual bool		removeFile(const u64 file_id, AosRundata *rdata) = 0;
	// -----------end. 

	virtual OmnFilePtr	createFileByStrKey(
					const u32 virtual_id,
					const OmnString &str_key,
					const u64 &requested_space,
					const bool reserve_flag,
					AosRundata *rdata
					AosMemoryCheckDecl) = 0;
	
	virtual OmnFilePtr	openFileByStrKey(
					const u32 virtual_id,
					const OmnString &str_key,	
					AosRundata *rdata 
					AosMemoryCheckDecl) = 0;
	
	virtual OmnFilePtr	openFileByKey(
					const u32 virtual_id,
					const OmnString &str_key,
					const bool create_flag,
					AosRundata *rdata
					AosMemoryCheckDecl) = 0;

	virtual OmnString 	getFileName( 
					const u64 &file_id, 
					AosRundata *rdata) = 0;

	virtual OmnFilePtr 	createNewTempFile(
					AosRundata *rdata,
					u64 &file_id,
					const OmnString &fname_prefix,
					const u64 &requested_space,
					const bool reserve_flag) = 0;

	virtual OmnLocalFilePtr 	createRaidFile(
					AosRundata *rdata,
					const u32 virtual_id,
					u64 &file_id,
					const OmnString &fname_prefix,
					const u64 &requested_space,
					const bool reserve_flag) = 0;
	
	// Ketty 2014/03/05
	virtual AosReliableFilePtr 	openRlbFileByStrKey(
				AosRundata *rdata,
				const u32 virtual_id,
				const OmnString &str_key,
				const OmnString &fname_prefix,
				const u64 &requested_space,
				const bool reserve_flag,
				const AosRlbFileType::E file_type,
				const bool create_flag = true) = 0;
	
	// Ketty 2014/03/05
	virtual AosReliableFilePtr openRlbFileByStrKey(
				AosRundata *rdata,
				const u32 virtual_id,
				const OmnString &str_key,
				const AosRlbFileType::E file_type) = 0;
	
	// Ketty 2014/03/05
	virtual bool removeFileByStrKey(
				AosRundata *rdata,
				const u32 virtual_id,
				const OmnString &str_key) = 0;

public:
	// This following is for Replic and manage the files.
	//virtual bool 	isBackupVirtual(const u32 virtual_id, bool & is_backup) = 0;

	virtual bool 	getFileInfo(
				const u64 &file_id,
				AosStorageFileInfo &file_info,
				bool &find, 
				AosRundata *) = 0;
	
	virtual bool	addFileByFileId(AosRundata *rdata, 
				AosStorageFileInfo &file_info,
				const int filter_deviceid) = 0;

	virtual bool	rebindFile(AosStorageFileInfo &file_info, AosRundata *rdata) = 0;

	virtual bool 	getTotalFileInfo(
				AosRundata *rdata, 
				const u32 virtual_id,
				AosBuffPtr &data_buff) = 0;
	//virtual bool 	getTotalFileInfo(
	//			AosRundata *rdata, 
	//			const u32 dist_id, 
	//			vector<AosStorageFileInfo> &total_files) = 0; 

	//virtual bool 	checkSpace(const u64 need_space, bool &enough) = 0;
	//virtual bool	freeSpace( const int device_id, const u64 free_size) = 0;
	//virtual bool 	deleteFiles(const vector<AosStorageFileInfo> &total_files) = 0;

	virtual OmnString getBaseDir(const u32 device_id) = 0;

	// Ketty 2013/07/26
	// Chen Ding, 2014/07/26
	// virtual OmnFilePtr	openFile(
	virtual OmnFilePtr	openRawFile(
				const u64 &file_id,
				AosRundata *rdata) = 0;
	
	virtual bool diskCheck() = 0;

public:
	//virtual bool	svrIsDeath(const u32 svr_id) = 0;
	//virtual bool 	svrIsUp(AosRundata *rdata, const u32 svr_id) = 0;

	static void setVfsMgr(const AosVfsMgrObjPtr &mgr){ smVfsMgr = mgr; };
	static AosVfsMgrObjPtr getVfsMgr(){ return smVfsMgr; };

};
#endif
