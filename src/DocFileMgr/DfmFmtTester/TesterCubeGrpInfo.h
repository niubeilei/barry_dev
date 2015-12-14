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
//
// Modification History:
// 2013/07/01	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocFileMgr_DfmFmtTester_TesterCubeGrpInfo_h
#define Aos_DocFileMgr_DfmFmtTester_TesterCubeGrpInfo_h 

#include "Rundata/Ptrs.h"
#include "RlbTester/Client/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/LRUCache.h"
#include "Util/Ptrs.h"

#include <map>
using namespace std;
	
class AosTesterCubeGrpInfo: public OmnRCObject
{
	OmnDefineRCObject;

private:
	u32	 		mCubeGrpId;
	int 		mLastDeathSvrId;
	u32 		mLastDeathProcId;
	map<int, u32>	mActiveSvrProcId;
	map<int, u32>	mDeathSvrProcId;
	bool		mShowLog;

public:
	AosTesterCubeGrpInfo(const u32 grp_id, const bool show_log);
	
	bool 	monitoyProc();

private:
	bool 	randStartProc();
	bool 	randKillProc();
	bool 	startProc(const int svr_id, const u32 proc_id);

};

#endif
