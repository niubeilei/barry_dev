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
// 07/23/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/CopyDocsFromCtnr.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SdocCopyDocsFromCtnr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosCopyDocsFromCtnrProc::AosCopyDocsFromCtnrProc(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_COPYDOCS_FROM_CTNR, 
		AosSeReqid::eCopyDocsFromCtnr, rflag)
{
}


bool 
AosCopyDocsFromCtnrProc::proc(const AosRundataPtr &rdata)
{
	// It copies all the docs in 'orig_objid' (which is a container)
	// to the container 'target_objid'. 
	//  <request ...>
	//      <item name="args">public:true|false, cid_required:true|false,
	//          orig_objid:xxx, target_objid:xxx</item>
	//  </request>

	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString contents;
	OmnString args = root->getChildTextByAttr("name", "args");
	if (args == "")
	{
		rdata->setError() << "Missing args!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString is_public, cid_required, orig_objid, target_objid;
	AosParseArgs(args, "public", is_public, "cidrequired", cid_required, 
			"orig_objid", orig_objid, "target_objid", target_objid);

	if (orig_objid == "")
	{
		rdata->setError() << "Missing the Originating container objid";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (target_objid == "")
	{
		rdata->setError() << "Missing the Target container objid"; 
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	/*
	bool rslt = AosSdocCopyDocsFromCtnr::copyDocs(orig_objid, target_objid, 
			(is_public == "true"), (cid_required == "true"), rdata);
	AOSLOG_LEAVE(rdata);
	return rslt;
	*/
	return false;
}

