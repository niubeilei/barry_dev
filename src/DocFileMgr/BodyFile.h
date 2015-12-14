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
// 01/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_BodyFile_h
#define AOS_DocFileMgr_BodyFile_h

#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


struct AosBodyFile : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eSanityFlags = 0x265d4ac7,

		eFileHeaderSize = 100,
		eOffsetBlocksize = 10,
		eOffsetSanityFlag       = eOffsetBlocksize + 4,
		eOffsetActiveBlockStart = eOffsetSanityFlag + 4,
	};

	AosReliableFilePtr mFile;
	int			mBlockSize;
	u32			mLastBlock;

	AosBodyFile(
			//const OmnFilePtr &file, 
			const AosReliableFilePtr &file, 
			const u32 blocksize, 
			const u32 lastblock, 
			const AosRundataPtr &rdata);
	//AosBodyFile(const OmnFilePtr &file, const AosRundataPtr &rdata);
	AosBodyFile(const AosReliableFilePtr &file, const AosRundataPtr &rdata);
	~AosBodyFile();

	inline u32 getBlockSize() const {return mBlockSize;}
	inline u32 getLastBlock() const {return mLastBlock;}
	u32 appendBlock(const AosRundataPtr &rdata);
	bool saveHeader(const AosRundataPtr &rdata);
	bool readHeader(const AosRundataPtr &rdata);
	AosReliableFilePtr getFile() const; 

	// Ketty 2014/02/21
	bool closeFile();
	
};
#endif
