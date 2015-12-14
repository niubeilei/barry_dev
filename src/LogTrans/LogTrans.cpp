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
// 2013/03/30 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "LogTrans/LogTrans.h"

#include "SeLogSvr/PhyLogSvr.h"


AosLogTrans::AosLogTrans(
		const AosTransType::E type,
		const bool regflag)
:
AosCubicTrans(type, regflag)
{
}


AosLogTrans::AosLogTrans(
		const AosTransType::E type,
		const u64 &pctr_docid,
		const bool need_save,
		const bool need_resp)
:
AosCubicTrans(type, pctr_docid, need_save, need_resp)
{
}


AosLogTrans::~AosLogTrans()
{
}


bool
AosLogTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosLogTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosLogTrans::proc()
{
	return AosPhyLogSvr::getSelf()->addTrans(this);
}

