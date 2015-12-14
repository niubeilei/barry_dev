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
// 02/07/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_SlabBlockFile_h
#define AOS_DocFileMgr_SlabBlockFile_h

#include "DfmUtil/DfmCompareFun.h"
#include "QueryRslt/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "StorageMgrUtil/SignatureAlgo.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <set>


struct AosSlabBlockFile : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eBitmapSize = 500000,		//500K
		eMaxFileSize = 4000000000,	// 4G
		
		eSignAlgo = AosSignatureAlgo::eMD5,
		eSignLen  = AosSignatureAlgo::eMD5Len,

		eOffsetBlocksize = 0,
		eOffsetCrtMaxBlockId = eOffsetBlocksize + 4,
		eOffsetBitmap = eOffsetCrtMaxBlockId + 4,
		eOffsetHeaderSign = eOffsetBitmap + eBitmapSize,

		eFileHeaderSize = eOffsetHeaderSign + eSignLen,

		eOffsetBlockStart = eFileHeaderSize,
		eCheckSize = 1024,

		// each Entry info.
		eOrigLenOff = 0,
		eCompressLenOff = eOrigLenOff + 4,
		eDataOff = eCompressLenOff + 4

	};

private:
	OmnMutexPtr		mLock;
	AosReliableFilePtr mFile;
	u32				mBlockSize;
	u32				mMaxBlockNum;
	char*			mBitmap;
	u32				mCrtMaxBlockId;
	set<u32>		mSanitySet;
	bool			mSaved;
	bool			mNeedSave;

	static char		smCheck[eCheckSize];
	static char		smCheckChar[8];
	static char		smCheckChar2[8];
	static bool		smInit;
	
public:
	AosSlabBlockFile(
			const AosReliableFilePtr &file, 
			const u32 blocksize,
			const AosRundataPtr &rdata); 
	AosSlabBlockFile(
			const AosReliableFilePtr &file,
			const AosRundataPtr &rdata); 
	~AosSlabBlockFile();

	static void sInit();
	static u32 calculateBlockSize(const u32 pure_block_size)
	{
		return pure_block_size + sizeof(u32) + sizeof(u32) + eSignLen;
	}

	inline u32 getBlockSize() const {return mBlockSize;}

	static u32 getMaxDocSize(const u32 block_size)
	{
		u32 conf_size = sizeof(u32) + sizeof(u32) + eSignLen;
		if(block_size  <= conf_size)	return 0;
		
		return block_size - conf_size;
	}	

	bool 	saveDoc(
				const u32 blockId, 
				const u32 orig_len,
				const u32 compress_len,
				const char *data,
				const AosRundataPtr &rdata);
	bool 	readDoc(
				const u32 blockId, 
				AosBuffPtr &buff,
				u32 &orig_len,
				u32 &compress_len,
				const AosRundataPtr &rdata);
	
	//bool	addDoc(
	//			const u32 blockId,
	//			const u32 dataLen,
	//			const char *data,
	//			const AosRundataPtr &rdata);
	//bool	modifyDoc(
	//			const u32 blockId,
	//			const u32 dataLen,
	//			const char *data,
	//			const AosRundataPtr &rdata);
	bool	removeDoc(const u32 blockId);
	//bool	readDoc(
	//			const u32 blockId,
	//			//const u32 dataLen,
	//			//char *data,
	//			AosBuffPtr &data_buff,
	//			const AosRundataPtr &rdata);
	bool	findEmptyBlock(
				bool &find,
				u32 &blockId,
				const AosRundataPtr &rdata);
	bool	saveBitmap(const AosRundataPtr &rdata);

	bool addDocs(
			const AosRundataPtr &rdata,
			const u32 blockId,
			const char *data,
			const u32 data_len);

	//AosBuffPtr readDocs(
	//		const AosRundataPtr &rdata,
	//		const u32 blockId,
	//		u32 &expect_num);

	bool saveDocToBuff(
			const u32 index,
			const AosBuffPtr &buff,
			const u32 orig_len,
			const u32 compress_len,
			const char *data);
private:
	bool saveHeader(const AosRundataPtr &rdata);
	bool readHeader(const AosRundataPtr &rdata);
	int	 getSubBlockId(char c);
	bool appendBitmap(const u32 blockId, const AosRundataPtr &rdata);
	bool removeBitmap(const u32 blockId);
	
	bool sanityCheck(const u32 blockId);
	bool sanityAdd(const u32 blockId);

	//bool checkSign(const AosBuffPtr &buff, const u32 buff_len);
	//void setSign(const AosBuffPtr &buff, const u32 buff_len);

};
#endif
