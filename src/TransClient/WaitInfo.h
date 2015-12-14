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
// 2013/07/18 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransClient_WaitInfo_h
#define AOS_TransClient_WaitInfo_h

#include "alarm_c/alarm.h"
#include "API/AosApiG.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "TransClient/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

#include <set>
using namespace std;

class AosWaitInfo: public OmnRCObject 
{
	OmnDefineRCObject;
	
	enum
	{
		eWakeupAckNum = 1,
	};

private:
	AosTransPtr	mTrans;
	u32			mNeedAckNum;
	set<int>	mAckSids;
	set<int>	mDeathSids;
	u32			mWaitTime;
	bool		mShowLog;

public:
	AosWaitInfo(const AosTransPtr &trans, const bool show_log);

	// the trans client has locked.
	void 	addAck(const int death_sid);
	void 	addDeathSvr(const int death_sid);
	
	bool 	isAckFinish();
	bool 	isAllSvrDeath();
	//bool 	canWakeup();

	AosTransPtr getTrans(){ return mTrans; };
	void	cleanWaitTime(){ mWaitTime = 0; };
	void	addWaitTime(){ mWaitTime++; };
	u32		getWaitTime(){ return mWaitTime; };
	OmnString toString();

};
#endif
