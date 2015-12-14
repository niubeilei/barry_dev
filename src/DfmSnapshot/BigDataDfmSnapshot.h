////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2014/02/26 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DfmSnapShot_BigDataDfmSnapshot_h
#define AOS_DfmSnapShot_BigDataDfmSnapshot_h

#include "DfmSnapshot/Ptrs.h"
#include "DfmUtil/Ptrs.h"
#include "UtilData/DfmDocType.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DfmSnapshotObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

class AosBigDataDfmSnapshot : public AosDfmSnapshotObj 
{
	OmnDefineRCObject;
	
	enum
	{
		eInvalidEntry = 0,
		eValidEntry = 1,
	};
	
	enum
	{
		//eStatusOff = 0,
		eEntryStart = 100,
	};
	
private:
	OmnMutexPtr			mLock;
	AosDfmSnapshotMgrPtr mSnapshotMgr;
	u64 				mSnapId;
	AosDfmDocType::E	mDocType;
	AosSnapshotSts::E		mStatus;
	AosDfmDocPtr	    mTmpDfmDoc;
	
	AosReliableFilePtr  mLogFile;
	u32				   mHeaderDataLen;
	u32				   mEntrySize;
	bool			   mFileIsProntToEnd;
	map<u64, u64>	   mIdToOffsetMap;
	bool			   mIdMapInited;
	
	bool			   mShowLog;
	
public:
	AosBigDataDfmSnapshot(
		const AosDfmSnapshotMgrPtr & snapshot_mgr,
		const u64 snap_id,
		const AosDfmDocType::E doc_type);
	~AosBigDataDfmSnapshot();

	static AosDfmSnapshotObjPtr createSnapshotStatic(
				const AosRundataPtr &rdata,
				const AosDfmSnapshotMgrPtr &snapshot_mgr, 
				const u64 snap_id,
				const AosDfmDocType::E doc_type);

	static AosDfmSnapshotObjPtr createSnapshotStatic(
				const AosRundataPtr &rdata,
				const AosDfmSnapshotMgrPtr &snapshot_mgr, 
				const AosBuffPtr &buff,
				const AosDfmDocType::E doc_type);
	
	virtual bool commitFinish(const AosRundataPtr &rdata);
	virtual bool rollbackFinish(const AosRundataPtr &rdata);
	virtual bool mergeFinish(const AosRundataPtr &rdata);
	virtual bool setStatus(const AosSnapshotSts::E sts);
	virtual AosSnapshotSts::E getStatus(){ return mStatus; };
	virtual bool isActive(){ return mStatus == AosSnapshotSts::eActive; };

	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual u64 getSnapId(){ return mSnapId; };

	virtual bool readEntry(
					const AosRundataPtr &rdata,
					const u64 docid,
					bool &exist,
					AosDfmDocHeaderPtr &header);

	virtual bool addEntry(
					const AosRundataPtr &rdata,
					const AosDfmDocHeaderPtr &header,
					const bool is_merge = false);

	virtual bool removeEntry(
					const AosRundataPtr &rdata,
					const u64 docid);
	
	virtual bool getEntrys(
					const AosRundataPtr &rdata, 
					vector<AosDfmDocHeaderPtr> &entrys,
					AosSnapshotReadInfo &read_info);
	
	void	setShowLog(){ mShowLog = true; };
	
private:	
	bool 	init(const AosRundataPtr &rdata);
	bool 	createLogFile(const AosRundataPtr &rdata);

	bool 	serializeFrom(
				const AosRundataPtr &rdata,
				const AosBuffPtr &buff);
	bool 	openLogFile(
				const AosRundataPtr &rdata,
				const u64 log_file_id);
	
	bool 	initIdMapPriv(const AosRundataPtr &rdata);
	
	bool 	removeLogFile(const AosRundataPtr &rdata);

	bool 	readEntryFromBuff(
				const AosRundataPtr &rdata,
				const AosBuffPtr &entry_buff,
				AosDfmDocHeaderPtr &header,
				bool &finished);

	bool 	readBuffFromFile(
				const AosRundataPtr &rdata,
				const u64 file_off,
				const AosBuffPtr &buff,
				bool &finished);

	//bool 	readBuffByReadInfo(
	//			const AosRundataPtr &rdata,
	//			AosSnapshotReadInfo &read_info);

	bool 	appendEntry(
				const AosRundataPtr &rdata,
				const AosDfmDocHeaderPtr &header);

	bool 	modifyEntry(
				const AosRundataPtr &rdata,
				const AosDfmDocHeaderPtr &header,
				const u64 file_off);

	bool 	appendToLogFile(
				const AosRundataPtr &rdata,
				const u64 file_off,
				const AosDfmDocHeaderPtr &header);

	u64		getFileOffsetFromMap(const AosRundataPtr &rdata, const u64 docid);
	bool 	addInfoToMap(const u64 docid, const u64 offset);

};
#endif
