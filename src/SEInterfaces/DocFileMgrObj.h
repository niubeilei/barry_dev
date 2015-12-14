////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 08/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DocFileMgrObj_h
#define Aos_SEInterfaces_DocFileMgrObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "DfmUtil/DfmDoc.h"
#include "DocFileMgr/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
//#include "ReliableFile/Ptrs.h"
#include "Util/TransId.h"
#include "Util/MemoryChecker.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosDocFileMgrObj : public OmnRCObject
{
public:
	enum 
	{
		eASEID_Doc = 10,
		eASEID_IIL = 11,
		eASEID_BITMAP = 12
	};

protected:
	//static AosDocFileMgrObjPtr		smCreator;

private:
	// Chen Ding, 12/01/2012
	AosDocFileMgrObjPtr mPrevEntry;
	AosDocFileMgrObjPtr mNextEntry;

public:
	AosDocFileMgrObj();
	virtual ~AosDocFileMgrObj();
	
	enum
	{
		//eReservedSize = 1000,	
		eReservedSize = 0,	
		eMaxRecordFileSize = 1000*1000*1000	// 1G, Chen Ding, 03/12/2012
	};

	// Chen Ding, 12/05/2012
	AosDocFileMgrObjPtr nextEntry() const {return mNextEntry;}
	AosDocFileMgrObjPtr prevEntry() const {return mPrevEntry;}
	void resetPointers() {mPrevEntry = 0; mNextEntry = 0;}
	void setPointers() {mPrevEntry = this; mNextEntry = this;}
	bool moveToFront(const AosDocFileMgrObjPtr &docFileMgr);
	bool insertAt(const AosDocFileMgrObjPtr &docFileMgr);
	bool removeFromList();

	virtual bool init() = 0;

	virtual bool saveDoc(const AosRundataPtr &rdata,
					vector<AosTransId> &trans_ids,
					const AosDfmDocPtr &doc,
					const bool flushflag = false) = 0;

	virtual bool deleteDoc(const AosRundataPtr &rdata,
			        vector<AosTransId> &trans_id,
					const AosDfmDocPtr &doc,
					const bool flushflag = false) = 0;
	
	virtual bool saveDoc(const AosRundataPtr &rdata,
					const AosTransId &trans_id,
					const AosDfmDocPtr &doc,
					const bool flushflag = false) = 0;
	
	virtual bool deleteDoc(const AosRundataPtr &rdata,
					const AosTransId &trans_id,
					const AosDfmDocPtr &doc,
					const bool flushflag = false) = 0;

	virtual AosDfmDocPtr readDoc(const AosRundataPtr &rdata,
						const u64 docid,
						const bool read_body = true) = 0;

	// Ketty 2014/03/05
	//virtual bool removeHeaderFiles(
	//				const AosRundataPtr &rdata,
	//				const u32 snap_id) = 0;
	virtual bool addHeaderFile(
					const AosRundataPtr &rdata,
					const AosDfmHeaderFilePtr &file) = 0;
	virtual bool addBodyFile(
					const AosRundataPtr &rdata,
					const AosDfmFilePtr &file) = 0;
	virtual bool removeAllFiles(const AosRundataPtr &rdata) = 0;

	// Ketty 2014/03/05
	//virtual bool addSnapShot(
	//				const AosRundataPtr &rdata,
	//				u32 &snap_id) = 0;
	//virtual bool removeSnapShot(
	//				const AosRundataPtr &rdata,
	//				const u32 snap_id) = 0;
	//virtual bool addAndRemoveSnapShot(
	//				const AosRundataPtr &rdata,
	//				u32 &snap_id) = 0;
	//virtual bool rollBack(
	//				const AosRundataPtr &rdata,
	//				const u32 snap_id) = 0;
	//virtual bool commit(const AosRundataPtr &rdata) = 0;

	virtual bool 	recoverDfmLog(const AosRundataPtr &rdata, const u64 &file_id) = 0;

	//virtual bool 	commitDfmLog(
	//					const AosRundataPtr &rdata, 
	//					const AosDfmLogFilePtr &dfmlogfile) = 0;

	// The following is fo AosFileGroupMgr.
	virtual bool		stop() = 0;
	virtual bool		startStop() = 0;
	virtual u32			getId() = 0;
	virtual u32			getVirtualId() = 0;
	// end.
	
	virtual AosDfmDocType::E getDocType() = 0;

	// Ketty 2013/01/19
	inline static u64 getMaxRecordFileSize() {return eMaxRecordFileSize;}
	static int getReservedSize() {return eReservedSize;}
	inline static bool getPosition(
	 			const u64 &local_id, 
	 			const u32 record_size,
	 			u32 &seqno, 
	 			u64 &offset, 
	 			const u64 &max_file_size)
	{
	 	aos_assert_r(max_file_size > 0, false);
	 	
	 	u64 file_size = max_file_size - eReservedSize;
	 	u32 docs_per = file_size / record_size;
	 	seqno = local_id / docs_per;
	 	offset = (local_id % docs_per) * record_size + eReservedSize;
	 	return true;
	}

	//static void setCreator(const AosDocFileMgrObjPtr &creator) {smCreator = creator;}
	//static AosDocFileMgrObjPtr getCreator() {return smCreator;}

	virtual bool saveDocs(
			const AosRundataPtr &rdata,
			map<u64, u64> &index_map,
			const AosBuffPtr &buff) = 0;

	// Chen Ding, 2013/02/19
	virtual bool saveHeader(const AosRundataPtr &rdata, 
					const AosDfmDocPtr &doc) = 0;

	//	Following is interface use snapshot 
	virtual u64 createSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosTransId &trans_id) = 0;

	virtual bool commitSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosTransId &trans_id) = 0;

	virtual bool rollbackSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosTransId &trans_id) = 0;
	
	virtual bool mergeSnapshot(
				const AosRundataPtr &rdata,
				const u64 target_snap_id,
				const u64 merger_snap_id,
				const AosTransId &trans_id) = 0;

	virtual AosDfmDocPtr readDoc(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const u64 docid,
				const bool read_body = true) = 0;

	virtual bool saveDoc(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosDfmDocPtr &doc,
				vector<AosTransId> &trans_ids) = 0;

	virtual bool deleteDoc(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosDfmDocPtr &doc,
				vector<AosTransId> &trans_ids) = 0;
	
};

#endif

