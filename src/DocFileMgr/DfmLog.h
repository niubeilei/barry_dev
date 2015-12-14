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
#ifndef AOS_DocFileMgr_DfmLog_h
#define AOS_DocFileMgr_DfmLog_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocFileMgr/Ptrs.h"
#include "DfmUtil/Ptrs.h"
#include "DfmUtil/DfmDoc.h"
#include "DfmUtil/DfmCompareFun.h"
#include "DocFileMgr/Ptrs.h"
#include "FmtMgr/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"

class AosDfmLog: virtual public OmnRCObject
{
	OmnDefineRCObject;

private:

	u32 						mVirtualId;
	u32 						mDfmId;
	AosDocFileMgrObjPtr			mDocFileMgr;

	AosDfmBodyPtr				mBody;
	AosDfmHeaderPtr				mHeader;
	u32							mSnapId;
	
	bool						mShowLog;
	AosDfmLogFilePtr			mDfmLogFile;

	static 	AosDfmLogProcThrdPtr	smProcThrdObj;	
public:
	AosDfmLog(
			const AosRundataPtr &rdata,
			const AosDfmDocType::E doc_type,
			const AosDocFileMgrObjPtr &mgr, 
			const u32 virtual_id,
			const u32 dfm_id,
			const bool show_log);

	~AosDfmLog();

	static bool config(const AosXmlTagPtr &app_config);

	bool addEntry(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc,
		vector<AosTransId> &trans_ids);

	AosDfmDocPtr readDoc(
		const AosRundataPtr &rdata, 
		const u64 docid);

	bool saveDoc(
		const AosRundataPtr &rdata,
		const u64 &fileid,
		const bool init_flag);

	bool stop();

	bool startStop(const AosRundataPtr &rdata);

	static bool continueProcCache();

	static bool stopProcCache();

 	static bool init(
		const AosRundataPtr &rdata,
		const u32 &virtual_id,
		const AosFileGroupMgrPtr &fgm);

	static bool clear(
		const AosRundataPtr &rdata,
		const u32 &virtual_id);

	static u32 getMaxFileSize();
	
	bool recoverDfmLog(
		const AosRundataPtr &rdata,
		const u64 &file_id);
	
	bool addRequest(
		const AosRundataPtr &rdata,
		const u64 &file_id);

	u32 getVirtualId(){return mVirtualId;}

	AosDfmDocPtr readFromFile(
		const AosRundataPtr &rdata,
		const u64 &file_id);

	//bool commit(
	//	const AosRundataPtr &rdata, 
	//	const AosDfmLogFilePtr &dfmlogfile);

	bool addEntrys(
		const AosRundataPtr &rdata,
		map<u64, u64> &index_map,
		const AosBuffPtr &buff);

	bool flushContents(const AosRundataPtr &rdata);
private:
	bool saveDocPriv(
		const AosRundataPtr &rdata,
		const u64 &file_id);

	bool readDfmLogFile(
		const AosRundataPtr &rdata,
		const u64 &file_id,
		AosDfmCompareFunType::set_f &vv,
		bool &no_send_fmt);
};

#endif
