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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocSetVote.h"

#include "DbQuery/Query.h"
#include "Debug/Error.h"
#include "SEInterfaces/DocClientObj.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "QueryClient/QueryClient.h"
#include "Rundata/Rundata.h"
#include "Security/Session.h"
#include "Util/StrSplit.h"
#include "Util/SPtr.h"
#include "Util/Locale.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include <iostream>
using namespace std;

AosSdocSetVote::AosSdocSetVote(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_SETVOTE, AosSdocId::eSetVote, flag)
{
}


AosSdocSetVote::~AosSdocSetVote()
{
}

bool 
AosSdocSetVote::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// parseQuery
	// get attr by zky_startnum:
	// set attr like : zky_vote
	// <smartdoc 
	// ...
	// zky_startnum="xx"
	// zky_attrset = "zky_vote">
	//
	// <query
	// ....>
	// </query>
	//
	// </smartdoc>
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString run_doc = sdoc->getAttrStr(AOSTAG_RUN);
	if (run_doc != "no")
	{
		AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
				sdoc->getAttrU64(AOSTAG_DOCID, 0), 
				sdoc->getAttrStr(AOSTAG_OBJID), 
				AOSTAG_RUN, "no", "no", false, false, true);
	}

	AosQueryReqObjPtr query = AosQuery::parseQuery(sdoc, rdata);
	if (!query)
	{
		OmnAlarm  << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr query_xml = sdoc->getFirstChild(AOSTAG_QUERY);
	if (!query_xml)
	{
		rdata->setError() << "Missing Query Tag";
		return false;
	}

	bool rslt = AosQuery::doQuery(query, query_xml, rdata);
	aos_assert_rr(rslt, rdata, false);

	OmnString zky_startnum = sdoc->getAttrStr(ZKY_STARTNUM, "0");
	OmnString zky_attrset = sdoc->getAttrStr(ZKY_ATTRSET, "");
	if (zky_attrset != "")
	{
		AosXmlParser parser;
		AosXmlTagPtr queryResultXml = parser.parse(rdata->getResults(), "" AosMemoryCheckerArgs);
		if(!queryResultXml)
		{
			rdata->setError() << "Failed to Retrieve Query Result";
			return false;
		}
		AosXmlTagPtr record = queryResultXml->getFirstChild("record");
		if (!record)
		{
			rdata->setError() << "No Record in QueryResult";
			return false;
		}
		while(record)
		{
			record->setAttr(zky_attrset, zky_startnum);
			AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
				record->getAttrU64(AOSTAG_DOCID, 0), 
				record->getAttrStr(AOSTAG_OBJID), 
				zky_attrset, zky_startnum, "", false, false, true);
			record = queryResultXml->getNextChild();
		}
	}

	return true;
}

