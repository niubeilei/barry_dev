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
#include "SeReqProc/OneKeyCopy.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEModules/CodeComp.h"
#include "SEModules/Ptrs.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosOneKeyCopy::AosOneKeyCopy(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_ONE_KEY_COPY, AosSeReqid::eOneKeyCopy, rflag)
{
}


bool 
AosOneKeyCopy::proc(const AosRundataPtr &rdata)
{
	// This proc creates a new VPD and all the ones the VPD uses.
	// The input is in the following format:
	// 	<request ...>
	// 		<objdef>
	// 			<onekeycopy>
	// 				<homepage>the objid of the vpd to be copied</homepage>
	// 				<prefix>the objid generation type</prefix>
	// 				<strtag>the string used in objid generation</strtag>
	// 				<container>the objid of the container for all the VPDs</container>
	// 				<tags>the tags for all the vpds</tags>
	// 				<memberof>the memberof</memberof>
	// 			</onekeycopy>
	// 		</objdef>
	//
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();

	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr child = root->getFirstChild("objdef");
	aos_assert_r(child,false);

	AosXmlTagPtr child1 = child->getFirstChild();
    aos_assert_r(child1,false);

	AosXmlTagPtr vpd = child1->getFirstChild("homepage");
	aos_assert_r(vpd,false);
	OmnString objid = vpd->getNodeText();

	AosXmlTagPtr prefix = child1->getFirstChild("prefix");
	aos_assert_r(prefix,false);
	OmnString fix_type = prefix->getNodeText();

	AosXmlTagPtr strtag = child1->getFirstChild("strtag");
	aos_assert_r(strtag, false);
	OmnString stradd = strtag->getNodeText();

	OmnString ctnr_objid;
	AosXmlTagPtr tag = child1->getFirstChild("container");
	if (tag) ctnr_objid = tag->getNodeText();

	OmnString tags;
	tag = child1->getFirstChild("tags");
	if (tag) tags = tag->getNodeText();

	AosSessionObjPtr ss = rdata->getSession();
	AosCodeComp codeComp;
	bool rslt = codeComp.copyVpd(rdata, objid, fix_type, stradd, tags, ctnr_objid);
	
	if (!rslt)
	{
		AosSetError(rdata, "eInternalError");
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

