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
// 2014/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoCall/EndPointMgr.h"

AosEndPointInfo  AosEndPointMgr::smInvalidEndpoint;
vector<AosEndPointInfo>	AosEndPointMgr::smEndpoints;

AosEndPointMgr::AosEndPointMgr(AosRundata *rdata)
{
	if (!init(rdata))
	{
		OmnThrowException("Failed creating EndpointMgr");
		return;
	}
}


AosEndPointMgr::~AosEndPointMgr()
{
}


bool
AosEndPointMgr::init(AosRundata *rdata)
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_rr(conf, rdata, false);

	AosXmlTagPtr tags = conf->getFirstChild("endpoints");
	aos_assert_rr(tags, rdata, false);

	AosXmlTagPtr tag = tags->getFirstChild();
	aos_assert_rr(tag, rdata, false);

	while (tag)
	{
		AosEndPointInfo info;
		bool rslt = info.init(rdata, tag);
		aos_assert_rr(rslt, rdata, false);
		smEndpoints.push_back(info);

		tag = tags->getNextChild();
	}

	return true;
}


