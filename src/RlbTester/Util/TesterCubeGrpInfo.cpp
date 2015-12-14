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
#include "RlbTester/Util/TesterCubeGrpInfo.h"

#include "API/AosApi.h"
#include "TransBasic/AppMsg.h"
#include "SEInterfaces/ProcInfo.h"

#include "RlbTester/TesterTrans/RlbTesterKillProcMsg.h"
#include "RlbTester/TesterTrans/RlbTesterStartProcMsg.h"

// AosTesterCubeGrpInfo Func ===============================
AosTesterCubeGrpInfo::AosTesterCubeGrpInfo(const u32 grp_id, const bool show_log)
:
mCubeGrpId(grp_id),
mLastDeathSvrId(-1),
mLastDeathProcId(0),
mShowLog(show_log)
{
	int svr_id = -1;
	u32 proc_id = 0;
	while(1)
	{
		bool rslt = AosGetNextSvrInfo(mCubeGrpId, svr_id, proc_id);
		aos_assert(rslt);
		if(svr_id == -1)	break;
		
		mActiveSvrProcId.insert(make_pair(svr_id, proc_id));
	}
}


bool
AosTesterCubeGrpInfo::monitoyProc()
{
	u32 act_svr_num = mActiveSvrProcId.size(); 
	u32 death_svr_num = mDeathSvrProcId.size(); 
	aos_assert_r(act_svr_num || death_svr_num, false);

	if(act_svr_num == 0)
	{
		//return randStartProc();
		// first start the last master.
		return startProc(mLastDeathSvrId, mLastDeathProcId);
	}
	
	if(death_svr_num == 0)
	{
		return randKillProc();
	}

	u8 rs = rand() % 2;
	if(rs == 0)
	{
		return randStartProc(); 
	}
	return randKillProc();
}


bool
AosTesterCubeGrpInfo::randStartProc()
{
	if(mDeathSvrProcId.size() == 0)	return true;
	
	u32 idx = rand() % mDeathSvrProcId.size();
	map<int, u32>::iterator itr = mDeathSvrProcId.begin();
	while(idx--) itr++;	
	aos_assert_r(itr != mDeathSvrProcId.end(), false);

	int svr_id = itr->first;
	u32 proc_id = itr->second;
	
	startProc(svr_id, proc_id);
	return true;
}
	

bool
AosTesterCubeGrpInfo::startProc(const int svr_id, const u32 proc_id)
{
	AosAppMsgPtr msg = OmnNew AosRlbTesterStartProcMsg(
		svr_id, AOSTAG_SVRPROXY_PID, proc_id);
	
	if(mShowLog)
	{
		OmnScreen << "RlbTesterClt; Monitor, startProc"
			<< "; svr_id:" << svr_id
			<< "; proc_id:" << proc_id 
			<< endl;
	}
	
	bool rslt = AosSendMsg(msg);
	aos_assert_r(rslt, false);
	
	map<int, u32>::iterator itr = mDeathSvrProcId.find(svr_id);
	mDeathSvrProcId.erase(itr);
	mActiveSvrProcId.insert(make_pair(svr_id, proc_id));
	return true;
}


bool
AosTesterCubeGrpInfo::randKillProc()
{
	if(mActiveSvrProcId.size() == 0)	return true;
	
	u32 idx = rand() % mActiveSvrProcId.size();
	map<int, u32>::iterator itr = mActiveSvrProcId.begin();
	while(idx--) itr++;	
	aos_assert_r(itr != mActiveSvrProcId.end(), false);

	int svr_id = itr->first;
	u32 proc_id = itr->second;
	
	AosAppMsgPtr msg = OmnNew AosRlbTesterKillProcMsg(
		svr_id, AOSTAG_SVRPROXY_PID, proc_id);
	
	if(mShowLog)
	{
		OmnScreen << "RlbTesterClt; Monitor, killProc"
			<< "; svr_id:" << svr_id
			<< "; proc_id:" << proc_id 
			<< endl;
	}

	bool rslt = AosSendMsg(msg);
	aos_assert_r(rslt, false);
	
	mActiveSvrProcId.erase(itr);
	mDeathSvrProcId.insert(make_pair(svr_id, proc_id));

	if(mActiveSvrProcId.size() == 0)
	{
		mLastDeathSvrId = svr_id;
		mLastDeathProcId = proc_id;
	}
	return true;
}

