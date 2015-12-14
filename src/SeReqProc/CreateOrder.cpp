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
#include "SeReqProc/CreateOrder.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosCreateOrder::AosCreateOrder(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATE_ORDER, 
		AosSeReqid::eCreateOrder, rflag)
{
}


bool 
AosCreateOrder::proc(const AosRundataPtr &rdata)
{
	// This function is created by Phnix
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString sdoc_objids = root->getChildTextByAttr("name", "args");
	if (sdoc_objids != "")
	{
		AosEventMgr::getSelf()->procEvent(eAosHookCreateDoc,
						eAosHkptCreateOrder, sdoc_objids, rdata);
		AosSmartDocObj::procSmartdocsStatic(sdoc_objids, rdata);
		rdata->setOk();
	}
	else
	{
		rdata->setError() << "Missing smartdoc objid!";
	}
	AOSLOG_LEAVE(rdata);
	return true;
}
