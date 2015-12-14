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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocTrans/BatchDelDocsTrans.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"

AosBatchDelDocsTrans::AosBatchDelDocsTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eBatchDelDocs, regflag)
{
}


AosBatchDelDocsTrans::AosBatchDelDocsTrans(
		const OmnString &scanner_id,
		const u32 svr_id,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(AosTransType::eBatchDelDocs, svr_id, need_save, need_resp),
mScannerId(scanner_id)
{
}


AosBatchDelDocsTrans::~AosBatchDelDocsTrans()
{
}


bool
AosBatchDelDocsTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mScannerId = buff->getOmnStr("");
	aos_assert_r(mScannerId != "", false);
	return true;
}


bool
AosBatchDelDocsTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mScannerId);
	return true;
}


AosTransPtr
AosBatchDelDocsTrans::clone()
{
	return OmnNew AosBatchDelDocsTrans(false);
}


bool
AosBatchDelDocsTrans::proc()
{
	// The request is in the form:
	// 	<trans AOSTAG_VIRTUAL_ID="xxx"
	// 		AOSTAG_SIZEID="xxx"
	// 	</trans>

	bool rslt =  AosDocSvr::getSelf()->deleteBatchDocs(mScannerId, mRdata);
	if (!rslt)
	{
		AosSetErrorU(mRdata, "no_file_mgr") << ": " << mScannerId;
		OmnAlarm << mRdata->getErrmsg() << enderr;
		return false;
	}
	aos_assert_rr(rslt, mRdata, false);
	mRdata->setOk();
	return true;
}

#endif
