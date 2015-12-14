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
// 10/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/RemoveMember.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosRemoveMember::AosRemoveMember(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_REMOVE_MEMBER, AosSeReqid::eRemoveMember, rflag)
{
}


bool 
AosRemoveMember::proc(const AosRundataPtr &rdata)
{
	// A user requests logging out. Logout means canceling the session.
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();

	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString args = root->getChildTextByAttr("name", "args");
	OmnString ctr_objid, objid;	
	AosParseArgs(args, "ctnr_objid", ctr_objid, "doc_objid", objid);

	AosXmlTagPtr ctr = AosDocClientObj::getDocClient()->getDocByObjid(ctr_objid, rdata);
	//aos_assert_r(ctr, false);
	if(!ctr)
	{
		AOSLOG_LEAVE(rdata);
		return false;	
	}

	//check remove member security
	bool rslt = false;
	/*
	bool rslt =  AosSecurityMgrSelf->checkDelMember(ctr_objid, rdata);
	if(!rslt)
	{
		rdata->setError();
		AOSLOG_LEAVE(rdata);
		return false;	
	}
	*/

	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	//aos_assert_r(doc, false);
	if(!doc)
	{
		AOSLOG_LEAVE(rdata);
		return false;	
	}

	OmnString pctrs = doc->getAttrStr(AOSTAG_MEMBEROF);
	//aos_assert_r(pctrs, false);
	if(pctrs == "")
	{
		AOSLOG_LEAVE(rdata);
		return false;	
	}

	OmnString pctr;
	OmnStrParser1 parser(pctrs, ",", false, false);
	pctrs = "";
	bool findPctr = false;
	while ((pctr = parser.nextWord()) != "")
	{
		if(pctr != ctr_objid)
		{
			if(pctrs != "") pctrs << ",";
			pctrs << pctr;
		}
		else
		{
			findPctr = true;
		}
	}

	if(!findPctr)
	{
		rdata->setError() << "the obj is not in this ctnr";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (pctrs == "") 
	{
		// The doc does not have any parent. Need to put it into the
		// lost&found.
		if (rdata->getUserid())
		{
			// Need to put the doc to the requester's private lostnfound
			OmnString cid = AosDocClientObj::getDocClient()->getCloudid(rdata->getUserid(), rdata);
			if (cid != "")
			{
				pctrs = AosObjid::compose(AOSTAG_CTNR_LOSTFOUND, cid);
			}
			else
			{
				rdata->setError() << "Missing cloud id";
				AOSLOG_LEAVE(rdata);
				return false;
			}
		}
		else
		{
			// Need to put the doc to the public lostnfound
			pctrs = AOSTAG_CTNR_LOSTFOUND;
		}
	}

	rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
			doc->getAttrU64(AOSTAG_DOCID, 0), 
			doc->getAttrStr(AOSTAG_OBJID),
			AOSTAG_MEMBEROF, pctrs, "", false, false, true);
	//aos_assert_r(rslt, false);
	if(!rslt)
	{
		AOSLOG_LEAVE(rdata);
		return false;
	}

	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

