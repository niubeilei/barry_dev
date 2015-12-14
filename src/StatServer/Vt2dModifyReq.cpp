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
// 2014/07/24 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StatServer/Vt2dModifyReq.h"

#include "API/AosApi.h"
#include "Thread/Sem.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadPool.h"
#include "Vector2D/Vector2D.h"
#include "Debug/Debug.h"




AosVt2dModifyReq::AosVt2dModifyReq(
		const AosRundataPtr &rdata,
		const OmnSemPtr &sem,
		const AosVector2DPtr &vt2d,
		AosVt2dModifyInfo &mdf_info,
		const AosBuffPtr &buff)
:
OmnThrdShellProc("Vt2dModifyReq"),
mRdata(rdata->clone(AosMemoryCheckerArgsBegin)),
mSem(sem),
mVt2d(vt2d),
mMdfInfo(mdf_info),
mBuff(buff)
{
}


bool
AosVt2dModifyReq::procFinished()
{
	mSem->post();
	return true;
}


bool
AosVt2dModifyReq::run()
{
	//OmnScreen << "AosVector2D numThrdsRunning:"  << mNumThrdsRunning << ";"<< endl;
	aos_assert_r(mVt2d, false);
	OmnTagFuncInfo << endl;
	return mVt2d->modify(mRdata.getPtr(), mMdfInfo, mBuff);

}


