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
//	All documents are stored in 'mDocFilename' + seqno. There is 
//	a Document Index, that is stored in files 'mDocFilename' + 
//		'Idx_' + seqno
//
// Modification History:
// 12/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_DocFileMgr_h
#define AOS_SEUtil_DocFileMgr_h
/*
#include "SearchEngine/Ptrs.h"
#include "Thread/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/File.h"
#include "XmlUtil/Ptrs.h"



class AosDocFileMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eDocSizeOffset = 0,
		eDiskcapOffset = 4,
		eDocOffset = 8,
		eMaxFiles = 500,
		eMaxHeaderFiles = 1000,
		eReservedSize = 1000,
		eNumRcdsOffset = 40,
		eFilesizeOffset = 10,
		eAllSpaceSize = 10000,
		eDftDiskBlocksize = 1000000,
		eFileHeaderSize = 100,
		eSanityFlags = 0x265d4ac7,
		eMaxBlockSizes = 4,

		eOffsetBlocksize 			= 10,
		eOffsetSanityFlag 			= eOffsetBlocksize + 4,
		eOffsetActiveBlockStart 	= eOffsetSanityFlag + 4,

		eFirstBlockStart			= 500000,
		//eFirstBlockStart			= eFileHeaderSize,
		eFileHeaderStart			= 0
	};

private:
	OmnMutexPtr		mLock;
	u32				mCrtLogSeqno;
	u32				mCrtSeqno;
	u32				mMaxDocFiles;
	OmnString		mDirname;
	OmnString		mFilename;

	OmnFilePtr		mDocFiles[eMaxFiles];
	OmnFilePtr		mHeaderFiles[eMaxHeaderFiles];
	char *			mBuffs[eMaxFiles];
	u32				mBlocksizes[eMaxBlockSizes+1];
	u32				mBlocksizeIdxs[eMaxBlockSizes+1];
	u32				mSmallDocMaxSize;

	int				mNumHeaders;
	int				mCrtOffset;
	bool			mServerReadonly;
	u32				mDiskBlocksize;
	AosDiskBlockPtr	mActiveBlocks[eMaxBlockSizes+1];

	AosDiskBlockMgrPtr	mBlockMgr;
	AosIdleBlockMgrPtr	mIdleBlockMgr;

public:
	AosDocFileMgr();
	AosDocFileMgr(
		const u32 maxdocfiles, 
		const OmnString &dirname, 
		const OmnString &fname);
	AosDocFileMgr(const AosXmlTagPtr &def, const OmnString &defname);
	~AosDocFileMgr();

	bool	stop();
	bool 	
	saveDoc(
		u32 &seqno,
		u64 &offset,
		const u32 &docsize,
		const char *data)
	{
		if (offset == 0) return saveNewDoc(docsize, seqno, offset, data);
		return modifyDoc(seqno, offset, docsize,data);
	}
	
	bool 	init(
				const OmnString &dirname, 
				const OmnString &fname,
				const u32 maxdocfiles); 
	bool	saveHeader(const u32 seqno,
				const u64 offset, 
				const char *data, 
				const int len, 
				const bool flushflag);

	bool	saveHeaderSegment(const u32 seqno,
				const u64 offset, 
				const int totalLen, 
				const u64 offset2, 
				const char *data, 
				const int len, 
				const bool flushflag);

	bool	readHeader(
				const u32 seqno,
				const u32 offset, 
				char *data, 
				const u32 max, 
				int &len);

	bool  	findLogFilePriv(u32, u32&);
	AosXmlTagPtr	getXmlDoc2(const u32 seqno, const u32 offset);
	AosXmlTagPtr	getXmlDoc3(const u32 seqno, const u32 offset);
	bool
	readDoc(
		const u32 seqno, 
		const u64 offset, 
		char *data, 
		const u32 docsize); 
	bool deleteDoc(
		const u32 seqno, 
		const u64 offset);

	int getIncSize1(){return 1;};
	static int getReservedSize() {return eReservedSize;}

private:
	bool 	modifyDoc(
				u32 &seqno,
				u64 &offset,
				const u32 &docsize,
				const char *data);
	bool 	saveNewDoc(
				const u32 &docsize,
				u32 &seqno, 
				u64 &offset, 
				const char *data);

	bool 	findDocFilePriv(const u64 &newsize, u32 &seqno);
	bool 	findSpace1(
					const u32 newsize, 
					u32 &seqno, 
					u64 &offset);
	bool 	allocateDocSpace1(
					const u32 expectedsize, 
					u64 &diskcap, 
					u32 &seqno, 
					u64 &offset); 

	OmnFilePtr 	openDocFilePriv(const u32 seqno, bool &isnewfile AosMemoryCheckDecl);
	OmnFilePtr 	openDocFilePriv(const u32 seqno AosMemoryCheckDecl)
				{
					bool isnewfile;
					return openDocFilePriv(seqno, isnewfile AosMemoryCheckerFileLine);
				}
	OmnFilePtr 	getDocFilePriv(const u32 seqno AosMemoryCheckDecl);
	OmnFilePtr 	openHeaderFilePriv(const u32 seqno AosMemoryCheckDecl);

	AosDiskBlockPtr
	adjustSpace(
		u32 &seqno, 
		u32 &offset, 
		const u32 &newsize) ;

	AosDiskBlockPtr 
	readDiskBlock(
		const u32 seqno, 
		const u32 block_start);

	AosDiskBlockPtr
	findNewBlock(
		u32 &seqno, 
		u32 &offset, 
		const u32 newsize);

	AosDiskBlockPtr 	
	createNewBlock(u32 &seqno);

	bool
	createHeader(
		const OmnFilePtr &ff,
		const u32 seqno, 
		const u32 blocksize, 
		const u32 blockstart);

	bool
	updateHeader(
		const u32 seqno, 
		const u32 blocksize, 
		const u32 blockstart);

	bool
	createHeader(
		const OmnFilePtr &ff,
		const u32 seqno, 
		const u32 blocksize);

	int getDocsizeIdx(const u32 docsize);
	AosDiskBlockPtr createActiveBlock(
			const u32 docsize, 
			const OmnFilePtr &ff, 
			u32 &seqno);
	AosDiskBlockPtr getActiveBlock(u32 &seqno, const u32 docsize);
	AosDiskBlockPtr findDocFilePriv(const int size_idx, u32 &seqno);
	bool 	doesFileSizeMatch(const u32 seqno, const int size_idx);
	u32 	getBlockStart(const u32 seqno);
	u32 	getBlocksize(const u32 seqno);
	u32 	getBlocksize(const OmnFilePtr file);
	bool 	openLogFilePriv(const u32 seqno);
	char * 	readFileHeader(const u32 seqno);
	char * 	readFileHeaderfi(const OmnFilePtr file);

	AosDiskBlockPtr
	getEmptyBlock(
		const AosDiskBlockPtr &block, 
		u32 &seqno,
		u32 needsize);
	AosDiskBlockPtr findActiveBlock(u32 &seqno, const int size_idx);



};
*/
#endif
