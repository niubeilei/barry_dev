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
#if 0
This is not used anymore. Chen Ding, 02/10/2012
#include "SeReqProc/GetNewId.h"

#include "EventMgr/EventMgr.h"
#include "SEInterfaces/IILClientObj.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosGetNewId::AosGetNewId(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_GET_NEWID, AosSeReqid::eGetNewId, rflag)
{
}


bool 
AosGetNewId::proc(const AosRundataPtr &rdata)
{
	// It generates a batch of new IDs and returns the IDs through
	// the following message:
	//  <Contents>
	//      <rcd compid="xxx" size="xxx"/>
	//  </Contents>

	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	u32 blocksize;
	OmnString crtid = AosSeIdGenMgr::getSelf()->nextCompIdBlock(blocksize);
	if (!crtid)
	{
		AosSetError(rdata, "internal_error");
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString contents = "<Contents><rcd compid=\"";
	contents << crtid
		    << "\" size=\"" << blocksize << "\"/></Contents>";

	rdata->setResults(contents);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}	
#endif
