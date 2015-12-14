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
#include "AdminTrans/AdminStartProcessTrans.h"

#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SysMsg/StartProcessMsg.h"
#include "Util/File.h"

AosAdminStartProcessTrans::AosAdminStartProcessTrans(const bool regflag)
:
AosAdminTrans(AosTransType::eAdminStartProcess, regflag)
{
}


AosAdminStartProcessTrans::AosAdminStartProcessTrans(
		const OmnString &args,
		const AosXmlTagPtr &cluster_config,
		const AosXmlTagPtr &norm_config,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosAdminTrans(AosTransType::eAdminStartProcess, svr_id, need_save, need_resp),
mArgs(args),
mClusterConfig(cluster_config),
mNormConfig(norm_config)
{
}


AosAdminStartProcessTrans::~AosAdminStartProcessTrans()
{
}


bool
AosAdminStartProcessTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAdminTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mArgs = buff->getOmnStr("");

	OmnString str = buff->getOmnStr("");
	aos_assert_r(str != "", false);
	AosXmlParser parser;
	mClusterConfig = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(mClusterConfig, false);
	str = buff->getOmnStr("");
	aos_assert_r(str != "", false);
	mNormConfig = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(mNormConfig, false);
	return true;
}


bool
AosAdminStartProcessTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAdminTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mArgs);
	aos_assert_r(mClusterConfig, false);
	buff->setOmnStr(mClusterConfig->toString());
	aos_assert_r(mNormConfig, false);
	buff->setOmnStr(mNormConfig->toString());
	return true;
}


AosTransPtr
AosAdminStartProcessTrans::clone()
{
	return OmnNew AosAdminStartProcessTrans(false);
}


bool
AosAdminStartProcessTrans::proc()
{
	/*
OmnScreen << "****************************AdminStartProcessTrans**********************" << endl;
	aos_assert_r(mClusterConfig && mNormConfig, false);
	AosAppMsgPtr msg = OmnNew AosStartProcessMsg(mArgs, 
			mClusterConfig, mNormConfig, AosGetSelfServerId(), 0);
	aos_assert_r(msg, false);
	bool rslt = AosSendMsg(msg);
	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	*/
	return true;
}


