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
#ifndef AOS_FmtMgr_FmtSvr_h
#define AOS_FmtMgr_FmtSvr_h

#include "API/AosApi.h"
#include "FmtMgr/Ptrs.h"
#include "ReplicMgr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "SEInterfaces/FmtMgrObj.h"
#include "Util/Ptrs.h"

#include <vector>
#include <set>
#include <map>
using namespace std;

class AosFmtSvr: public AosFmtMgrObj 
{
	OmnDefineRCObject;
	
	enum
	{
		eMaxLogFileSize = 1 * 1000 * 1000 * 1000,		// 1G.
	};

private:
	bool				mIsMaster;

protected:
	OmnString			mDirname;
	AosFmtFilePtr		mFmtFile;
	//vector<u32>			mSvrIds;
	
	AosRundataPtr		mRdata;
	bool				mShowLog;

public:
	AosFmtSvr(const bool is_master);
	~AosFmtSvr();

	virtual bool config(const AosXmlTagPtr &def);
	virtual bool start();
	virtual bool stop();
	bool 	switchFrom(const AosFmtSvrPtr &from);

	bool	isMaster(){ return mIsMaster; };

	virtual bool readFmtBySync(
				const AosRundataPtr &rdata,
				const u32 cube_grp_id,
				const u64 beg_fmt_id,
				const u64 end_fmt_id,
				int &file_seq,
				bool &finish,
				AosBuffPtr &fmt_buff);
	
	//==================== Bkp FmtSvr Func=========.
	virtual bool recvFinishedFmtId(const u64 max_fmt_id);
	//==================== Bkp FmtSvr Func End=========.

	//==================== Master FmtSvr Func=========.
	virtual bool getNextSendFmtId(
				const AosRundataPtr &rdata,
				const u32 cube_grp_id,
				const int from_svr_id,
				u64 &next_fmt_id);
	virtual bool sendFmt(
				const AosRundataPtr &rdata,
				const AosFmtPtr	&fmt);
	
	virtual void setFmt(const AosFmtPtr &fmt);

	virtual AosFmtPtr getFmt();

	virtual bool addToGlobalFmt(
				const AosRundataPtr &rdata, 
				const u64 file_id,
				const u64 offset,
				const char *data,
				const int len,
				const bool flushflag);

	virtual bool addToGlobalFmt(
				const AosRundataPtr &rdata,
				const AosStorageFileInfo &file_info);

	virtual bool addDeleteFileToGlobalFmt(
				const AosRundataPtr &rdata, 
				const u64 &file_id);

	virtual bool addToGlobalFmt(
				const AosRundataPtr &rdata, 
				const AosFmtPtr &fmt);
	//==================== Master FmtSvr Func End =========.

protected:
	u64 	getMaxFmtIdInFile();

private:
	bool 	readFmtBySyncPriv(
				const AosRundataPtr &rdata,
				const u64 beg_fmt_id,
				const u64 end_fmt_id,
				int &file_seq,
				bool &finish,
				AosBuffPtr &fmt_buff);

};
#endif
