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
#include "AdminTrans/AdminAddServerTrans.h"

#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SysMsg/AddServerMsg.h"
#include "Util/File.h"
#include "Porting/Sleep.h"

AosAdminAddServerTrans::AosAdminAddServerTrans(const bool regflag)
:
AosAdminTrans(AosTransType::eAdminAddServer, regflag)
{
}


AosAdminAddServerTrans::AosAdminAddServerTrans(
		const AosXmlTagPtr &svr_proxy_config,
		const AosXmlTagPtr &admin_config,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosAdminTrans(AosTransType::eAdminAddServer, svr_id, need_save, need_resp),
mSvrProxyConfig(svr_proxy_config),
mAdminConfig(admin_config)
{
}


AosAdminAddServerTrans::~AosAdminAddServerTrans()
{
}


bool
AosAdminAddServerTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAdminTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	OmnString str = buff->getOmnStr("");
	aos_assert_r(str != "", false);
	AosXmlParser parser;
	mSvrProxyConfig = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(mSvrProxyConfig, false);

	str = buff->getOmnStr("");
	aos_assert_r(str != "", false);
	mAdminConfig = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(mAdminConfig, false);

	return true;
}


bool
AosAdminAddServerTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAdminTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	aos_assert_r(mSvrProxyConfig, false);
	buff->setOmnStr(mSvrProxyConfig->toString());

	aos_assert_r(mAdminConfig, false);
	buff->setOmnStr(mAdminConfig->toString());
	return true;
}


AosTransPtr
AosAdminAddServerTrans::clone()
{
	return OmnNew AosAdminAddServerTrans(false);
}


bool
AosAdminAddServerTrans::proc()
{
OmnScreen << "****************************AdminAddServerTrans**********************" << endl;
	aos_assert_r(mSvrProxyConfig && mAdminConfig, false);
	//config admin server
	bool rslt = AosNetworkMgrObj::getNetworkMgr()->configSelfSvr(mAdminConfig);
	aos_assert_r(rslt, false);
	
	rslt = AosNetworkMgrObj::getNetworkMgr()->configServers(mAdminConfig);
	aos_assert_r(rslt, false);

	//config proxy
	AosAppMsgPtr msg = OmnNew AosAddServerMsg(
			mSvrProxyConfig, mAdminConfig, AosGetSelfServerId(), 0);
	aos_assert_r(msg, false);
	rslt = AosSendMsg(msg);
	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	return true;
}


