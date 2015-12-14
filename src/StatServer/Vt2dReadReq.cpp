////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2014/8/11 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "StatServer/Vt2dReadReq.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Vector2D/Vector2D.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadPool.h"
#include "Vector2DQryRslt/Vt2dQryRslt.h"
#include "Vector2DQryRslt/Vt2dQryRsltProc.h"
#include "Debug/Debug.h"

AosVt2dReadReq::AosVt2dReadReq(
		const AosRundataPtr &rdata,
		const OmnSemPtr &sem,
		const AosVector2DPtr &vt2d,
		vector<AosStatTimeArea> &qry_time_areas,
		vector<u64> &stat_docids,
		const AosVt2dQryRsltProcPtr &qryrslt_proc,
		const AosVt2dQryRsltPtr &qry_rslt)
:
OmnThrdShellProc("Vt2dReadReq"),
mRdata(rdata->clone(AosMemoryCheckerArgsBegin)),
mSem(sem),
mVector2d(vt2d),
mQryTimeAreas(qry_time_areas),
mStatDocids(stat_docids),
mQryRsltProc(qryrslt_proc),
mQryRslt(qry_rslt)
{
}


AosVt2dReadReq::~AosVt2dReadReq()
{
}


bool
AosVt2dReadReq::procFinished()
{
	aos_assert_r(mSem, false);
	mSem->post();
	return true;
}



bool
AosVt2dReadReq::run()
{
	aos_assert_r(mVector2d, false);

	bool rslt = mVector2d->retrieve(mRdata, mStatDocids,
			mQryTimeAreas, mQryRslt, mQryRsltProc);
	aos_assert_r(rslt, false);
	OmnTagFuncInfo << endl;
	return true;
}

