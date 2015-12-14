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
// 08/16/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgr_DiskUtil_h
#define AOS_StorageMgr_DiskUtil_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "StorageMgr/Ptrs.h"
#include "StorageMgrUtil/FileInfo.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include <vector>
#include <map>
#include <set>
using namespace std;

class AosStorageDirInfo : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eMaxTry = 100,
		//eMaxFileNumPerDir	= 20,
		eMaxFileNumPerDir = 10000
	};
	
private:
	OmnMutexPtr	mLock;
	OmnString	mBaseDirname;				// for generate the full_filename.
	OmnString	mDirname;
	OmnString	mDataDirname;
	u32			mCrtDataDirSeqno;
	u32			mCrtDataFileNum;
	bool		mInited;

public:
	AosStorageDirInfo(const OmnString &base_dir, const OmnString &dirname);
	OmnString	getNextDataDirname();
	bool		getAllFnames(vector<OmnString> &fnames);
	void		removeDataDir();
	OmnString	getDirname(){ return mDirname; };

private:
	bool		init();

};


class AosVirtualDirInfo : virtual public OmnRCObject
{
	OmnDefineRCObject;
private:
	OmnMutexPtr						mLock;
	u32								mVirtualId;
	OmnString						mBaseDirname;
	AosStorageDirInfoPtr			mDirInfo;
	map<u32, AosStorageDirInfoPtr>	mSiteDirMap;

public:
	AosVirtualDirInfo(const u32 virtual_id, const OmnString &base_dir, const OmnString &dirname);
	OmnString getNextSubDirname(const u32 site_id);
	OmnString getDirname(){ return mDirInfo->getDirname(); };

	// for disk recover
	bool 	getTotalFiles(set<AosStorageFileInfo> &files, const u32 disk_id);

private:
	void	init();
	u64 	getFileIdByFname(const OmnString &fname);

};

#endif

