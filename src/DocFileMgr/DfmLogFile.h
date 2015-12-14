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
// 06/18/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DfmLogFile_h
#define AOS_DocFileMgr_DfmLogFile_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocFileMgr/Ptrs.h"
#include "DfmUtil/Ptrs.h"
#include "DfmUtil/DfmDoc.h"
#include "DocFileMgr/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"

class AosDfmLogFile: virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eFlagOff = 0,
		eFmtFlagOff = eFlagOff + sizeof(u32),
		eDfmIdOff = eFmtFlagOff + sizeof(u32),
		eFileIdOff = eDfmIdOff + sizeof(u32),
		eStartOff = eFileIdOff + sizeof(u64) 
	};

	struct DocFileInfo
	{
		int64_t 	offset;
		u64			fileid;
	};

private:
	typedef map<u64, DocFileInfo> map_t;
	typedef map<u64, DocFileInfo> ::iterator mapitr_t;

	u32                         mMaxFileSize;
	u32							mVirtualId;
	u32							mDfmId;
	//u64						mSnapId;
	OmnMutexPtr					mLock;
	OmnFilePtr					mFile;
	u64							mFileId;
	int64_t						mCrtOffset;

	map_t						mDocMap;
	deque<map_t>				mMaps;

	AosDfmLogPtr				mDfmLog;
	OmnString					mType;

	AosDfmDocPtr				mTempDoc;

	bool						mShowLog;

public:
	AosDfmLogFile(
		const AosDfmDocType::E doc_type,
		const u32 file_size, 
		const u32 virtual_id,
		const u32 dfm_id,
		const AosDfmLogPtr &log,
		const bool show_log);

	//AosDfmLogFile(
	//	const AosDfmDocType::E doc_type,
	//	const u32 file_size, 
	//	const u32 virtual_id,
	//	const u32 dfm_id,
	//	const AosDfmLogPtr &log,
	//	const u64 snap_id,
	//	const bool show_log);

	~AosDfmLogFile();

	bool addEntry(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc,
		vector<AosTransId> &trans_ids);

	AosDfmDocPtr  readDoc(
		const AosRundataPtr &rdata, 
		const u64 docid);

	bool addCrtRequest(const AosRundataPtr &rdata);

	bool recoverDfmLog(
		const AosRundataPtr &rdata,
		const OmnFilePtr &file);
	
	bool removeFile(const u64 &file_id);

	AosDfmDocPtr readFromFile(
		const AosRundataPtr &rdata,
		const int64_t &offset,
		const OmnFilePtr &file);

	OmnFilePtr	getFileLocked(
		const AosRundataPtr &rdata,
		const int64_t requested_space);

	bool saveDocPriv(
		const AosRundataPtr &rdata,
		const OmnFilePtr &file);

	AosBuffPtr readDfmLogFile(
		const u64 &file_id,
		const bool mod_flag,
		bool &no_send_fmt);

	bool removeFileIdFromQueue(const u64 &file_id);

	bool readFileIdFromQueue(const u64 &file_id);

	//bool commit(
	//	const AosRundataPtr &rdata, 
	//	const AosDfmLogFilePtr &dfmlogfile);

	//bool getMapQueue(deque<map_t> &maps);

	AosDfmDocPtr	getTempDoc(){return mTempDoc;}

	//OmnString getDfmLogDirPath();

	//bool rollBackFinishTrans(const u64 file_id);

	bool addEntrys(
		const AosRundataPtr &rdata,
		map<u64, u64> &index_map,
		const AosBuffPtr &buff);

private:
	bool findDocLocked(
		const u64 &docid, 
		DocFileInfo &info);

	void addCrtRequestLocked(const AosRundataPtr &rdata); 

};

#endif
