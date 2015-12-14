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
// 12/15/2009	Created by Chen Ding. Modify by Ketty 2012/09/12
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DocFileMgr_h
#define AOS_DocFileMgr_DocFileMgr_h

#include "DocFileMgr/Ptrs.h"
#include "DfmUtil/DfmConfig.h"
#include "DfmSnapshot/DfmSnapshotMgr.h"
#include "DocFileMgr/DfmUtil.h"
#include "DocFileMgr/SlabBlockFile.h"
#include "FmtMgr/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "DocFileMgr/DiskBlock.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "StorageMgr/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"
#include "Util/MemoryCheckerObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/LRUCache.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/File.h"
#include <map>
#include <vector>
using namespace std;


class AosDocFileMgr : public AosDocFileMgrObj,
					public AosMemoryCheckerObj
{
	OmnDefineRCObject;

	struct LockGen {
		OmnMutexPtr getData(const u64 docid)
		{
			return OmnNew OmnMutex();
		}

		OmnMutexPtr getData(const u64 docid, const OmnMutexPtr &lock)
		{
			return lock;
		}
	};
	
	AosLRUCache<u64, OmnMutexPtr> mLockPool;
	static LockGen smLockGen;

public:
	enum
	{
		// mDfmBuff.
		eDfmIdOff = 0,
		eDfmVidOff = eDfmIdOff + 4,
		eDfmSiteidOff = eDfmVidOff + 4,
	};

private:
	OmnMutexPtr		mLock;

	AosFileGroupMgrPtr	mFgm;
	u32				mVirtualId;
	u32				mId;		// Ketty 2011/07/25
	u32				mSiteid;	// Ketty 2013/01/29
	
	AosDfmConfig    mConfig;
	
	AosDfmHeaderPtr	mHeader;
	AosDfmBodyPtr	mBody;
	list<AosDfmHeaderFilePtr>	mHeaderFiles;
	vector<AosDfmFilePtr>	mBodyFiles;
	AosDfmLogPtr	mDfmLog;
	
	AosDfmSnapshotMgrPtr mSnapshotMgr;

public:
	bool			mShowLog;

public:
	AosDocFileMgr(
		const AosRundataPtr &rdata,
		const AosFileGroupMgrPtr &fgm,
		const u32 virtual_id,
		const u32 dfm_id,
		const AosDfmConfig &config AosMemoryCheckDecl);
	AosDocFileMgr(
		const AosRundataPtr &rdata,
		const AosFileGroupMgrPtr &fgm AosMemoryCheckDecl);
	~AosDocFileMgr();
	
	virtual bool serializeFrom(const AosBuffPtr &dfm_buff);
	virtual bool serializeTo(const AosBuffPtr &dfm_buff);
	virtual bool init();

	static AosDocFileMgrObjPtr createDfmStatic(
			const AosRundataPtr &rdata,
			const AosFileGroupMgrPtr &fgm,
			const u32 virtual_id,
			const u32 dfm_id,
			const AosDfmConfig &config AosMemoryCheckDecl);
	static AosDocFileMgrObjPtr serializeDfmStatic(
			const AosRundataPtr &rdata,
			const AosFileGroupMgrPtr &fgm,
			const AosBuffPtr &dfm_buff AosMemoryCheckDecl);

	virtual u32		getId(){ return mId; };
	virtual u32		getVirtualId(){ return mVirtualId; };
	virtual bool	stop();
	virtual bool	startStop();
	virtual AosDfmDocType::E getDocType(){ return mConfig.mDocType; };

	// Ketty 2012/10/22
	virtual bool	saveDoc(const AosRundataPtr &rdata,
						vector<AosTransId> &trans_ids,
						const AosDfmDocPtr &doc,
						const bool flushflag = false);
	
	virtual bool	deleteDoc(const AosRundataPtr &rdata,
						vector<AosTransId> &trans_id,
						const AosDfmDocPtr &doc,
						const bool flushflag = false);
	
	virtual AosDfmDocPtr readDoc(const AosRundataPtr &rdata,
						const u64 docid,
						const bool read_body = true);

	virtual bool 	saveDoc(const AosRundataPtr &rdata,
						const AosTransId &trans_id,
						const AosDfmDocPtr &doc,
						const bool flushflag = false);
	
	virtual bool 	deleteDoc(const AosRundataPtr &rdata,
						const AosTransId &trans_id,
						const AosDfmDocPtr &doc,
						const bool flushflag = false);

	// Chen Ding, 2013/02/19
	virtual bool saveHeader(const AosRundataPtr &rdata, 
					const AosDfmDocPtr &doc);

	// this func is used by FileGroupMgr.
	virtual bool removeAllFiles(const AosRundataPtr &rdata);
	static bool removeFilesByDfmBuff(
				const AosRundataPtr &rdata,
				const u32 dfm_id,
				const AosBuffPtr &dfm_buff);

	// this func used by DfmHeader or DfmBody
	//virtual bool removeHeaderFiles(
	//			const AosRundataPtr &rdata,
	//			const u32 snap_id);
	
	virtual bool addHeaderFile(
				const AosRundataPtr &rdata,
				const AosDfmHeaderFilePtr &file);

	virtual bool addBodyFile(
				const AosRundataPtr &rdata,
				const AosDfmFilePtr &file);

	AosDfmHeaderPtr getDfmHeaderObj() const;
	AosDfmBodyPtr   getDfmBodyObj() const;
	
	bool	isSameConf(const AosDfmConfig &conf){ return mConfig.isSame(conf); };

	virtual bool recoverDfmLog(const AosRundataPtr &rdata, const u64 &file_id);

	//virtual bool commitDfmLog(
	//		const AosRundataPtr &rdata, 
	//		const AosDfmLogFilePtr &dfmlogfile);
	
private:
	bool saveToFile(const AosRundataPtr &rdata);
	bool saveToFileLocked(const AosRundataPtr &rdata);

public:
	bool saveDocAsync(
		const AosRundataPtr &rdata,
		vector<AosTransId> &trans_ids,
		const AosDfmDocPtr &doc,
		const AosDfmCallerPtr &caller,
		const bool flushflag);

	bool deleteDocAsync(
		const AosRundataPtr &rdata,
		vector<AosTransId> &trans_ids,
		const AosDfmDocPtr &doc,
		const AosDfmCallerPtr &caller,
		const bool flushflag);

	bool readDocAsync(
		const AosRundataPtr &rdata,
		const u64 docid,
		const bool read_body,
		const AosDfmCallerPtr &caller);

	virtual bool saveDocs(
		const AosRundataPtr &rdata,
		map<u64, u64> &index_map,
		const AosBuffPtr &buff);

public:
	//--------------- Following is interface use snapshot ----------- 
	virtual u64 createSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosTransId &trans_id);	

	virtual bool commitSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosTransId &trans_id);

	virtual bool rollbackSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosTransId &trans_id);

	virtual bool mergeSnapshot(
				const AosRundataPtr &rdata,
				const u64 target_snap_id,
				const u64 merger_snap_id,
				const AosTransId &trans_id);

	virtual AosDfmDocPtr readDoc(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const u64 docid,
				const bool read_body = true);
	
	virtual bool saveDoc(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosDfmDocPtr &doc,
				vector<AosTransId> &trans_ids);

	virtual bool deleteDoc(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosDfmDocPtr &doc,
				vector<AosTransId> &trans_ids);

