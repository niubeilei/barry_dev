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
#include "RlbTester/TesterTrans/RlbTesterIsProcDeathedTrans.h"

#include "API/AosApi.h"

AosRlbTesterIsProcDeathedTrans::AosRlbTesterIsProcDeathedTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eRlbTesterIsProcDeathed, regflag)
{
}


AosRlbTesterIsProcDeathedTrans::AosRlbTesterIsProcDeathedTrans(
		const int svr_id,
		const u32 proc_id)
:
AosTaskTrans(AosTransType::eRlbTesterIsProcDeathed, 
		svr_id, proc_id, false, false)
{
}


AosRlbTesterIsProcDeathedTrans::~AosRlbTesterIsProcDeathedTrans()
{
}


bool
AosRlbTesterIsProcDeathedTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosRlbTesterIsProcDeathedTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	return true;
}


AosTransPtr
AosRlbTesterIsProcDeathedTrans::clone()
{
	return OmnNew AosRlbTesterIsProcDeathedTrans(false);
}


bool
AosRlbTesterIsProcDeathedTrans::proc()
{
	// do nothing.
	return true;
}


