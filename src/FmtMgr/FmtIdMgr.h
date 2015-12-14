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
// 08/15/2013 	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_FmtMgr_FmtIdMgr_h
#define AOS_FmtMgr_FmtIdMgr_h

#include "alarm_c/alarm.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"

class AosFmtIdMgr: public OmnRCObject 
{
	OmnDefineRCObject;
	
	enum
	{
		eNumFmtId = 100,
	};

private:
	OmnMutexPtr		mLock;
	OmnString		mFname;
	OmnFilePtr		mFile;
	u64				mNextFmtId; 
	u32				mNumFmtId;

public:
	AosFmtIdMgr(const OmnString &dir_name);
	~AosFmtIdMgr();
	
	u64 nextFmtId();
	u64 nextSendFmtId();
	bool resetFmtId(const u64 fmt_id);
	bool 	init();

private:
	bool 	loadNewFmtId(const u64 crt_seqno);
	bool 	saveNewFmtId(const u64 new_id);

};
#endif

