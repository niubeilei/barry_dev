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
// 07/28/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgr_VirtualFileSys_h
#define AOS_StorageMgr_VirtualFileSys_h

#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/StorageApp.h"
#include "DfmUtil/DfmConfig.h"
#include "Rundata/Ptrs.h"
#include "StorageMgr/Ptrs.h"
#include "StorageMgr/StorageMgr.h"
#include "StorageMgr/FileId.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/RlbFileType.h"
#include "XmlUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "UtilData/ModuleId.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "UtilHash/Ptrs.h"

#include <map>
#include <list>
using namespace std;

class AosVirtualFileSys : virtual public OmnRCObject
{
	OmnDefineRCObject;
	
public:
	enum
	{
		// This is for mAllocFile
		eMaxNameLen = 300,
		eEntryStart = 0,

		//// fid. device_id. site_id. request_space. backup_policy. fname. checksum.
		eCheckSumSize = sizeof(u8),
		eEntrySize = 8 + 4 + 4 + 8 + 4 + eMaxNameLen + eCheckSumSize,
	};

	enum
	{
		eFileSeqnoStart = AOSFILEID_MAX + 1,
	
		eStrKeyFileSize = 1000000,
		eAllocFileSize = 1000000,
	};

private:
	OmnMutexPtr				mStartLock;
	OmnMutexPtr				mLock;
	AosRundataPtr 			mRdata;
	u32						mVirtualId;
	AosStorageMgrPtr		mStorageMgr;

	AosConfigMgrPtr         mConfigMgr;
	AosFileGroupMgrPtr		mFileGroupMgr;
	AosStorageAppPtr		mStorageApp;

	OmnFilePtr				mAllocFile;
	u32						mFileSeqno;

	AosStrObjHashPtr        mStrHashMap;
	int						mCubeGrpId;

	bool					mStart;

public:
	AosVirtualFileSys(const u32 virtual_id, 
			const AosStorageMgrPtr &stm);
	~AosVirtualFileSys();

	bool 	clean();
	bool	start();
	bool	stop();
	bool	isStart();
	bool	startStop();
	bool 	switchToBkp();
	bool 	reStart();
	
	OmnString	getVirtualInfo(AosRundata *);

	//bool	isBackupVFS(){ return mIsBkp; };
	bool 	isLocalBackupVFS();
	
	u32		getVirtualId(){ return mVirtualId; };

public:
	AosDocFileMgrObjPtr retrieveDocFileMgr(
				AosRundata *rdata,
				const AosDfmConfig &config);
				//const AosModuleId::E module_id,
				////const u32 siteid, 
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata);

	AosDocFileMgrObjPtr createDocFileMgr(
				AosRundata *rdata,
				const AosDfmConfig &config);
				//const AosModuleId::E module_id,
				////const u32 siteid, 
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata);
	
	AosDocFileMgrObjPtr retrieveDocFileMgrByKey(
				AosRundata *rdata,
				const OmnString dfm_key,
				const bool create_flag,
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata);
				const AosDfmConfig &config);

	AosDocFileMgrObjPtr createDocFileMgrByKey(
				AosRundata *rdata,
				const OmnString dfm_key,
				const AosDfmConfig &config);
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata);

	AosDocFileMgrObjPtr createDocFileMgrById(
				AosRundata *rdata,
				const u32 dfm_id,
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata);
				const AosDfmConfig &config);

	AosDocFileMgrObjPtr retrieveDocFileMgrById(
				AosRundata *rdata,
				const u32 dfm_id, 
				//const AosDfmProcPtr &proc,
				//AosRundata *rdata);
				const AosDfmConfig &config);

	bool 	removeDocFileMgr(
				AosRundata *rdata,
				const AosModuleId::E module_id);
				//const u32 siteid, 
	
	bool    saveConfigDoc(const u64 &docid, 
				const AosXmlTagPtr &doc, 
				AosRundata *rdata);

	AosXmlTagPtr getConfigDoc(const u64 &docid, AosRundata *rdata);

