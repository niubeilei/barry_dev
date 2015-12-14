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
// 03/03/2012	Moved to VirtualFileSys.h
// 07/30/2012 Moved from VirtualFileSys.cpp	by Ketty.
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_StorageApp_h
#define AOS_DocFileMgr_StorageApp_h

#include "DocFileMgr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "StorageMgr/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "UtilData/ModuleId.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"

#include <map>
#include <list>
using namespace std;

class AosStorageApp : virtual public OmnRCObject
{
	OmnDefineRCObject;
	
public:
	enum
	{
		eMaxRcdEachBlock = 5000,
		eBlockRcdStart = 4,         // the first 4 bytes if for the crt_rcd_num.
		eRcdSize = 8,               // first 4 byte is for siteId, next 4 is for dfmId
		eBlockSize = eBlockRcdStart + eRcdSize * eMaxRcdEachBlock,

		eFileSize = eBlockSize * AosModuleId::eMax   // for mAppFile.
	};

private:
	OmnMutexPtr				mLock;
	//u32						mVirtualId;
	AosVirtualFileSysPtr	mVfs;
	AosReliableFilePtr		mAppFile;

public:
	AosStorageApp(const AosVirtualFileSysPtr &vfs);
	~AosStorageApp();
	
	bool	init();
	void	stop();

	bool 	getDfmId(
				const AosRundataPtr &rdata,
				const AosModuleId::E module_id,
				u32 &dfm_id);

	bool 	addDfmId(
				const AosRundataPtr &rdata,
				const AosModuleId::E module_id,
				const u32 dfm_id);

	bool 	removeDfmId(
				const AosRundataPtr &rdata,
				const AosModuleId::E module_id,
				u32 &dfm_id);

private:
	bool 	getModuleBuff(
				const AosRundataPtr &rdata,
				const AosModuleId::E module_id,
				AosBuffPtr &buff);

	bool	saveModuleBuff(
				const AosRundataPtr &rdata,
				const AosModuleId::E module_id,
				const AosBuffPtr &buff);

};
#endif
