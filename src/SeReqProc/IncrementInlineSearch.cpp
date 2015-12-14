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
// 08/11/2011	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/IncrementInlineSearch.h"

#include "SEInterfaces/DocClientObj.h"
#include "SEUtil/IILName.h"
#include "SEInterfaces/IILClientObj.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"


AosIncrementInlineSearch::AosIncrementInlineSearch(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_INCREMENTINLINESEARCH, AosSeReqid::eIncrementInlineSearch, rflag)
{
}


bool 
AosIncrementInlineSearch::proc(const AosRundataPtr &rdata)
{
	// This function check the whether the request is unique.
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	// It checks whether a value is unique. This is determined by the 
	// following:
	// 	[container, attribute]
	// which means that all the data in the container should hold
	// different values of the attribute 'attribute'. This function
	// checks whether a new value is unique. The parameters
	// are passed in through 'args', in the following format:
	// 		args="container:xxx, attr:xxx, value:xxx"
	// The function assumes empty value is not allowed.
	OmnString args = root->getChildTextByAttr("name", "args");
	OmnString ctnr, key, value, sid, docid;
	AosParseArgs(args, "ctnr", ctnr, "key", key, "value", value, "seqid", sid, "docid", docid);
	
	// Make sure the container is not empty
	if (ctnr == "")
	{
		AosSetError(rdata, "eMissingContainer");
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Make sure the attribute name is not empty
	if (key == "")
	{
		AosSetError(rdata, "eMissingAttrname");
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	// Make sure the value is not empty
	if (value == "")
	{
		AosSetError(rdata, "eMissingValue");
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	u64 seqid = atoll(sid.data());
	if((seqid >> 32) > 0)
	{
		rdata->setError() << "seqid error";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
	if(!doc)
	{
		rdata->setError() << "can't find the doc by docid :" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString iilname = AosIILName::composeInlineSearchContainerAttrIILName(ctnr, key);
	//bool rslt = AosIILClient::getSelf()->incrementInlineSearchValue(iilname, value, seqid, 1, rdata);
	bool rslt = AosIILClientObj::getIILClient()->incrementInlineSearchValue(iilname, value, seqid, 1, rdata);
	if(!rslt)
	{
		rdata->setError() << "can't increment the iil.";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString resp;
	resp << "<Contents>" << doc->toString() << "</Contents>";
	rdata->setResults(resp);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

