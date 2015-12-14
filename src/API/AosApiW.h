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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiW_h
#define AOS_API_ApiW_h

#include "SEInterfaces/DocClientObj.h"

extern AosDocClientObjPtr g__AosDocClient;

inline AosXmlTagPtr AosWriteLockDocByObjid(const OmnString &objid, AosRundata *rdata)
{
	if (!g__AosDocClient) g__AosDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(g__AosDocClient, 0);
	return g__AosDocClient->writeLockDocByObjid(objid, rdata);
}

inline AosXmlTagPtr AosWriteLockDocByDocid(const u64 &docid, AosRundata *rdata)
{
	if (!g__AosDocClient) g__AosDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(g__AosDocClient, 0);
	return g__AosDocClient->writeLockDocByDocid(docid, rdata);
}

#endif
