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
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/RegisterHook.h"

#include "EventMgr/EventMgr.h"
#include "Rundata/RdataUtil.h"
#include "Rundata/Rundata.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/SeReqUtil.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/Ptrs.h"
#include "Util/String.h"

AosRegisterHook::AosRegisterHook(const bool regflag)
:
AosSeRequestProc(AOSREQIDNAME_REG_HOOK, AosSeReqid::eRegisterHook, regflag)
{
}


bool 
AosRegisterHook::proc(const AosRundataPtr &rdata)
{
	// This function registers a hook. It assumes 'doc' is in the
	// following format:
	// 	<request ...>
	// 		<objdef>
	// 			<registers>
	//				<register hook="xxx" sdocobjid="xxx">key</register>
	//				<register hook="xxx" sdocobjid="xxx">key</register>
	//				...
	// 			</registers>
	// 		</objdef>
	// 	</term>
	//
	// Up to eMaxReqPerRequest number of smartdocs can be registered.
	AOSLOG_ENTER_R(rdata, false);
	rdata->setError();	

	AosSeReqProcPtr seproc = AosGetSeReqProc(rdata);
	aos_assert_r(seproc, false);

	AosXmlTagPtr objdef = AosRdataUtil::getObjdef(rdata);
	if (!objdef || (objdef = objdef->getFirstChild()))
	{
		rdata->setError() << "Missing registration data";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	int guard = eMaxRegisterPerRequest;
	AosXmlTagPtr def = objdef->getFirstChild();
	rdata->setOk();
	while (def && guard--)
	{
		OmnString hookstr = def->getAttrStr(AOSTAG_HOOK);
		OmnString sdoc_objid = def->getAttrStr(AOSTAG_SDOCOBJID);
		if (sdoc_objid == "")
		{
			rdata->setError() << "Smartdoc ID is empty: ";
			AOSLOG_LEAVE(rdata);
			return false;
		}

		OmnString key = def->getNodeText();
		AosEventMgr::getSelf()->registerSdoc(hookstr, key, sdoc_objid, rdata);
	}
	AOSLOG_LEAVE(rdata);
	return true;	
}

