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
// 2014/11/07 Created by Chen Ding.
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_BlobSE_BlobSE_h
#define AOS_BlobSE_BlobSE_h

#include <vector>

#include "BlobSE/BlobHeader.h"
#include "BlobSE/Marker.h"
#include "BlobSE/Ptrs.h"
#include "DfmUtil/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "RawSE/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
//#include "SEInterfaces/BlobSEObj.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosBlobSE : 	//public AosBlobSEObj,
					virtual public OmnRCObject

{
	OmnDefineRCObject;

public:
	enum
	{
		eSiteID				= 1,
		eCubeID				= 1,
		eAseID				= 1,
		eDftActiveLogSize	= 100000000,	//100M
	};

	struct Config
	{
		u32		siteid;
		u32		cubeid;
		u64		aseid;
		u64		active_log_max_size;
		u64		max_doc_size;
		u64		change_log_size;
		u32		header_custom_data_size;
		u64		write_req_time_window;
		bool	change_log_enabled;
		bool	show_alarm_on_doc_not_exist;
		bool	strict_sanity_check;

		Config()
		:
		siteid(0),
		cubeid(0),
		aseid(0),
		active_log_max_size(0),
		max_doc_size(0),
		change_log_size(0),
		header_custom_data_size(0),
		write_req_time_window(0),
		change_log_enabled(false),
		show_alarm_on_doc_not_exist(true),
		strict_sanity_check(false)
		{
		}
	};

private:
	OmnMutex*				mLockRaw;
	OmnMutex*				mActiveRawFileLock;
	OmnMutexPtr				mLock;
	AosRawSEPtr				mRawSE;
	AosRawFilePtr			mActiveRawFile;
	AosBlobHeaderPtr		mHeader;
	AosDfmDocPtr			mDoc;
	AosHeaderCachePtr		mHeaderCache;
	AosBodyCachePtr			mBodyCache;
	AosBlobSESnapshotMgrPtr	mSnapshotMgr;
	AosOprIDCachePtr		mOprIDCache;
	Config					mConfig;
	u64						mLastSavedEntryRawfid;
	u64						mLastSavedEntryOffset;
	u64						mLastLogFileID;
	u64						mLastAppendedOprID;
	bool					mIsFirstStart;
	u64                     mActiveFileSize;
	AosMarkerMgrPtr			mMarkerMgr;

public:
	AosBlobSE(
			AosRundata 					*rdata,
			const AosDfmDocPtr			&doc,
			const AosXmlTagPtr			&conf);
	~AosBlobSE();

	int saveDoc(
			const AosRundataPtr	&rdata,
			const u32			ulTermID,
			const u64			ullOprID,
			AosBlobSEReqEntry*	pReq,
			AosBlobHeaderPtr&	pHeader);

	int deleteDoc(
			const AosRundataPtr	&rdata,
			const u32			ulTermID,
			const u64			ullOprID,
			AosBlobSEReqEntry*	pReq,
			AosBlobHeaderPtr&	pHeader);

    int getEntryByOprID(
			const AosRundataPtr	&rdata,
			const u64			ullOprID,
			u32					&ulTermID,
			AosBuffPtr			&pBuff);

	int readDoc(
			const AosRundataPtr	&rdata,
			AosBlobSEReqEntry*	pReq);

	int updateHeader(
			AosRundata			*rdata,
			AosBlobHeaderPtr&	pHeader);

	virtual bool saveDoc(
				const AosRundataPtr	&rdata,
				const u64			snap_id,
				const AosDfmDocPtr	&doc,
				const u64			opr_id,
				const u64			timestamp = 0);

	virtual bool modifyDoc(
				const AosRundataPtr	&rdata,
				const u64			snap_id,
				const AosDfmDocPtr	&doc,
				const u64			opr_id,
				const u64			timestamp = 0);

	virtual AosDfmDocPtr readDoc(
				const AosRundataPtr	&rdata,
				const u64			snap_id,
				const u64			docid,
				const u64			timestamp = 0);

	virtual bool deleteDoc(
				const AosRundataPtr	&rdata,
				const u64			snap_id,
				const AosDfmDocPtr	&doc,
				const u64			opr_id,
				const u64			timestamp = 0);

	int searchForSaveMark(
			AosRundata	*rdata,
			u64			&rawfid,
			u64			&offset,
			AosBuffPtr	&buff);

	int getNextHeader(
			AosRundata			*rdata,
			AosBlobHeaderPtr	&header,
			u64					&rawfid,
			u64					&offset,
			AosBuffPtr			&buff,
			u64					&snap_id);

	int shutdown(AosRundata	*rdata);

	int updateHeaders(
			AosRundata							*rdata,
			const std::vector<AosBlobHeaderPtr>	&vHeaders,
			const u64							timestamp);

	const Config getConfig() const {return mConfig;};

	int addToDeleteLog(
			AosRundata				*rdata,
			const AosBlobHeaderPtr	&header);

	int createSnapshot(
			const AosRundataPtr	&rdata,
			const u64			ullSnapshotID);

	int commitSnapshot(
			const AosRundataPtr	&rdata,
			const u64			ullSnapshotID);

	int cancelSnapshot(
			const AosRundataPtr	&rdata,
			const u64			ullSnapshotID);

	int submitSnapshot(
			const AosRundataPtr	&rdata,
			const u64			target_snap_id,
			const u64			submit_snap_id);

	int doCompationAsRaftLeader();

	int doCompactionAsRaftFollower();

	int appendDataToActiveLogFile(
			AosRundata	*rdata,
			const char*	data,
			const u64	len,
			u64			&rawfid,
			u64			&offset);

	int getBodyFileBoundaryByTimestamp(
			AosRundata	*rdata,
			const u64	ullTimestamp,
			u64			&ullBoundaryRawfid,
			u64			&ullBoundaryOffset);

	u64 getTimestampOfTheOldestExistingOldHeaderFileFromMetafile(AosRundata	*rdata);

	int removeEntry(
			AosRundata			*rdata,
			AosBlobHeaderPtr	&pHeader);

	int saveMarkerToMarkerFile(
			AosRundata	*rdata,
			AosMarker	&marker);

	int init(
			AosRundata	*rdata,
			const u64	ullLastAppliedOprID);

	int generateHeaderForOprID(
			AosRundata			*rdata,
			const u64			ullOprID,
			AosBlobHeaderPtr	&pHeader);

	int saveSnapshotOprEntry(
			AosRundata						*rdata,
			const u32						ulTermID,
			const u64						ullOprID,
			const u64						ullSnapshotID,
			AosBlobHeader::OperationType	eOprType);

	u64 getLastAppendOprIDFromActiveLogFile() const {return mLastAppendedOprID;}

	OmnString getBaseDir();

private:
	int config(
			AosRundata*			rdata,
			const AosXmlTagPtr	&conf);

	bool blobTooBig(u64	blob_len);

	bool isHugeBlob(u64	blob_len);

	bool verifyChecksum(
			AosRundata			*rdata,
			const AosBuffPtr	&buff);

	bool isActiveFileTooBig() const;

	int createNewActiveRawFileNoLock(const AosRundataPtr	&rdata);

	bool saveHugeBlob(
			const AosRundataPtr	&rdata,
			const AosDfmDocPtr	&doc,
			const u64			snap_id);

	AosRawFilePtr getPrevActiveLogFile(
			AosRundata	*rdata,
			const u64	rawfid);

	AosRawFilePtr getNextActiveLogFile(
			AosRundata	*rdata,
			const u64	rawfid);

	int getPrevEntry(
			AosRundata	*rdata,
			char*		&pos);

	bool isSavedEntry(const char* buff);

	bool isStartToSaveEntry(const char* buff);

	bool openActiveLogFile(AosRundata	*rdata);

	u64 getLastLogFileIDFromMetafile(AosRundata	*rdata);

	int saveActiveLogFileIDToMetafile(AosRundata	*rdata);

	bool isFirstStart() const {return mIsFirstStart;};

	u64 getNextActiveRawfileRawfid(const u64 rawfid);

	u64 getPrevActiveRawfileRawfid(const u64 rawfid);
};
#endif

