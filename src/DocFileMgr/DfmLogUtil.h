////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 08/14/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DfmLogUtil_h
#define AOS_DocFileMgr_DfmLogUtil_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "aosUtil/Types.h"
#include "DocFileMgr/Ptrs.h"
#include "DfmUtil/Ptrs.h"
#include "DfmUtil/DfmCompareFun.h"
#include "Rundata/Ptrs.h"
#include "Util/TransId.h"
#include "Util/Ptrs.h"

#define AOS_DFMLOG_DIRNAME 			"DfmLog"
#define AOS_NORMAl_DIRNAME_PRE    	"norm"

#include <vector>

class AosDfmLogUtil
{

	enum
	{
		eEntryStart = 0,
		eMaxNameLen = 300,
		eEntrySize = sizeof(u64) + eMaxNameLen 
	};
private:
	static OmnMutexPtr 		smLock;
	static OmnFilePtr 		smAllocFile;
	static u64				smFileSeqno;
	static OmnString 		smBaseDirName;
	static bool				smInit;

public:
	static AosDfmDocPtr getDocInfoFromBuff(
			const AosRundataPtr &rdata,
			const AosDfmDocPtr &temp_doc,
			const AosBuffPtr &buff);

	static AosDfmDocPtr getDocInfoFromBuff(
			const AosDfmDocPtr &temp_doc,
			const AosBuffPtr &buff,
			vector<AosTransId> &trans_ids,
			const bool read_trans_ids = true);

	static bool transIdSerializeTo(
			const AosBuffPtr &buff, 
			vector<AosTransId> &trans_ids);

	static bool serializeFromTransId(
			const AosBuffPtr &buff, 
			vector<AosTransId> &trans_ids,
			const bool read_trans_ids);

	static bool setDocInfoToBuff(
			const AosRundataPtr &rdata,
			const AosBuffPtr &buff,
			const AosDfmDocPtr &doc,
			vector<AosTransId> &trans_ids);

	static OmnString composeFname(
			const u32 siteid,
			const u32 dfm_id,
			const u32 crt_file_seqno);

	//static bool getBaseDirName(vector<OmnString> &vv);

	//static void	getFileNames(
	//		const AosRundataPtr &rdata,
	//		vector<OmnString> &file_path,
	//		const u32 vid);

	//static u32 getCrtFileSeqno(
	//		const AosRundataPtr &rdata,
	//		const u32 vid,
	//		const u32 dfm_id);

	static void print(
			const bool show_log,
			const OmnString &name,
			AosDfmCompareFunType::set_f &vv);

	static void printSetFs(
			const bool show_log,
			const OmnString &name,
			AosDfmCompareFunType::set_fs &docs);

	static void	getNormDirFileNames(
			const AosRundataPtr &rdata,
			vector<OmnString> &file_path,
			const u32 vid);

	//static void	getSnapShotDirFileNames(
	//		vector<OmnString> &file_path,
	//		const u64 snap_id,
	//		const u32 vid, 
	//		const u32 siteid,
	//		const u32 dfm_id);

	static OmnFilePtr createNewFile(
			const u32 vid, 
			const u32 siteid,
			const u32 dfm_id,
			//const u64 snap_id,
			u64 &file_id);

	static OmnFilePtr openFile(const u64 file_id);

	static bool removeFile(const u64 file_id);

	static bool moveFile(const u64 file_id, const u32 vid);

	static OmnString getNormDirName();

	static bool createNormDir(const u32 vid);

	//static OmnString getSnapShotDirName(const u64 snap_id);

	//static bool createSnapShotDir(const u32 vid, const u64 snap_id);

	//static bool removeSnapShotDir(const u32 vid, const u64 snap_id);

	static OmnString getVidDirName(const u32 vid);

	static bool createVidDir(const u32 vid);

	static OmnString getDfmLogBaseDirName();

	static OmnString getBaseDirname();

	static bool removeEmptyDir(const OmnString &path);

	//static OmnString getSnapShotFullPath(const u32 vid, const u64 snap_id);

	static bool createDfmLogBaseDir();

private:
	static void init();
};
#endif
