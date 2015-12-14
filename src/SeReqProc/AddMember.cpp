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
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/AddMember.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "ErrorMgr/ErrmsgId.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEBase/SeUtil.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosAddMember::AosAddMember(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_ADD_MEMBER, 
		AosSeReqid::eAddMember, rflag)
{
}


bool 
AosAddMember::proc(const AosRundataPtr &rdata)
{
	// This function is created by john 2010/12/20, modifyed by Ken 2011/01/16
	// This function adds a doc to a container on a sorted attribute.
	// If no sorted attribute is specified, it adds the objid.
	// The parent container, objid, and sorted attribute are
	// specified through the 'args' tag:
	//  ctnr_objid:xxx, doc_objid:xxx, sattr:xxx
	//
	// Adding a member to a container is done by adding an entry
	//  (sorted_attribute_value, docid)
	// to the container's sorted_attribute_list.
		
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString errmsg;
	OmnString args = root->getChildTextByAttr("name", "args");
	OmnString ctnr_objid, objid;
	AosParseArgs(args, "ctnr_objid", ctnr_objid, "doc_objid", objid);

	AOSLOG_LEAVE(rdata);
	AosXmlTagPtr ctnr = AosRetrieveDocByObjid(ctnr_objid, false, rdata);
	if (!ctnr)
	{
		AosSetError(rdata, "eContainerNotFound") << ctnr_objid;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool rslt = AosSecurityMgrSelf->checkAddMember1(ctnr, rdata);
	if (!rslt)
	{
		AOSLOG_LEAVE(rdata);
		return true;
	}

	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	aos_assert_r(doc, false);

	OmnString pctrs = doc->getAttrStr(AOSTAG_MEMBEROF);

	OmnStrParser1 parser(pctrs, ",", false, false);
	OmnString pctnrs;
	while ((pctnrs = parser.nextWord()) != "")
	{
		if(pctnrs == ctnr_objid)
		{
			errmsg << "objid has been in the container";
			rdata->setErrmsg(errmsg);
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}

	// If the existing container is LostAndFound, need to remove it.
	if (rdata->getUserid())
	{
		OmnString cid = AosDocClientObj::getDocClient()->getCloudid(rdata->getUserid(), rdata);
		if (cid != "")
		{
			OmnString pn = AosObjid::compose(AOSTAG_CTNR_LOSTFOUND, cid);
			if (pctrs == pn) pctrs = "";
		}
	}

	if (pctrs != "") pctrs << ",";
	pctrs << ctnr_objid;

	// Chen Ding, 2011/01/27
	// rslt =   AosDocServerSelf->modifyAttrStr(AOSAPPNAME_SYSTEM, mUserid,
	//          doc->getAttrU64(AOSTAG_DOCID, 0), AOSTAG_PARENTC,
	//          pctrs, "", false, false, errcode, errmsg);
	AOSLOG_LEAVE(rdata);
	rslt =  AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
				doc->getAttrU64(AOSTAG_DOCID, 0),
				doc->getAttrStr(AOSTAG_OBJID),
				AOSTAG_MEMBEROF,
				pctrs, "", false, false, true);
	
	AOSLOG_LEAVE(rdata);
	return rslt;
}
