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
#ifndef AOS_DocFileMgr_DfmBody_h
#define AOS_DocFileMgr_DfmBody_h

#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/DfmUtil.h"
#include "DfmUtil/DfmCompareFun.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "Thread/Ptrs.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"
#include "Util/MemoryCheckerObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"

#include <set>


class AosDfmBody : virtual public OmnRCObject
{
	OmnDefineRCObject;

	typedef map<u32, AosBodyFilePtr> FileMap;
	typedef map<u32, AosBodyFilePtr>::iterator FileMapItr;

private:
	OmnMutexPtr		mLock;
	u32				mVirtualId;
	u32				mDfmId;

	static OmnString		smCompressAlg;  //Linda
	bool			mShowLog;

	//AosDiskBlockMgrPtr  mDiskBlockMgr;
	AosSlabBlockMgrPtr  mSlabBlockMgr;
	AosReadOnlyBlockPtr mReadOnlyBlock;

public:
	AosDfmBody(
		const u32 v_id,
		const u32 dfm_id,
		const OmnString &prefix,
		const OmnString &compress_alg,
		vector<AosDfmFilePtr> &files,
		const bool show_log);
	AosDfmBody(
		const u32 v_id,
		const u32 dfm_id,
		const OmnString &prefix,
		const OmnString &compress_alg,
		const bool show_log);
	~AosDfmBody();

	bool 	stop(const AosRundataPtr &rdata);
	
	bool 	flushBitmap(const AosRundataPtr &rdata);
	
	bool	readBody(
				const AosRundataPtr &rdata, 
				const AosDfmDocPtr &doc);
	
	bool 	addBody(
				const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				const AosDfmDocPtr &doc);

	bool 	modifyBody(
				const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				bool &kickout,
				const AosDfmDocPtr &doc);
	
	bool 	removeBody(
				const AosRundataPtr &rdata,
				const AosDfmDocPtr &doc);

	bool 	removeBody(
				const AosRundataPtr &rdata,
				const AosDfmDocHeaderPtr &header);

	bool	procBody(
				const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				AosDfmCompareFunType::set_fs &docs);

	bool	modifyBodys(
				const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				AosDfmCompareFunType::set_fs &docs,
				AosDfmCompareFunType::set_fs &new_docs,
				AosDfmCompareFunType::set_fs &del_docs);
	
	bool 	addBodys( 
				const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				AosDfmCompareFunType::set_fs &docs);
	
	bool 	removeBodys(
				const AosRundataPtr &rdata,
				AosDfmCompareFunType::set_fs &docs);

	static bool 	compressBody(
				AosBuffPtr &body_buff,
				const AosRundataPtr &rdata AosMemoryCheckDecl);

	static bool 	uncompressBody(
				AosBuffPtr &body_buff,
				const u32 orig_len,
				const u32 compress_len,
				const AosRundataPtr &rdata AosMemoryCheckDecl);
	
	//bool 	isInSlabFile(const u32 fseqno);
	//bool 	saveSlabFileBitmap(const u32 fseqno, const AosRundataPtr &rdata);

private:
	bool	findBlock(
				const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				const AosDfmDocPtr &doc);

	void	print(const OmnString &name, 
				AosDfmCompareFunType::set_fs &docs);
};
#endif


