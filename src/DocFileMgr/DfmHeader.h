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
// 01/18/2013 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DfmHeader_h
#define AOS_DocFileMgr_DfmHeader_h

#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/DfmUtil.h"
#include "DfmUtil/DfmCompareFun.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "StorageMgr/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"
#include "Util/MemoryCheckerObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/File.h"


class AosDfmHeader : virtual public OmnRCObject
{
	OmnDefineRCObject;

	typedef map<u32, AosDfmHeaderFilePtr> FileMap;
	typedef map<u32, AosDfmHeaderFilePtr>::iterator FileMapItr;
	
	enum EntrySts
	{
		eInvalidEntry = 0,
		eValidEntry = 1,
	};

public:
	enum
	{
		eMaxHeaderFileSize 		= 1000000000,		// 1G Ketty 2011/11/08
		//eMaxHeaderFileSize 		= 1000000,		// 1M Ketty for test. 
	
		// Header buff.
		//eHeaderLenOff = 0,
		eOprFlagOff = 0,                
		eHeaderLenOff = eOprFlagOff + 4,
		eHeaderInfoOff = eHeaderLenOff + 4,
		
		eHeaderMetaLen = sizeof(u32) + sizeof(u8),	// Ketty 2014/03/03
		//eMaxReadHeaderSize = 4000000
	};

private:
	OmnMutexPtr		mLock;

	u32				mVirtualId;
	u32				mDfmId;

	FileMap			mFiles;
	OmnString	    mPrefix;

	u32				mCheckSumOff;
	u32        		mDocHeaderSize;
	u32        		mDocHeaderEntrySize;
	u32             mMaxHeaderPerFile;
	
	u32				mMaxSeqno;

	AosDfmDocPtr	mTempDoc;
	bool			mShowLog;

public:
	AosDfmHeader(
		const u32 v_id,
		const u32 dfm_id,
		//const u32 snap_id,
		const OmnString &h_prefix,
		list<AosDfmHeaderFilePtr> &files,
		AosDfmDocType::E doc_type,
		const bool show_log);
	~AosDfmHeader();

public:
	// Ketty 2014/03/03
	//AosDfmDocPtr readHeader(
	//		const AosRundataPtr &rdata,
	//		const u64 docid,
	//		AosDfmHeaderStatus::E &sts);
	
	// Ketty 2014/03/03
	//bool saveHeader(
	//		const AosRundataPtr &rdata,
	//		const AosDocFileMgrObjPtr &dfm,
	//		const AosDfmDocPtr &doc,
	//		const AosDfmHeaderStatus::E sts);

	// Ketty 2014/03/03
	//bool readHeaders(
	//		const AosRundataPtr &rdata,
	//		const u64 beg_docid,
	//		const u32 total_num,
	//		vector<AosDfmDocPtr> &all_doc,
	//		vector<AosDfmHeaderStatus::E> &all_sts);

	// Ketty 2014/03/03
	//bool readHeaders(
	//		const AosRundataPtr &rdata,
	//		const u64 beg_docid,
	//		const AosBuffPtr &headers_buff);

	// Ketty 2014/03/03
	//bool saveHeaderToBuff(
	//		const AosBuffPtr &headers_buff,
	//		const u32 idx,
	//		const AosDfmDocPtr &doc,
	//		const AosDfmHeaderStatus::E sts);

	// Ketty 2014/03/03
	//bool readHeaderFromBuff(
	//		const AosBuffPtr &headers_buff,
	//		const AosDfmDocPtr &doc,
	//		const u32 idx,
	//		const bool clone);
		
	// Ketty 2014/03/03
	//bool saveHeaders(
	//		const AosRundataPtr &rdata,
	//		const AosDocFileMgrObjPtr &dfm,
	//		const u64 beg_docid,
	//		const AosBuffPtr &headers_buff);

	// Ketty 2014/03/03
	//bool removeFiles(
	//		const AosRundataPtr &rdata, 
	//		const AosDocFileMgrObjPtr &dfm);
	
	// Chen Ding, 03/24/2012
	//bool	 saveBatchFixedDocs(
	//			const AosDocFileMgrObjPtr &dfm,			
	//			const int num_docs,
	//			const u64 *docids, 
	//			const AosBuffPtr &buff, 
	//			const int record_size,
	//			const AosRundataPtr &rdata);
	
	u32 	getEntrySize(){ return mDocHeaderEntrySize; };

	bool	readHeaders(
				const AosRundataPtr &rdata,
				AosDfmCompareFunType::set_f &docs);

	bool	saveHeaders(
				const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				AosDfmCompareFunType::set_f &docs);

	// Ketty 2014/03/03
	bool 	readHeader(
				const AosRundataPtr &rdata,
				const u64 docid,
				AosDfmDocHeaderPtr &header);

	// Ketty 2014/03/03
	bool 	saveHeader(
				const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				const AosDfmDocHeaderPtr &header);
		
private:
	bool readAndCheckHeader(
			const AosRundataPtr &rdata,
			const u32 seqno,
			const u64 offset,
			const u32 need_size,
			char * data,
			u32 &read_size);

	AosReliableFilePtr openHeaderFile(
			const AosRundataPtr &rdata,
			const u32 seqno); 

	AosReliableFilePtr openHeaderFile(
			const AosRundataPtr &rdata, 
			const u32 seqno,
			const AosDocFileMgrObjPtr &dfm);
	
};
#endif