public:
	AosReliableFilePtr createRlbFile(
			AosRundata *rdata,
			//u64 &file_id,
			const OmnString &fname_prefix,
			const u64 &requested_space,
			const AosRlbFileType::E file_type,
			const bool reserve_flag);

	AosReliableFilePtr openRlbFile(
			const u64 &file_id,
			const AosRlbFileType::E file_type,
			AosRundata *rdata);
	
	AosReliableFilePtr openRlbFile(
			AosRundata *rdata,
			const u64 file_id,
			const OmnString &fname_prefix,
			const u64 &requested_space,
			const bool reserve_flag,
			const AosRlbFileType::E file_type,
			const bool create_flag = true);

	AosReliableFilePtr createRlbFileById(
			AosRundata *rdata,
			const u64 file_id,
			const OmnString &fname_prefix,
			const u64 &requested_space,
			const AosRlbFileType::E file_type,
			const bool reserve_flag);


public:
	// This two func will be called by ReplicMgr
	bool	getFileInfo(AosRundata *rdata, 
				const u64 file_id, 
				bool & find,  
				AosStorageFileInfo & file_info);

	bool	addNewFileById(AosRundata *rdata, 
						AosStorageFileInfo &file_info, 
						const int filter_deviceid = -1);
	// end.

	bool	addNewFile(AosRundata *rdata,
						const OmnString &name_prefix,
						const u64 request_space,
						const bool reserve_flag,
						AosStorageFileInfo &file_info,
						int &device_id);

	bool	addNewFile(AosRundata *rdata,
						const OmnString &name_prefix,
						const u64 request_space,
						const bool reserve_flag,
						AosStorageFileInfo &file_info);

	bool	addNewFileByStrKey(
				AosRundata *rdata,
				const OmnString &str_key,
				const u64 request_space,
				const bool reserve_flag,
				AosStorageFileInfo &file_info);


	bool	getFileInfoByStrKey(
				AosRundata *rdata,
				const OmnString &str_key,
				AosStorageFileInfo &file_info,
				bool &exist);

	// Ketty 2014/03/05
	bool 	removeFileByStrKey(
				AosRundata *rdata,
				const OmnString &str_key,
				u64 &file_id,
				bool &exist);

	bool 	getTotalFileInfo(
				AosRundata *rdata,
				AosBuffPtr &data_buff);

	bool	removeFile(AosRundata *rdata, const u64 file_id);

	OmnString	getFileName(AosRundata *rdata, const u64 file_id);

	bool	removeAllTempFile(AosRundata *rdata);
	bool	removeTempFile(AosRundata *rdata, const u32 fseqno);

	bool 	recover();

private:
	//bool 	addStrKeyFile(OmnString &full_fname);
	OmnString 	getAllocFileName();
	
	bool	saveToAllocFile(AosRundata *rdata, const AosStorageFileInfo &file_info);
	bool	getFromAllocFile(
				AosRundata *rdata,
				const u64 file_id, 
				bool& find, 
				AosStorageFileInfo &file_info);
	
	void	setFileInfoToBuff(const AosBuffPtr &buff, const AosStorageFileInfo & file_info);
	bool	getFileInfoFromBuff(
				AosStorageFileInfo &file_info,
				const AosBuffPtr &buff);

	bool	checkStrKeyExist(
				AosRundata *rdata,	
				const OmnString &str_key, 
				bool &exist, u64 &file_id);

	bool 	initAllocFile();
	bool	resetAllocFile();

	// for recover.
	bool 	recoverAllocFile(
				set<AosStorageFileInfo> &damage_files,
				int &svr_id);
	bool 	getDamagedFiles(set<AosStorageFileInfo> &damage_files);
	bool 	recoverDamagedFiles(
				set<AosStorageFileInfo> &damage_files,
				int &svr_id);
	bool 	recoverDamagedFile(
				AosStorageFileInfo &file_info,
				int &svr_id, 
				bool &svr_death);
	OmnFilePtr createRecoverFile(AosStorageFileInfo &file_info);
	int 	getNextRecoverSvr(const int crt_sid);
	bool 	getAllocFileData(AosBuffPtr &file_buff);
	
};
#endif

