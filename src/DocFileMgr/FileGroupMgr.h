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
// 07/22/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_FileGroupMgr_h
#define AOS_DocFileMgr_FileGroupMgr_h

#include "DocFileMgr/Ptrs.h"
#include "DfmUtil/DfmConfig.h"
#include "Rundata/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "Thread/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "StorageMgr/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/MemoryChecker.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "UtilHash/Ptrs.h"
#include "AppMgr/App.h"
#include "DfmWrapper/DfmWrapper.h"

//#include <hash_map>
#include <map>
using namespace std;

class AosFileGroupMgr : virtual public OmnRCObject 
{
	OmnDefineRCObject;
	
	enum
	{
		eFileSize = 1000000,		// for mConfigFile
		eMaxDocFileMgr = 30,		// for docFileMgr cache.
		eDocFileMgrIdStart = 100,		// 1-99 is for reserved. 0 is error check
		
		eDFMInfoStart = 0,
		//eEntrySize = 160000,		// 160K 	12000 num files.
		eEntrySize = 1600000		// 1.6M 	120000 num files.
	};

private:
	OmnMutexPtr			mLock;
	u32					mVirtualId;
	AosVirtualFileSysPtr	mVfs;
	u32					mCrtDFMId;			
	OmnString			mFileName;
	AosReliableFilePtr	mConfigFile;
	AosDfmConfig        mConfig;
	
	AosDocFileMgrObjPtr mDocFileMgrHead;
	map<u32, AosDocFileMgrObjPtr>   mDocFileMgrMap; // for docFileMgr cache

	AosStrObjHashPtr        mStrHashMap;

public:
	AosFileGroupMgr(const AosVirtualFileSysPtr &vfs);
	~AosFileGroupMgr();

	bool        init();
	bool 		clean();
	bool        stop();
	bool        startStop();
	bool 		stopDfmLogCache();
	bool 		continueProcDfmLogCache();

	AosDocFileMgrObjPtr	createDfm(
				const AosRundataPtr &rdata,
				const AosDfmConfig &config);			

	AosDocFileMgrObjPtr	createDfmById(
				const AosRundataPtr &rdata,
				const u32 dfm_id,
				const AosDfmConfig &config);			
	
	AosDocFileMgrObjPtr	retrieveDfm(
				const AosRundataPtr &rdata,
				const u32 dfm_id,
				const AosDfmConfig &config);			

	AosDocFileMgrObjPtr	retrieveDfm(
				const AosRundataPtr &rdata,
				const u32 dfm_id);

	AosDocFileMgrObjPtr	retrieveDfmByKey(
				const AosRundataPtr &rdata,
				const OmnString dmf_key,
				const bool create_flag,
				const AosDfmConfig &config);			

	AosDocFileMgrObjPtr	createDfmByKey(
				const AosRundataPtr &rdata,
				const OmnString dmf_key,
				const AosDfmConfig &config);			

	bool 		removeDfm(
					const AosRundataPtr &rdata,
					const u32 dfm_id); 

	bool 		saveDfmBuffToFile(
					const AosRundataPtr &rdata,
					const u32 dfm_id,
					const AosBuffPtr &dfm_buff);
	
	bool 		removeDfmBuffFromFile(
					const AosRundataPtr &rdata,
					const u32 dfm_id,
					AosBuffPtr &dfm_buff);
	
	bool		removeFile(
					const AosRundataPtr &rdata,
					const u32 dfm_id,
					AosBuffPtr &dfm_buff,
					const u64 file_id);

private:
	bool    sanityCheck();

	bool	insertToListPriv(const AosDocFileMgrObjPtr &docFileMgr);
	AosDocFileMgrObjPtr getFromListPriv(const u32 dfm_id);

	bool 	readDfmBuffFromFile(
				const AosRundataPtr &rdata,
				const u32 dfm_id,
				AosBuffPtr &dfm_buff);

	bool	checkStrKeyExist(
				const OmnString &dfm_key, 
				bool &exist, 
				u32 &dfm_id,
				const AosRundataPtr &rdata);

};

#endif
