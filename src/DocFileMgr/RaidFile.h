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
//
// Modification History:
// 05/013/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_RaidFile_h
#define AOS_DocFileMgr_RaidFile_h

#include "DocFileMgr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "StorageMgrUtil/FileInfo.h"
#include "Thread/Sem.h"
#include "Thread/RwLock.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/LocalFile.h"
#include "Util/String.h"
#include "Util/Ptrs.h"

#include <vector>

struct AosRaidFile : public OmnLocalFile
{
	OmnDefineRCObject;

public:
	enum
	{
		eCheckSum = 78127582349847814ULL,
		eBlockSize = 1024 * 1024,   // 1M
		eEntrySize = sizeof(u64)+ sizeof(u32) + sizeof(u32) + sizeof(u64),
		eIndexFileSize = eEntrySize * 20
 	};

private:
	OmnFilePtr 					mFile;
	vector<AosStorageFileInfo>	mFileInfos;
	u32							mDeviceNum;	
	vector<OmnFilePtr>			mFiles;
	OmnRwLockPtr				mRwLock;
	i64							mSeekPos;
	bool						mIsGood;

public:
	AosRaidFile();
	AosRaidFile(
			const AosRundataPtr &rdata,
			const OmnFilePtr &file,
			const vector<AosStorageFileInfo> &fileinfos);
	AosRaidFile(
			const AosRundataPtr &rdata,
			const OmnFilePtr &file);
	AosRaidFile(
			const AosRundataPtr &rdata,
			const AosBuffPtr &buff);
	~AosRaidFile();

	bool	closeFile();

	bool	write(
			const i64 &startPos,
			const char *buff,       
			const i64 &len,
			const bool flush = false); 

	i64		read(
			const i64 &startPos, 
			const char *buff,
			const i64 &len); 
	
	void 	unlock(){mRwLock->unlock();}

	void 	readlock(){mRwLock->readlock();}

	void 	writelock()	{mRwLock->writelock();}

	void  	deleteFile(AosRundata *rdata);

	bool	append(
			const char *buff,
			const i64 &len,
			const bool flush = false);

	OmnString 	getFileName() const;

	i64	 getDeviceId(){return 1;};

	bool	isGood() const;

	void    resetFile();

	u64 	getLength();

	bool	serializeTo(const AosBuffPtr &buff);

private:

	void 	calculateOffSize(
			const u64 &crt_block_id, 
			const i64 &global_off,  
			const i64 &size,  
			i64 &seek_pos,      
			i64 &bytes_to_read);

	OmnFilePtr 	getFile(const u64 &block_id);

	bool	addReq(
			const AosRaidFileReqPtr &request,
			const i64 &start_pos,
			const i64 &size);

	void	setFileInfoToBuff(
			const AosBuffPtr &buff, 
			const AosStorageFileInfo & file_info);

	void	getFileInfoFromBuff(
			AosStorageFileInfo &file_info,
			const AosBuffPtr &buff);

	bool	sanityCheck(
			const i64 &global_off,
			const u64 block_id, 
			const i64 &seekPos,
			const i64 &bytes_to_read);

};
#endif
