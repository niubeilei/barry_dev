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
// 06/17/2013	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_FmtMgr_Fmt_h
#define AOS_FmtMgr_Fmt_h

#include "FmtMgr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "StorageMgrUtil/FileInfo.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/TransId.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <vector>
using namespace std;

class AosFmt: public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum FmtOpr
	{
		eInvalid,
		
		eCreateFile,
		eModifyFile,
		eDeleteFile,
		eTransIds,
		
		eMax,
	};
	
	enum
	{
		eFmtIdOff = 0,
		eEntryStartOff = eFmtIdOff + 8, 
	};

private:
	u64			mFmtId;
	AosBuffPtr	mFmtBuff;
	vector<AosTransId>	mTransIds;

	static bool	smShowLog;

public:
	AosFmt();
	AosFmt(const AosBuffPtr &fmt_buff);
	~AosFmt();

	bool	add(const AosStorageFileInfo &file_info);
	bool	add(const u64 file_id,
				const u64 offset,
				const char *data,
				const int len,
				const bool flushflag);
	bool	addDeleteFile(const u64 file_id);
	bool	add(vector<AosTransId> &trans_ids);
	bool 	appendFmt(const AosFmtPtr &fmt);
	vector<AosTransId> & getTransIds(){ return mTransIds; };

	void	setFmtId(const u64 fmt_id);
	u64		getFmtId(){ return mFmtId; };
	AosBuffPtr	getFmtBuff(){ return mFmtBuff; };

	FmtOpr 	nextFmtOpr();
	bool	getOprInfo(AosStorageFileInfo &file_info);
	bool	getOprInfo(u64 &file_id, 
				u64 &file_offset, 
				AosBuffPtr &data, 
				bool &flushflag);
	bool	getOprInfo(vector<AosTransId> &trans_ids);
	bool	getOprInfo(u64 &file_id);

	void 	reset(const int64_t len = 100)
	{
		if (mFmtBuff->dataLen() > 0)
		{
			mFmtBuff = OmnNew AosBuff(len, 0 AosMemoryCheckerArgs);
			memset(mFmtBuff->data(), 0, len);
		}
	}

	static u64	getFmtIdFromBuff(const AosBuffPtr &buff);
	static AosFmtPtr getNextFmt(const AosBuffPtr &cont);

	static void sendGlobalFmt(
			const AosRundataPtr &rdata,
			const bool need_lock = true);

	static bool addToGlobalFmt(
				const AosRundataPtr &rdata, 
				const u64 file_id,
				const u64 offset,
				const char *data,
				const int len,
				const bool flushflag);
	static void setShowLog(const bool show_log){ smShowLog = show_log; };

private:
	bool 	getOprInfoCheck(const FmtOpr &check_opr);

};
#endif
