////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 01/13/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Random/DbRand.h"

#include "Random/RandomUtil.h"
#include "SEInterfaces/QueryObj.h"
#include "SEInterfaces/QueryRsltObj.h"


OmnString 
AosDbRand::pickUserDomain(AosRundata *rdata)
{
	// This function randomly picks a user domain. 
	AosQueryRsltObjPtr values = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryObjPtr queryobj = AosQueryObj::getQueryObj();
	aos_assert_r(queryobj, "");
	bool rslt = queryobj->getUserDomains1(0, 10000, false, values, rdata);
	aos_assert_r(rslt, "");
	values->reset();

	OmnString userdomain;
	u64 uid;

	int total = values->getTotal(rdata);
	if (total <= 0) return "";
	int idx = OmnRandom::nextU32(0, total-1);
	for (int i=0; i<=idx; i++)
	{
		aos_assert_r(values->nextDocidValue(uid, userdomain, rdata), "");
	}

	return userdomain;
}


OmnString 
AosDbRand::pickContainer(AosRundata *rdata)
{
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryObjPtr queryobj = AosQueryObj::getQueryObj();
	aos_assert_r(queryobj, "");
	bool rslt = queryobj->getAllContainers(0, 10000, false, query_rslt, rdata);
	aos_assert_rr(rslt, rdata, "");
	int nn = query_rslt->getNumDocs();
	if (nn <= 0)
	{
		// There are no containers. 
		return "";
	}

	int idx = OmnRandom::nextU32(0, nn-1);
	u64 docid = query_rslt->getDocid(idx);
	aos_assert_rr(docid, rdata, "");

	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_rr(docclient, rdata, "");
	AosXmlTagPtr doc = docclient->getDocByDocid(docid, rdata);
	aos_assert_rr(doc, rdata, "");
	return doc->getAttrStr(AOSTAG_OBJID);
}

