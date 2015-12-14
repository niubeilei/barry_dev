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
// 04/11/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocFileMgr_DfmFmtTester_DfmInfo_h
#define Aos_DocFileMgr_DfmFmtTester_DfmInfo_h 

#include "Thread/Ptrs.h"
#include "DocFileMgr/Ptrs.h"
#include "DfmUtil/DfmConfig.h"
#include "DocFileMgr/DfmFmtTester/DocInfo.h"
#include "DocFileMgr/DfmFmtTester/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

#include "Thread/Mutex.h"

#include <set>
#include <vector>
using namespace std;


class AosDfmInfo : public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eTotalMem = 5 * 1000 * 1000,		// 5G
		eMaxDocs = (eTotalMem / sizeof(AosDocInfo3) ) * 1000,
	};

private:
	OmnMutexPtr		mLock;
	u32				mSiteid;
	map<u64, AosDocInfo3Ptr> 	mDocInfo;
	u32			 	mCrtDocInfoNum;
	u32				mVId;
	
	bool			mShowLog;

public:
	AosDfmInfo();
	AosDfmInfo(
		const AosRundataPtr &rdata,
		const u32 vid,
		const bool show_log);
	~AosDfmInfo();
	
	AosDocInfo3Ptr addDoc(const AosRundataPtr &rdata, const u64 docid);
	bool 	randModifyDoc(const AosRundataPtr &rdata);
	bool 	randDeleteDoc(const AosRundataPtr &rdata);
	bool 	randReadDoc(const AosRundataPtr &rdata);

	bool 	serializeFrom(
				const AosBuffPtr &buff);
	bool 	serializeTo(const AosBuffPtr &buff);

	u32		getVirtualId(){ return mVId; };

	u64     getDfmInfoId()
	{
		u64 dfm_info_id = mVId;                     
		dfm_info_id = (dfm_info_id << 32) + mSiteid;
	};

	u64		getMaxDocid()
	{
		map<u64, AosDocInfo3Ptr>::reverse_iterator ritr = mDocInfo.rbegin();
		if(ritr == mDocInfo.rend())	return 0;
		
		return ritr->first;
	}
private:
	AosDocInfo3Ptr 	randGetDocInfo();

};

#endif
