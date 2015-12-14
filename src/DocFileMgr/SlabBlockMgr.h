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
#ifndef AOS_DocFileMgr_SlabBlockMgr_h
#define AOS_DocFileMgr_SlabBlockMgr_h

#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/DfmUtil.h"
#include "DfmUtil/DfmCompareFun.h"
#include "QueryRslt/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosSlabBlockMgr : virtual public OmnRCObject 
{
	OmnDefineRCObject;

	enum
	{
		eMaxFiles = 1000,
		eBatchBlockSize = 10000000 // 10M
	};

	typedef map<u32, AosDfmSlabFilePtr> FileMap;
	typedef map<u32, AosDfmSlabFilePtr>::iterator FileMapItr;

private:
	OmnMutexPtr			mLock;

	u32					mVirtualId;
	u32					mDfmId;
	OmnString			mSlabPrefix;

	FileMap				mFiles;
	int					mCrtMaxSeqno;
	bool				mShowLog;	

public:
	AosSlabBlockMgr(
		const u32 v_id,
		const u32 dfm_id,
		const OmnString &prefix,
		vector<AosDfmFilePtr> &files,
		const bool show_log);
	~AosSlabBlockMgr();
	
	bool	saveBitmap(
				const u32 seqno, 
				const AosRundataPtr &rdata);

	bool 	flushBitmap(const AosRundataPtr &rdata);

	bool 	addDoc(const AosDfmDocPtr doc, const AosRundataPtr &rdata);
	//bool	addDoc(
	//			const AosDocFileMgrObjPtr &dfm,
	//			u32 &seqno,
	//			u64 &offset,
	//			const u32 orig_len,
	//			const u32 compress_len,
	//			const char *data,
	//			const AosRundataPtr &rdata);
	bool	modifyDoc(
				const u32 &seqno,
				const u64 &offset,
				const u32 orig_len,
				const u32 compress_len,
				const char *data,
				bool &need_kickout,
				const AosRundataPtr &rdata);
	bool	removeDoc(
				const u32 seqno,
				const u64 offset,
				const AosRundataPtr &rdata);
	bool	readDoc(
				const u32 seqno,
				const u64 offset,
				AosBuffPtr &data_buff,
				u32 &orig_len,
				u32 &compress_len,
				const AosRundataPtr &rdata);

	bool	addDocs(
				const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				AosDfmCompareFunType::set_fs &docs);

	bool 	findBlock(
				const AosDocFileMgrObjPtr &dfm,
				u32 &seqno,
				u32 &blockId,
				const u32 docsize,
				const AosRundataPtr &rdata);
private:

	AosSlabBlockFilePtr		openSlabFile(
								const u32 seqno,
								const AosRundataPtr &rdata);
	AosSlabBlockFilePtr		createSlabFile(
								const AosDocFileMgrObjPtr &dfm,
								u32 &seqno, 
								const u32 blocksize,
								const AosRundataPtr &rdata);

	bool	addDocs(
				const AosRundataPtr &rdata,
				AosDfmCompareFunType::set_fs &docs,
				AosDfmCompareFunType::setitr_fs &itr,
				const AosSlabBlockFilePtr &slabFile,
				const u32 &seqno);

};
#endif
