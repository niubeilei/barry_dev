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
#include "AdminTrans/AdminStopProcessTrans.h"

#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SysMsg/StopProcessMsg.h"
#include "Util/File.h"

AosAdminStopProcessTrans::AosAdminStopProcessTrans(const bool regflag)
:
AosAdminTrans(AosTransType::eAdminStopProcess, regflag)
{
}


AosAdminStopProcessTrans::AosAdminStopProcessTrans(
		const int signal_no,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosAdminTrans(AosTransType::eAdminStopProcess, svr_id, need_save, need_resp),
mSignalNo(signal_no)
{
}


AosAdminStopProcessTrans::~AosAdminStopProcessTrans()
{
}


bool
AosAdminStopProcessTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAdminTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mSignalNo = buff->getInt(14);
	return true;
}


bool
AosAdminStopProcessTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAdminTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setInt(mSignalNo);
	return true;
}


AosTransPtr
AosAdminStopProcessTrans::clone()
{
	return OmnNew AosAdminStopProcessTrans(false);
}


bool
AosAdminStopProcessTrans::proc()
{
OmnScreen << "****************************AdminStopProcessTrans**********************" << endl;
	AosAppMsgPtr msg = OmnNew AosStopProcessMsg(mSignalNo, AosGetSelfServerId(), 0);
	aos_assert_r(msg, false);
	bool rslt = AosSendMsg(msg);
	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	return true;
}


