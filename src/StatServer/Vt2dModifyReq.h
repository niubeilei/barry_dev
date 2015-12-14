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
#ifndef Aos_StatServer_Vt2dModifyReq_h
#define Aos_StatServer_Vt2dModifyReq_h

#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Vector2D/Ptrs.h"
#include "Vector2DUtil/Vt2dModifyInfo.h"


class ValueBlock;

class AosVt2dModifyReq : public  OmnThrdShellProc
{
	OmnDefineRCObject;

	AosRundataPtr 					mRdata;
	OmnSemPtr 						mSem;
	AosVector2DPtr					mVt2d;
	AosVt2dModifyInfo				mMdfInfo;
	AosBuffPtr						mBuff;

	ValueBlock*					mValblk;

public:
	AosVt2dModifyReq(
		const AosRundataPtr &rdata,
		const OmnSemPtr &sem,
		const AosVector2DPtr &vt2d,
		AosVt2dModifyInfo &mdf_info,
		const AosBuffPtr &buff);

	virtual bool run();
	virtual bool procFinished();

};
#endif

