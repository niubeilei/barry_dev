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
// 03/24/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SysMsg/ReadFmtTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/FmtMgrObj.h"

AosReadFmtTrans::AosReadFmtTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eReadFmt, regflag)
{
	setIsSystemTrans(); 
}


AosReadFmtTrans::AosReadFmtTrans(
		const int svr_id,
		const u32 cube_grp,
		const int file_seq,
		const u64 beg_fmt_id,
		const u64 end_fmt_id)
:
AosTaskTrans(AosTransType::eReadFmt, svr_id, 
		AosProcessType::eCube, cube_grp, false, true),
mCubeGrpId(cube_grp),
mFileSeq(file_seq),
mBegFmtId(beg_fmt_id),
mEndFmtId(end_fmt_id)
{
	setIsSystemTrans(); 
}


AosReadFmtTrans::~AosReadFmtTrans()
{
}


bool
AosReadFmtTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mCubeGrpId = buff->getU32(0);
	mFileSeq = buff->getInt(-1);
	mBegFmtId = buff->getU64(0);
	mEndFmtId = buff->getU64(0);
	return true;
}


bool
AosReadFmtTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU32(mCubeGrpId);
	buff->setInt(mFileSeq);
	buff->setU64(mBegFmtId);
	buff->setU64(mEndFmtId);
	return true;
}


//AosConnMsgPtr
AosTransPtr
AosReadFmtTrans::clone()
{
	return OmnNew AosReadFmtTrans(false);
}


bool
AosReadFmtTrans::proc()
{
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert_r(fmt_mgr, false);
	
	bool finish;
	AosBuffPtr data_buff;
	bool rslt = fmt_mgr->readFmtBySync(mRdata.getPtrNoLock(), mCubeGrpId, mBegFmtId, 
		 mEndFmtId, mFileSeq, finish, data_buff);

	AosBuffPtr resp_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU8(finish);
	resp_buff->setInt(mFileSeq);

	if(data_buff)
	{
		resp_buff->setBuff(data_buff);
	}

	sendResp(resp_buff);
	return true;
}


