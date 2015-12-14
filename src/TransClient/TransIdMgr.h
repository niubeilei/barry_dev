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
// 05/30/2011 	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransClient_TransIdMgr_h
#define AOS_TransClient_TransIdMgr_h

#include "alarm_c/alarm.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "Thread/Ptrs.h"
#include "TransClient/Ptrs.h"
#include "Util/TransId.h"
#include "Util/Ptrs.h"
#include "Util/String.h"

class AosTransIdMgr: public OmnRCObject 
{
	OmnDefineRCObject;
	
	enum
	{
		eNumTransId = 100,
	};

private:
	OmnMutexPtr		mLock;
	OmnFilePtr		mFile;
	u64				mNextTransSubId; 
	u32				mNumTransId;

public:
	AosTransIdMgr();
	~AosTransIdMgr();
	
	bool		start();
	AosTransId  nextTransId();

private:
	bool 	checkIdFile();
	bool 	notifyIsNewTransClt();
	bool 	init();
	bool 	loadNewTransId(const u64 crt_seqno);

};
#endif