public:
	//--------------- Following is interface for DfmSnapshotMgr ----------- 
	bool 	commit(
				const AosRundataPtr &rdata,
				const AosDfmSnapshotObjPtr &snapshot);

	bool 	rollback(
				const AosRundataPtr &rdata,
				const AosDfmSnapshotObjPtr &snapshot);

	bool 	merge(
				const AosRundataPtr &rdata,
				const AosDfmSnapshotObjPtr &target_snapshot,
				const AosDfmSnapshotObjPtr &merger_snapshot);
	//--------------- Interface for DfmSnapshotMgr end -------- 


private:
	AosDfmDocPtr readDocFromSnapshot(
				const AosRundataPtr &rdata,
				const u64 docid,
				const bool read_body,
				const u64 snap_id,
				bool &exist);

	AosDfmDocPtr readDocFromDfm(
				const AosRundataPtr &rdata,
				const u64 docid,
				const bool read_body);
	
	bool	 saveDocToDfm(
				const AosRundataPtr &rdata,
				const AosDfmDocPtr &doc);
	
	bool 	deleteDocFromDfm(
				const AosRundataPtr &rdata,
				const AosDfmDocPtr &doc);
	
	bool 	saveDocSnapKeepNew(
				const AosRundataPtr &rdata,
				const AosDfmDocPtr &doc,
				const AosDfmSnapshotObjPtr &snapshot);
	
	bool 	deleteDocSnapKeepNew(
				const AosRundataPtr &rdata,
				const AosDfmDocPtr &doc,
				const AosDfmSnapshotObjPtr &snapshot);

	bool 	flushSnapshotToDfm(
				const AosRundataPtr &rdata,
				const AosDfmDocHeaderPtr &snap_header);
	
	bool 	cleanSnapshotFromDfm(
				const AosRundataPtr &rdata,
				const AosDfmDocHeaderPtr &snap_header);

	bool 	flushEntryToSnapshot(
				const AosRundataPtr &rdata,
				const AosDfmSnapshotObjPtr &target_snapshot,
				const AosDfmDocHeaderPtr &new_entry);
	
	AosFmtPtr createFmt(const AosRundataPtr &rdata);

	bool 	sendFmt(
				const AosRundataPtr &rdata,
				const AosFmtPtr &fmt);

private:
	bool 	saveBody(
				const AosRundataPtr &rdata,
				const AosDfmDocPtr &doc);

};
#endif
