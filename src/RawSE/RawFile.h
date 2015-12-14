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
//	This class is used to manage raw files whose content are not the concern
//
// Modification History:
// 2014-11-21 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RawSE_RawFile_H_
#define Aos_RawSE_RawFile_H_

#include "aosUtil/Types.h"
#include "RawSE/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include <ext/hash_map>


class AosRawFile : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	struct Identifier
	{
		u64	rawFileID;
		u32	siteID;
		u32	cubeID;
		u64	aseID;

		Identifier(
				const u32 site_id,
				const u32 cube_id,
				const u64 ase_id,
				const u64 raw_file_id)
		:
		rawFileID(raw_file_id),
		siteID(site_id),
		cubeID(cube_id),
		aseID(ase_id)
		{
		};

		~Identifier(){};

		inline bool operator==(const Identifier& rhs) const
		{
			return this->rawFileID == rhs.rawFileID && this->siteID == rhs.siteID
					&& this->cubeID == rhs.cubeID && this->aseID == rhs.aseID;
		};
	};

	enum Type	//TODO:these types need to be explicitly defined, which needs more design work
	{
		eHeader				= 0,		//for PartitionMgr use only
		eReadOnly			= OmnFile::eReadOnly,
		eAppendOnly			= OmnFile::eAppend,
		eModifyOnly 		= OmnFile::eWriteCreate,
		eReadWrite			= OmnFile::eReadWrite,
		eReadWriteCreate	= OmnFile::eReadWriteCreate,
		eSlab				= 99,
	};

public:
	AosRawFilePtr			mPrev;		//for aging, should only be used by RawSE
	AosRawFilePtr			mNext;		//for aging, should only be used by RawSE

private:
	u64						mRawFileID;
	u32						mSiteID;
	u32						mCubeID;
	u64						mAseID;
	u64						mLocalID;
	u64						mGuard;			//for checking whether the object is deleted
	OmnMutex*				mLockRaw;
	OmnMutexPtr				mLock;
	OmnFile*				mFile;
	OmnString				mFileNameWithFullPath;
	AosPartitionIDMgr		*mPartitionIDMgr;
	Type					meType;

public:
	AosRawFile(
			AosRundata				*rdata,
			AosPartitionIDMgr		*partition_id_mgr,
			const u32				site_id,
			const u32				cube_id,
			const u64				ase_id,
			const u64				raw_file_id,
			const u16				level_1_size,
			const u16				level_2_size,
			const u16				level_3_size,
			const Type				eType);
	virtual ~AosRawFile();

	bool isGood()
	{
		if (mFile)
			return mFile->isGood();
		else
			return false;
	} const

	u64	getRawFileID() const {return mRawFileID;}

	u32	getSiteID() const {return mSiteID;}

	u32	getCubeID() const {return mCubeID;}

	u64	getAseID() const {return mAseID;}

	Type getType() const {return meType;}

	bool lock() const {return mLockRaw->lock();}

	bool unlock() const {return mLockRaw->unlock();}

	void setType(const Type type) {meType = type;}

	OmnString getFileNameWithFullPath() const {return mFileNameWithFullPath;}

	u64 getLength() const
	{
		if (mFile)
		{
			if (mFile->isGood())
			{
				return mFile->getFileCrtSize();
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}

	int create(
			AosRundata	*rdata);

	int append(
			AosRundata 			*rdata,
			const char* const	data,
			const u64 			length);

	int truncate(
			AosRundata 	*rdata,
			const u64 	length);

	int read(
			AosRundata	*rdata,
			const u64	offset,
			const u64	length,
			AosBuffPtr	&buff,
			const bool	bShowAlarm = true);

	int del(AosRundata	*rdata);

	int overwrite(
			AosRundata 			*rdata,
			const char* const	data,
			const u64 			length);

	int close();

	u32 getLastModifyTime();

	int open(
			AosRundata			*rdata,
			AosRawFile::Type	open_mode,
			const bool			bShowAlarm = true);

	void dumpInfo();

private:
	int open(
			AosRundata		*rdata,
			OmnFile::Mode	open_mode,
			const bool		bShowAlarm = true);

	int composeFileNameWithFullPath(
			AosRundata	*rdata,
			const u16	level_1_size,
			const u16	level_2_size,
			const u16	level_3_size,
			const Type	eType);
};
#endif /* RAWFILE_H_ */
